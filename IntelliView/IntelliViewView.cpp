/* Copyright (C) 2024-2025 Stefan-Mihai MOGA
This file is part of IntelliView application developed by Stefan-Mihai MOGA.
IntelliView is an alternative Windows version to the famous Microsoft Photo!

IntelliView is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliView is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliView. If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

// IntelliViewView.cpp : implementation of the CIntelliViewView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "IntelliView.h"
#endif

#include "IntelliViewDoc.h"
#include "IntelliViewView.h"
#include "QOIPP.h"

#include <afxwin.h>
#include <afxext.h>
#include <afxctl.h>
#include <afxcmn.h>
#include <afxcontrolbars.h>
#include <afxtaskdialog.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CIntelliViewView

IMPLEMENT_DYNCREATE(CIntelliViewView, CScrollView)

BEGIN_MESSAGE_MAP(CIntelliViewView, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CIntelliViewView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_REGISTERED_MESSAGE(AFX_WM_DRAW2D, &CIntelliViewView::OnDraw2D)
	ON_REGISTERED_MESSAGE(AFX_WM_RECREATED2DRESOURCES, &CIntelliViewView::OnRecreatedResources)
	ON_COMMAND(ID_ZOOMIN, &CIntelliViewView::OnZoomIn)
	ON_UPDATE_COMMAND_UI(ID_ZOOMIN, &CIntelliViewView::OnUpdateZoomIn)
	ON_COMMAND(ID_ZOOMOUT, &CIntelliViewView::OnZoomOut)
	ON_UPDATE_COMMAND_UI(ID_ZOOMOUT, &CIntelliViewView::OnUpdateZoomOut)
	ON_COMMAND(ID_ACTUAL_SIZE, &CIntelliViewView::OnActualSize)
	ON_UPDATE_COMMAND_UI(ID_ACTUAL_SIZE, &CIntelliViewView::OnUpdateActualSize)
	ON_COMMAND(ID_BESTFIT, &CIntelliViewView::OnBestFit)
	ON_UPDATE_COMMAND_UI(ID_BESTFIT, &CIntelliViewView::OnUpdateBestFit)
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CIntelliViewView construction/destruction

CIntelliViewView::CIntelliViewView() noexcept : m_ImageType{ IMAGE_TYPE::UNDEFINED },
	m_nActivePage{ -1 },
	m_nFrames{ 0 },
	m_nCurrentZoomLevelX{ 100 },
	m_bSizeToFit{ theApp.m_bSizeToFit },
	m_nSlideshowTimerID{ 0 },
	m_nAnimationTimerID{ 0 },
	m_nNextFrameIndex{ 0 },
	m_nTotalLoopCount{ 0 },
	m_nLoopNumber{ 0 },
	m_bHasLoop{ false },
	m_FrameDisposal{ DISPOSAL_METHOD::UNDEFINED },
	m_nFrameDelay{ 0 },
	m_cxGifImage{ 0 },
	m_cyGifImage{ 0 },
	m_FramePosition{},
	m_BackgroundColor{ D2D1::ColorF{0, 0.0f} },
	m_nOrientationFlag{ 0 }
{
	EnableD2DSupport(TRUE);
}

CIntelliViewView::~CIntelliViewView()
{
}

BOOL CIntelliViewView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

// CIntelliViewView drawing

void CIntelliViewView::OnDraw(CDC* /*pDC*/)
{
	CIntelliViewDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}

void CIntelliViewView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

// CIntelliViewView printing

CSize CIntelliViewView::DrawSize(_In_ const CRect& rClient)
{
	//Validate our parameters
#pragma warning(suppress: 26496)
	AFXASSUME(m_pImage.get() != nullptr);

	const CD2DSizeU imageSize{ m_pImage->GetPixelSize() };
	if (m_bSizeToFit)
	{
#pragma warning(suppress: 26472)
		if ((imageSize.width < static_cast<UINT32>(rClient.Width())) && (imageSize.height < static_cast<UINT32>(rClient.Height())))
		{
			m_nCurrentZoomLevelX = rClient.Width() * 100 / imageSize.width;
			const UINT nZoom{ rClient.Height() * 100 / imageSize.height };
			if (nZoom < m_nCurrentZoomLevelX)
				m_nCurrentZoomLevelX = nZoom;
		}
#pragma warning(suppress: 26472)
		else if ((imageSize.width > static_cast<UINT32>(rClient.Width())) || (imageSize.height > static_cast<UINT32>(rClient.Height())))
		{
			m_nCurrentZoomLevelX = rClient.Width() * 100 / imageSize.width;
			const UINT nZoom{ rClient.Height() * 100 / imageSize.height };
			if (nZoom < m_nCurrentZoomLevelX)
				m_nCurrentZoomLevelX = nZoom;
		}
	}

#pragma warning(suppress: 26472)
	return { static_cast<int>(imageSize.width * m_nCurrentZoomLevelX / 100), static_cast<int>(imageSize.height * m_nCurrentZoomLevelX / 100) };
}

void CIntelliViewView::StopAnimation()
{
	if (m_nAnimationTimerID != 0)
	{
		KillTimer(m_nAnimationTimerID);
		m_nAnimationTimerID = 0;
	}
}

bool CIntelliViewView::CreateRenderTargetIfNecessary()
{
	if (m_pRenderTarget == nullptr)
		return false;
	ASSERT_VALID(m_pRenderTarget);
	CHwndRenderTarget* pHwndRenderTarget{ GetRenderTarget() };
	if (pHwndRenderTarget == nullptr)
		return false;
	if (!pHwndRenderTarget->IsValid())
	{
		pHwndRenderTarget->Create(GetSafeHwnd());
		if (!pHwndRenderTarget->IsValid())
			return false;
	}
	return true;
}

#pragma warning(suppress: 26429)
HRESULT CIntelliViewView::LoadQOI(_In_z_ LPCTSTR lpszPathName, _In_ ID2D1RenderTarget* pRenderTarget, _Inout_ std::unique_ptr<CD2DBitmap>& bitmap)
{
	// Validate our parameters
	ASSERT(pRenderTarget != nullptr);

	auto pD2DState{ AfxGetD2DState() };
	if (pD2DState == nullptr)
		return E_FAIL;
	IWICImagingFactory* pFactory{ pD2DState->GetWICFactory() };
	if (pFactory == nullptr)
		return E_FAIL;
	QOI::Desc desc{};
	auto data{ QOI::read(lpszPathName, 4, desc) };
	if (data.size() == 0)
		return E_FAIL;

	// Convert the straight RGBA data returned from qoi_read to premultiplied RGBA format which D2D requires for bitmaps
#pragma warning(suppress: 26429)
	size_t nDataIndex{ 0 };
	for (UINT i{ 0 }; i < desc.nWidth; i++)
	{
		for (UINT j{ 0 }; j < desc.nHeight; j++)
		{
#pragma warning(suppress: 26446 26472)
			data[nDataIndex] = static_cast<BYTE>(static_cast<int>(data[nDataIndex]) * data[nDataIndex + size_t{ 3 }] / 255);
#pragma warning(suppress: 26446 26472)
			data[nDataIndex + 1] = static_cast<BYTE>(static_cast<int>(data[nDataIndex + size_t{ 1 }]) * data[nDataIndex + size_t{ 3 }] / 255);
#pragma warning(suppress: 26446 26472)
			data[nDataIndex + 2] = static_cast<BYTE>(static_cast<int>(data[nDataIndex + size_t{ 2 }]) * data[nDataIndex + size_t{ 3 }] / 255);
			nDataIndex += 4;
		}
	}

	const UINT nStride{ desc.nWidth * 4 };
	const UINT nBufferSize{ desc.nHeight * nStride };
	ATL::CComPtr<IWICBitmap> pEmbeddedBitmap;
	HRESULT hr{ pFactory->CreateBitmapFromMemory(desc.nWidth, desc.nHeight, GUID_WICPixelFormat32bppPRGBA, nStride,
												nBufferSize, data.data(), &pEmbeddedBitmap) };
	if (FAILED(hr))
		return hr;
	ID2D1Bitmap* pTempRawFrame{ nullptr };
	hr = pRenderTarget->CreateBitmapFromWicBitmap(pEmbeddedBitmap, nullptr, &pTempRawFrame);
	if (FAILED(hr))
		return hr;
	bitmap = std::make_unique<CD2DBitmap>(nullptr, UINT{ 42 }, nullptr, CD2DSizeU{ 0, 0 }, FALSE); //Required because "default" style constructor is protected!
	bitmap->Attach(pTempRawFrame);

#ifdef _DEBUG
	// Test code here to test the QOI++ file writing functionality
	auto data2{ QOI::read(lpszPathName, 4, desc) };
	if (data2.size())
	{
		desc.nChannels = 4;
		QOI::write(_T("d:\\temp\\bla.qoi"), data2.data(), desc);
	}
#endif //#ifdef _DEBUG

	return S_OK;
}

HRESULT CIntelliViewView::GetGlobalMetadata()
{
	// Validate our parameters
	ASSERT(m_pDecoder != nullptr);

	// Get the frame count
	PROPVARIANT propValue;
#pragma warning(suppress: 26486)
	PropVariantInit(&propValue);
	HRESULT hr{ m_pDecoder->GetFrameCount(&m_nFrames) };
	if (FAILED(hr))
		return hr;

	// Create a MetadataQueryReader from the decoder
	ATL::CComPtr<IWICMetadataQueryReader> pMetadataQueryReader;
	hr = m_pDecoder->GetMetadataQueryReader(&pMetadataQueryReader);
	if (FAILED(hr))
	{
		// If we have no decoder-level query reader then try getting it from the first frame
		ATL::CComPtr<IWICBitmapFrameDecode> pWicFrame;
		if (SUCCEEDED(m_pDecoder->GetFrame(0, &pWicFrame)))
		{
			ATL::CComPtr<IWICMetadataQueryReader> pMetadataQueryReader2;
			if (SUCCEEDED(pWicFrame->GetMetadataQueryReader(&pMetadataQueryReader2)))
				GetOrientationData(pMetadataQueryReader2);
		}
		return hr;
	}

	// Get the orientation data from the decoder-level query reader if we can
	GetOrientationData(pMetadataQueryReader);

	// Get background color
	if (FAILED(GetGifBackgroundColor(pMetadataQueryReader)))
	{
		//Default to transparent if failed to get the color
		m_BackgroundColor = D2D1::ColorF{ 0, 0.0f };
	}

	// Get width
	hr = pMetadataQueryReader->GetMetadataByName(L"/logscrdesc/Width", &propValue);
	if (FAILED(hr))
		return hr;
	if (propValue.vt != VT_UI2)
	{
		PropVariantClear(&propValue);
		return E_FAIL;
	}
	m_cxGifImage = propValue.uiVal;
	PropVariantClear(&propValue);

	// Get height
	hr = pMetadataQueryReader->GetMetadataByName(L"/logscrdesc/Height", &propValue);
	if (FAILED(hr))
		return hr;
	if (propValue.vt != VT_UI2)
	{
		PropVariantClear(&propValue);
		return E_FAIL;
	}
	m_cyGifImage = propValue.uiVal;
	PropVariantClear(&propValue);

	// First check to see if the application block in the Application Extension
	// contains "NETSCAPE2.0" and "ANIMEXTS1.0", which indicates the gif animation
	// has looping information associated with it.

	// If we fail to get the looping information, loop the animation infinitely.
	if (SUCCEEDED(pMetadataQueryReader->GetMetadataByName(L"/appext/application", &propValue)) &&
		propValue.vt == (VT_UI1 | VT_VECTOR) &&
		propValue.caub.cElems == 11 &&  // Length of the application block
		(!memcmp(propValue.caub.pElems, "NETSCAPE2.0", propValue.caub.cElems) || !memcmp(propValue.caub.pElems, "ANIMEXTS1.0", propValue.caub.cElems)))
	{
		PropVariantClear(&propValue);
		hr = pMetadataQueryReader->GetMetadataByName(L"/appext/data", &propValue);
		if (SUCCEEDED(hr))
		{
			//The data is in the following format:
			//byte 0: extsize (must be > 1)
			//byte 1: loopType (1 == animated gif)
			//byte 2: loop count (least significant byte)
			//byte 3: loop count (most significant byte)
			//byte 4: set to zero
#pragma warning(suppress: 26481 26489)
			if (propValue.vt == (VT_UI1 | VT_VECTOR) && (propValue.caub.cElems >= 4) && (propValue.caub.pElems[0] > 0) && (propValue.caub.pElems[1] == 1))
			{
#pragma warning(suppress: 26481 26489)
				m_nTotalLoopCount = MAKEWORD(propValue.caub.pElems[2], propValue.caub.pElems[3]);

				//If the total loop count is not zero, we then have a loop count. If it is 0, then we repeat infinitely
				if (m_nTotalLoopCount != 0)
					m_bHasLoop = true;
			}
		}
	}
	PropVariantClear(&propValue);
	return hr;
}

#pragma warning(suppress: 26429)
HRESULT CIntelliViewView::GetRawFrame(_In_ CRenderTarget* pRenderTarget, _In_ UINT uFrameIndex, _Inout_ std::unique_ptr<CD2DBitmap>& pGifRawFrame)
{
	// Validate our parameters
	ASSERT(pRenderTarget != nullptr);
	ASSERT(m_pDecoder != nullptr);

	// Retrieve the current frame
	ATL::CComPtr<IWICBitmapFrameDecode> pWicFrame;
	HRESULT hr{ m_pDecoder->GetFrame(uFrameIndex, &pWicFrame) };
	if (FAILED(hr))
		return hr;
	auto pD2DState{ AfxGetD2DState() };
	if (pD2DState == nullptr)
		return E_FAIL;
	IWICImagingFactory* pFactory{ pD2DState->GetWICFactory() };
	if (pFactory == nullptr)
		return E_FAIL;
	ATL::CComPtr<IWICFormatConverter> pConverter;
	hr = pFactory->CreateFormatConverter(&pConverter);
	if (FAILED(hr))
		return hr;
	// Format convert to 32bppPBGRA which D2D expects
	hr = pConverter->Initialize(pWicFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);
	if (FAILED(hr))
		return hr;

	// Create a D2D bitmap from IWICBitmapSource
#pragma warning(suppress: 26429)
	ID2D1RenderTarget* pRT{ pRenderTarget->GetRenderTarget() };
	ASSERT(pRT != nullptr);
	ID2D1Bitmap* pTempRawFrame{ nullptr };
	hr = pRT->CreateBitmapFromWicBitmap(pConverter, nullptr, &pTempRawFrame);
	if (FAILED(hr))
		return hr;
	if (pGifRawFrame != nullptr)
		pGifRawFrame.reset();
	pGifRawFrame = std::make_unique<CD2DBitmap>(nullptr, UINT{ 42 }, nullptr, CD2DSizeU{ 0, 0 }, FALSE); //Required because "default" style constructor is protected!
	pGifRawFrame->Attach(pTempRawFrame);

	// Get Metadata Query Reader from the frame
	ATL::CComPtr<IWICMetadataQueryReader> pFrameMetadataQueryReader;
	hr = pWicFrame->GetMetadataQueryReader(&pFrameMetadataQueryReader);
	if (FAILED(hr))
		return hr;

	// Get the metadata for the current frame
	PROPVARIANT propValue;
#pragma warning(suppress: 26486)
	PropVariantInit(&propValue);
	hr = pFrameMetadataQueryReader->GetMetadataByName(L"/imgdesc/Left", &propValue);
	if (SUCCEEDED(hr))
	{
		hr = (propValue.vt == VT_UI2 ? S_OK : E_FAIL);
		if (SUCCEEDED(hr))
			m_FramePosition.left = static_cast<FLOAT>(propValue.uiVal);
		PropVariantClear(&propValue);
	}
	if (SUCCEEDED(hr))
	{
		hr = pFrameMetadataQueryReader->GetMetadataByName(L"/imgdesc/Top", &propValue);
		if (SUCCEEDED(hr))
		{
			hr = (propValue.vt == VT_UI2 ? S_OK : E_FAIL);
			if (SUCCEEDED(hr))
				m_FramePosition.top = static_cast<FLOAT>(propValue.uiVal);
			PropVariantClear(&propValue);
		}
	}
	if (SUCCEEDED(hr))
	{
		hr = pFrameMetadataQueryReader->GetMetadataByName(L"/imgdesc/Width", &propValue);
		if (SUCCEEDED(hr))
		{
			hr = (propValue.vt == VT_UI2 ? S_OK : E_FAIL);
			if (SUCCEEDED(hr))
				m_FramePosition.right = static_cast<FLOAT>(propValue.uiVal) + m_FramePosition.left;
			PropVariantClear(&propValue);
		}
	}
	if (SUCCEEDED(hr))
	{
		hr = pFrameMetadataQueryReader->GetMetadataByName(L"/imgdesc/Height", &propValue);
		if (SUCCEEDED(hr))
		{
			hr = (propValue.vt == VT_UI2 ? S_OK : E_FAIL);
			if (SUCCEEDED(hr))
				m_FramePosition.bottom = static_cast<FLOAT>(propValue.uiVal) + m_FramePosition.top;
			PropVariantClear(&propValue);
		}
	}
	if (SUCCEEDED(hr))
	{
		//Get delay from the optional Graphic Control Extension
		if (SUCCEEDED(pFrameMetadataQueryReader->GetMetadataByName(L"/grctlext/Delay", &propValue)))
		{
			hr = (propValue.vt == VT_UI2 ? S_OK : E_FAIL);
			if (SUCCEEDED(hr))
			{
				//Convert the delay retrieved in 10 ms units to a delay in 1 ms units
				hr = UIntMult(propValue.uiVal, 10, &m_nFrameDelay);
			}
			PropVariantClear(&propValue);
		}
		else
		{
			//Failed to get delay from graphic control extension. Possibly a single frame image (non-animated gif)
			m_nFrameDelay = 0;
		}

		//If no frame delay was provided from the image, then default to something sensible
		if (m_nFrameDelay == 0)
			m_nFrameDelay = 20;
	}
	if (SUCCEEDED(hr))
	{
		if (SUCCEEDED(pFrameMetadataQueryReader->GetMetadataByName(L"/grctlext/Disposal", &propValue)))
		{
			hr = (propValue.vt == VT_UI1) ? S_OK : E_FAIL;
			if (SUCCEEDED(hr))
				m_FrameDisposal = static_cast<DISPOSAL_METHOD>(propValue.bVal);
		}
		else
		{
			//Failed to get the disposal method, use default. Possibly a non-animated gif
			m_FrameDisposal = DISPOSAL_METHOD::UNDEFINED;
		}
	}
	PropVariantClear(&propValue);
	return hr;
}

#pragma warning(suppress: 26429)
HRESULT CIntelliViewView::GetOrientationData(_In_ IWICMetadataQueryReader* pMetadataQueryReader)
{
	// Validate our parameters
	ASSERT(m_pDecoder != nullptr);

	PROPVARIANT propVariant;
#pragma warning(suppress: 26486)
	PropVariantInit(&propVariant);
	HRESULT hr{ pMetadataQueryReader->GetMetadataByName(L"/app1/ifd/{ushort=274}", &propVariant) }; //Try JPEG EXIF
	if (FAILED(hr))
		hr = pMetadataQueryReader->GetMetadataByName(L"/ifd/{ushort=274}", &propVariant);
	if (FAILED(hr))
		return hr;
	if (propVariant.vt != VT_UI2)
	{
		PropVariantClear(&propVariant);
		return E_FAIL;
	}
	m_nOrientationFlag = propVariant.uiVal;
	PropVariantClear(&propVariant);
	return hr;
}

#pragma warning(suppress: 26429)
HRESULT CIntelliViewView::GetGifBackgroundColor(_In_ IWICMetadataQueryReader* pMetadataQueryReader)
{
	// If we have a global palette, get the palette and background color
	PROPVARIANT propVariant;
#pragma warning(suppress: 26486)
	PropVariantInit(&propVariant);
	HRESULT hr{ pMetadataQueryReader->GetMetadataByName(L"/logscrdesc/GlobalColorTableFlag", &propVariant) };
	if (FAILED(hr))
		return hr;
	if ((propVariant.vt != VT_BOOL) || !propVariant.boolVal)
	{
		PropVariantClear(&propVariant);
		return E_FAIL;
	}
	PropVariantClear(&propVariant);

	// Background color index
	hr = pMetadataQueryReader->GetMetadataByName(L"/logscrdesc/BackgroundColorIndex", &propVariant);
	if (FAILED(hr))
		return hr;
	if (propVariant.vt != VT_UI1)
	{
		PropVariantClear(&propVariant);
		return E_FAIL;
	}
	const BYTE backgroundIndex{ propVariant.bVal };
	PropVariantClear(&propVariant);

	// Get the color from the palette
	auto pD2DState{ AfxGetD2DState() };
	if (pD2DState == nullptr)
		return E_FAIL;
	IWICImagingFactory* pFactory{ pD2DState->GetWICFactory() };
	if (pFactory == nullptr)
		return E_FAIL;
	ATL::CComPtr<IWICPalette> pWicPalette;
	hr = pFactory->CreatePalette(&pWicPalette);
	if (FAILED(hr))
		return hr;

	// Get the global palette
	hr = m_pDecoder->CopyPalette(pWicPalette);
	if (FAILED(hr))
		return hr;
	std::array<WICColor, 256> rgColors{};
	UINT cColorsCopied{ 0 };
#pragma warning(suppress: 26485)
	hr = pWicPalette->GetColors(256, rgColors.data(), &cColorsCopied);
	if (FAILED(hr))
		return hr;

	// Check whether background color is outside range
	if (backgroundIndex >= cColorsCopied)
		return E_FAIL;

	// Get the color in ARGB format
#pragma warning(suppress: 26446 26482)
	const WICColor dwBGColor{ rgColors[backgroundIndex] };

	// The background color is in ARGB format, and we want to extract the alpha value and convert it in FLOAT
	const FLOAT alpha{ (dwBGColor >> 24) / 255.f };
	m_BackgroundColor = D2D1::ColorF{ dwBGColor, alpha };

	return S_OK;
}

#pragma warning(suppress: 26440)
HRESULT CIntelliViewView::RestoreSavedGifFrame()
{
	if (m_pGifSavedFrame == nullptr)
		return E_FAIL;
	CD2DBitmap frameToCopyTo{ GetRenderTarget(), UINT{42}, nullptr, CD2DSizeU{0, 0}, FALSE }; //Required because "default" style constructor is protected!
	if (!m_FrameComposeRT.GetBitmap(frameToCopyTo))
		return E_FAIL;
	// Copy the whole bitmap
	return frameToCopyTo.CopyFromBitmap(m_pGifSavedFrame.get());
}

#pragma warning(suppress: 26440)
HRESULT CIntelliViewView::ClearCurrentGifFrameArea()
{
	// Validate our parameters
	ASSERT(m_FrameComposeRT.IsValid());

	m_FrameComposeRT.BeginDraw();

	// Clip the render target to the size of the raw frame
	m_FrameComposeRT.PushAxisAlignedClip(&m_FramePosition, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	m_FrameComposeRT.Clear(m_BackgroundColor);

	// Remove the clipping
	m_FrameComposeRT.PopAxisAlignedClip();

	return m_FrameComposeRT.EndDraw();
}

#pragma warning(suppress: 26440)
HRESULT CIntelliViewView::DisposeCurrentGifFrame()
{
	HRESULT hr{ S_OK };
	switch (m_FrameDisposal)
	{
		case DISPOSAL_METHOD::UNDEFINED: //Deliberate fallthrough
		case DISPOSAL_METHOD::NONE:
		{
			//We simply draw on the previous frames. Do nothing here
			break;
		}
		case DISPOSAL_METHOD::BACKGROUND:
		{
			//Dispose background. Clear the area covered by the current raw frame with background color
			hr = ClearCurrentGifFrameArea();
			break;
		}
		case DISPOSAL_METHOD::PREVIOUS:
		{
			//Dispose previous. We restore the previous composed frame first
			hr = RestoreSavedGifFrame();
			break;
		}
		default:
		{
			//Invalid disposal method
			hr = E_FAIL;
		}
	}
	return hr;
}

HRESULT CIntelliViewView::OverlayNextGifFrameForCopy(_In_ CRenderTarget* pRenderTarget, _Inout_ CBitmapRenderTarget& FrameComposeRT)
{
	// Validate our parameters
	ASSERT(FrameComposeRT.IsValid());

	// Get Frame information
#pragma warning(suppress: 26429)
	std::unique_ptr<CD2DBitmap> pGifRawFrame;
	HRESULT hr{ GetRawFrame(pRenderTarget, m_nNextFrameIndex, pGifRawFrame) };
	if (FAILED(hr))
		return hr;

	// Start producing the next bitmap
	FrameComposeRT.BeginDraw();

	// If starting a new animation loop
	if (m_nNextFrameIndex == 0)
	{
		//Draw background and increase loop count
		FrameComposeRT.Clear(m_BackgroundColor);
	}

	// Produce the next frame
	FrameComposeRT.DrawBitmap(pGifRawFrame.get(), m_FramePosition);

	hr = FrameComposeRT.EndDraw();

	pGifRawFrame->Destroy();
	return hr;
}

HRESULT CIntelliViewView::OverlayNextGifFrame()
{
	// Validate our parameters
	ASSERT(m_FrameComposeRT.IsValid());

	// Get Frame information
	HRESULT hr{ GetRawFrame(GetRenderTarget(), m_nNextFrameIndex, m_pGifRawFrame) };
	if (FAILED(hr))
		return hr;

	// For disposal 3 method, we would want to save a copy of the current composed frame
	if (m_FrameDisposal == DISPOSAL_METHOD::NONE)
	{
		hr = SaveComposedGifFrame();
		if (FAILED(hr))
			return hr;
	}

	// Start producing the next bitmap
	m_FrameComposeRT.BeginDraw();

	//If starting a new animation loop
	if (m_nNextFrameIndex == 0)
	{
		// Draw background and increase loop count
		m_FrameComposeRT.Clear(m_BackgroundColor);
		m_nLoopNumber++;
	}

	// Produce the next frame
	m_FrameComposeRT.DrawBitmap(m_pGifRawFrame.get(), m_FramePosition);

	hr = m_FrameComposeRT.EndDraw();
	if (FAILED(hr))
		return hr;

	// Increase the frame index by 1
	m_nNextFrameIndex = (++m_nNextFrameIndex) % m_nFrames;

	return hr;
}

HRESULT CIntelliViewView::SaveComposedGifFrame()
{
	// Validate our parameters
	ASSERT(m_FrameComposeRT.GetRenderTarget() != nullptr);

	CD2DBitmap frameToBeSaved{ &m_FrameComposeRT, UINT{42}, nullptr, CD2DSizeU{0, 0}, FALSE }; //Required because "default" style constructor is protected!
	if (!m_FrameComposeRT.GetBitmap(frameToBeSaved))
		return E_FAIL;

	// Create the temporary bitmap if it hasn't been created yet
	if (m_pGifSavedFrame == nullptr)
	{
		const CD2DSizeF bs{ frameToBeSaved.GetPixelSize() };
		D2D1_SIZE_U bitmapSize;
#pragma warning(suppress: 26467)
		bitmapSize.height = static_cast<UINT32>(bs.height);
#pragma warning(suppress: 26467)
		bitmapSize.width = static_cast<UINT32>(bs.width);
		D2D1_BITMAP_PROPERTIES bitmapProp;
		bitmapProp.pixelFormat = frameToBeSaved.GetPixelFormat();
		const CD2DSizeF dpi{ m_FrameComposeRT.GetDpi() };
		bitmapProp.dpiX = dpi.width;
		bitmapProp.dpiY = dpi.height;
		ATL::CComPtr<ID2D1Bitmap> tempFrame;
		const HRESULT hr{ m_FrameComposeRT.GetRenderTarget()->CreateBitmap(bitmapSize, bitmapProp, &tempFrame) };
		if (FAILED(hr))
			return hr;
		m_pGifSavedFrame = std::make_unique<CD2DBitmap>(nullptr, UINT{ 42 }, nullptr, CD2DSizeU{ 0, 0 }, FALSE); //Required because "default" style constructor is protected!
		m_pGifSavedFrame->Attach(tempFrame.Detach());
	}

	// Copy the whole bitmap
	return m_pGifSavedFrame->CopyFromBitmap(&frameToBeSaved);
}

#pragma warning(suppress: 26429)
HRESULT CIntelliViewView::LoadNormalFrame(_In_ int nFrame, _In_ ID2D1RenderTarget* pRenderTarget, _Inout_ std::unique_ptr<CD2DBitmap>& bitmap)
{
	// Validate our parameters
	if (m_pDecoder == nullptr)
		return E_FAIL;
	ASSERT(pRenderTarget != nullptr);

	// Retrieve the current frame
	ATL::CComPtr<IWICBitmapFrameDecode> pWicFrame;
	HRESULT hr{ m_pDecoder->GetFrame(nFrame, &pWicFrame) };
	if (FAILED(hr))
		return hr;
	auto pD2DState{ AfxGetD2DState() };
	if (pD2DState == nullptr)
		return E_FAIL;
	IWICImagingFactory* pFactory{ pD2DState->GetWICFactory() };
	if (pFactory == nullptr)
		return E_FAIL;
	ATL::CComPtr<IWICFormatConverter> pConverter;
	hr = pFactory->CreateFormatConverter(&pConverter);
	if (FAILED(hr))
		return hr;
	// Format convert to 32bppPBGRA which D2D expects
	hr = pConverter->Initialize(pWicFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);
	if (FAILED(hr))
		return hr;

	// Get any per frame orientation data
	ATL::CComPtr<IWICMetadataQueryReader> pMetadataQueryReader;
	if (SUCCEEDED(pWicFrame->GetMetadataQueryReader(&pMetadataQueryReader)))
		GetOrientationData(pMetadataQueryReader);

	// Use the IWICBitmapFlipRotator interface if required
	ATL::CComPtr<IWICBitmapFlipRotator> pFlipRotator;
	if (theApp.m_bAutoRotate && (m_nOrientationFlag != 0) && (m_nOrientationFlag != 1))
	{
		hr = pFactory->CreateBitmapFlipRotator(&pFlipRotator);
		if (FAILED(hr))
			return hr;
		switch (m_nOrientationFlag)
		{
			case 2:
			{
				hr = pFlipRotator->Initialize(pConverter, WICBitmapTransformFlipHorizontal);
				break;
			}
			case 3:
			{
				hr = pFlipRotator->Initialize(pConverter, WICBitmapTransformRotate180);
				break;
			}
			case 4:
			{
				hr = pFlipRotator->Initialize(pConverter, WICBitmapTransformFlipVertical);
				break;
			}
			case 5:
			{
				hr = pFlipRotator->Initialize(pConverter, static_cast<WICBitmapTransformOptions>(WICBitmapTransformRotate270 | WICBitmapTransformFlipHorizontal));
				break;
			}
			case 6:
			{
				hr = pFlipRotator->Initialize(pConverter, WICBitmapTransformRotate90);
				break;
			}
			case 7:
			{
				hr = pFlipRotator->Initialize(pConverter, static_cast<WICBitmapTransformOptions>(WICBitmapTransformRotate90 | WICBitmapTransformFlipHorizontal));
				break;
			}
			case 8:
			{
				hr = pFlipRotator->Initialize(pConverter, WICBitmapTransformRotate270);
				break;
			}
			default:
			{
				break;
			}
		}
		if (FAILED(hr))
			return hr;
	}

	// Create a D2D bitmap from IWICBitmapSource
	ID2D1Bitmap* pTempRawFrame{ nullptr };
	if (pFlipRotator != nullptr)
		hr = pRenderTarget->CreateBitmapFromWicBitmap(pFlipRotator, nullptr, &pTempRawFrame);
	else
		hr = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, nullptr, &pTempRawFrame);
	if (FAILED(hr))
		return hr;
	bitmap = std::make_unique<CD2DBitmap>(nullptr, UINT{ 42 }, nullptr, CD2DSizeU{ 0, 0 }, FALSE); //Required because "default" style constructor is protected!
	bitmap->Attach(pTempRawFrame);

	return S_OK;
}

HRESULT CIntelliViewView::GetImageDetails(_In_z_ LPCTSTR pszFilename)
{
	// Reset the states
	m_nNextFrameIndex = 0;
	m_FrameDisposal = DISPOSAL_METHOD::NONE; //No previous frame, use disposal none
	m_nLoopNumber = 0;
	m_bHasLoop = false;
	m_FrameComposeRT.Destroy();
	m_pGifRawFrame.reset();
	m_pGifSavedFrame.reset();
	m_ImageType = IMAGE_TYPE::UNDEFINED;
	m_nActivePage = -1;
	m_nOrientationFlag = 0;

	// Create a decoder for the file
	m_pDecoder.Release();
	auto pD2DState{ AfxGetD2DState() };
	if (pD2DState == nullptr)
		return E_FAIL;
	IWICImagingFactory* pFactory{ pD2DState->GetWICFactory() };
	if (pFactory == nullptr)
		return E_FAIL;
	HRESULT hr{ pFactory->CreateDecoderFromFilename(CStringW(pszFilename), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &m_pDecoder) };
	if (FAILED(hr))
		return hr;

	// Create the format convertor
	m_pFormatConverter.Release();
	hr = pFactory->CreateFormatConverter(&m_pFormatConverter);
	if (FAILED(hr))
		return hr;

	// Get the meta data from the file
	hr = GetGlobalMetadata();
	if (FAILED(hr))
	{
		if (m_nFrames > 1)
		{
			m_ImageType = IMAGE_TYPE::MULTIPAGE_IMAGE;
			m_nActivePage = 0;
		}
		else
			m_ImageType = IMAGE_TYPE::SINGLE_IMAGE;
		return hr;
	}

	// Auto start the animation if we have multiple frames
	if (m_nFrames > 1)
	{
		m_ImageType = IMAGE_TYPE::ANIMATED_GIF;
		hr = ComposeNextGifFrame();
		InvalidateRect(nullptr, FALSE);
	}
	else
		m_ImageType = IMAGE_TYPE::SINGLE_IMAGE;

	return hr;
}

HRESULT CIntelliViewView::ComposeNextGifFrame()
{
	// Create the frame compose render target if required
	if (!m_FrameComposeRT.IsValid())
	{
		const HRESULT hr{ CreateGifFrameComposeRenderTarget(GetRenderTarget(), m_FrameComposeRT) };
		if (FAILED(hr))
			return hr;
	}

	// First, kill the timer since the delay is no longer valid
	StopAnimation();

	// Compose one frame
	HRESULT hr{ DisposeCurrentGifFrame() };
	if (SUCCEEDED(hr))
		hr = OverlayNextGifFrame();

	// If we have more frames to play, set the timer according to the delay.
	// Set the timer regardless of whether we succeeded in composing a frame
	// to try our best to continue displaying the animation
	if (!EndOfAnimation() && (m_nFrames > 1))
	{
		//Set the timer according to the delay
		ASSERT(m_nFrameDelay != 0);
		m_nAnimationTimerID = SetTimer(2, m_nFrameDelay, nullptr);
	}

	return hr;
}

#pragma warning(suppress: 26429)
HRESULT CIntelliViewView::CreateGifFrameComposeRenderTarget(_In_ CRenderTarget* pRenderTarget, _Inout_ CBitmapRenderTarget& FrameComposeRT)
{
	// Validate our parameters
	ASSERT(pRenderTarget != nullptr);

	// Create a bitmap render target used to compose frames. Bitmap render targets cannot be resized, so we always recreate it.
	FrameComposeRT.Destroy();
	return pRenderTarget->CreateCompatibleRenderTarget(FrameComposeRT, CD2DSizeF{ static_cast<FLOAT>(m_cxGifImage), static_cast<FLOAT>(m_cyGifImage) }) ? S_OK : E_FAIL;
}

bool CIntelliViewView::IsLastFrame() noexcept
{
	return (m_nNextFrameIndex == 0);
}

bool CIntelliViewView::EndOfAnimation() noexcept
{
	return m_bHasLoop && IsLastFrame() && (m_nLoopNumber == (m_nTotalLoopCount + 1));
}

void CIntelliViewView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CIntelliViewView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CIntelliViewView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CIntelliViewView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CIntelliViewView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CIntelliViewView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

// CIntelliViewView diagnostics

#ifdef _DEBUG
void CIntelliViewView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CIntelliViewView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CIntelliViewDoc* CIntelliViewView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIntelliViewDoc)));
	return (CIntelliViewDoc*)m_pDocument;
}
#endif //_DEBUG

// CIntelliViewView message handlers

LRESULT CIntelliViewView::OnRecreatedResources(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	m_pImage.reset();
	m_pBackgroundBrushNonFullscreen.reset();
	m_pBackgroundBrushFullscreen.reset();
	m_FrameComposeRT.Destroy();
	m_pGifRawFrame.reset();
	m_pGifSavedFrame.reset();
	m_nNextFrameIndex = 0;
	m_FrameDisposal = DISPOSAL_METHOD::NONE; //No previous frames. Use disposal none.
	m_nLoopNumber = 0;

	// Re-start the animation if required
	if (m_ImageType == IMAGE_TYPE::ANIMATED_GIF)
	{
		// Load the first frame
		ComposeNextGifFrame();
		InvalidateRect(nullptr, FALSE);
	}
	return 0L;
}

#pragma warning(suppress: 26434 26440)
CIntelliViewDoc* CIntelliViewView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIntelliViewDoc)));
#pragma warning(suppress: 26466)
	return static_cast<CIntelliViewDoc*>(m_pDocument);
}

LRESULT CIntelliViewView::OnDraw2D(WPARAM /*wParam*/, LPARAM lParam)
{
	const CIntelliViewDoc* pDoc{ GetDocument() };
#pragma warning(suppress: 26496)
	AFXASSUME(pDoc != nullptr);

	// Pull out the render target
#pragma warning(suppress: 26490 26429)
	auto pRenderTarget{ reinterpret_cast<CHwndRenderTarget*>(lParam) };
#pragma warning(suppress: 26486)
	ASSERT(pRenderTarget != nullptr);

	//We want to work in actual pixel values
#pragma warning(suppress: 26486)
	pRenderTarget->SetDpi(CD2DSizeF(96, 96));

	// Recreate the bitmap if required
	const CString& sPathName{ pDoc->GetPathName() };
	if ((m_pImage == nullptr) && sPathName.GetLength())
	{
		if (m_nActivePage != -1)
		{
#pragma warning(suppress: 26486)
			if (FAILED(LoadNormalFrame(m_nActivePage, pRenderTarget->GetRenderTarget(), m_pImage)))
				m_pImage.reset();
		}
		else
		{
			CString sExt;
			_tsplitpath_s(sPathName, nullptr, 0, nullptr, 0, nullptr, 0, sExt.GetBuffer(_MAX_EXT), _MAX_EXT);
			sExt.ReleaseBuffer();
			if (sExt.CompareNoCase(_T(".QOI")) == 0)
			{
				if (FAILED(LoadQOI(sPathName, pRenderTarget->GetRenderTarget(), m_pImage)))
					m_pImage.reset();
			}
#pragma warning(suppress: 26486)
			else if (FAILED(LoadNormalFrame(0, pRenderTarget->GetRenderTarget(), m_pImage)))
				m_pImage.reset();
		}
	}

	const bool bFullscreen{ GetMainFrame()->FullScreen() };
	CRect r;
	GetClientRect(&r);

	// Draw the background
	if (bFullscreen)
	{
		// Create the fullscreen background brush if required
		if (m_pBackgroundBrushFullscreen == nullptr)
		{
#pragma warning(suppress: 26486)
			m_pBackgroundBrushFullscreen = std::make_unique<CD2DSolidColorBrush>(nullptr, D2D1::ColorF{ GetRValue(theApp.m_FullScreenBackgroundColor) / 256.f, GetGValue(theApp.m_FullScreenBackgroundColor) / 256.f, GetBValue(theApp.m_FullScreenBackgroundColor) / 256.f });
#pragma warning(suppress: 26486)
			if (FAILED(m_pBackgroundBrushFullscreen->Create(pRenderTarget)))
			{
				m_pBackgroundBrushFullscreen.reset();
				return FALSE;
			}
		}
		const CD2DRectF rectBackground{ r };
#pragma warning(suppress: 26486)
		pRenderTarget->FillRectangle(rectBackground, m_pBackgroundBrushFullscreen.get());
	}
	else
	{
		// Create the non fullscreen background brush if required
		if (m_pBackgroundBrushNonFullscreen == nullptr)
		{
			D2D1_BITMAP_BRUSH_PROPERTIES props{ D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP) };
			m_pBackgroundBrushNonFullscreen = std::make_unique<CD2DBitmapBrush>(nullptr, IDB_CROSSHATCH, _T("PNG"), CD2DSizeU{ 0, 0 }, &props, nullptr, FALSE);
#pragma warning(suppress: 26486)
			if (FAILED(m_pBackgroundBrushNonFullscreen->Create(pRenderTarget)))
			{
				m_pBackgroundBrushNonFullscreen.reset();
				return FALSE;
			}
		}

		const CD2DRectF rectBackground{ r };
#pragma warning(suppress: 26486)
		pRenderTarget->FillRectangle(rectBackground, m_pBackgroundBrushNonFullscreen.get());
	}

	// Draw the image if the document has one
	if (m_pImage != nullptr)
	{
		const CPoint ptScroll{ GetScrollPosition() };

		// Determine where the image should be drawn
		const CSize drawSize{ DrawSize(r) };
		CRect rDraw{ -ptScroll.x, -ptScroll.y, drawSize.cx - ptScroll.x, drawSize.cy - ptScroll.y };
		if (bFullscreen)
		{
			if (drawSize.cx < r.Width())
				rDraw.left = (r.Width() - drawSize.cx) / 2;
			if (drawSize.cy < r.Height())
				rDraw.top = (r.Height() - drawSize.cy) / 2;
			rDraw.right = rDraw.left + drawSize.cx;
			rDraw.bottom = rDraw.top + drawSize.cy;
		}

		const CD2DRectF rectBitmap{ rDraw };
		if (m_FrameComposeRT.IsValid())
		{
			// Get the bitmap to draw on the hwnd render target
#pragma warning(suppress: 26486)
			CD2DBitmap frameToRender{ pRenderTarget, UINT{42}, nullptr, CD2DSizeU(0, 0), FALSE }; //Required because "default" style constructor is protected!
			if (m_FrameComposeRT.GetBitmap(frameToRender))
			{
#pragma warning(suppress: 26486)
				pRenderTarget->DrawBitmap(&frameToRender, rectBitmap);
			}
		}
		else
		{
#pragma warning(suppress: 26486)
			pRenderTarget->DrawBitmap(m_pImage.get(), rectBitmap);
		}

		// Draw the filename if required
		if (bFullscreen && theApp.m_bShowFilenameDuringFullScreen)
		{
			//Get the font size to use
			NONCLIENTMETRICS ncm;
			ncm.cbSize = sizeof(ncm);
			if (!SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0))
				return FALSE;
			CDC* pDC{ GetDC() };
			if (pDC == nullptr)
				return FALSE;
			const int nPixelsY{ pDC->GetDeviceCaps(LOGPIXELSY) };
#pragma warning(suppress: 26486)
			ReleaseDC(pDC);
			auto fFontSize{ static_cast<FLOAT>(static_cast<double>(ncm.lfMessageFont.lfHeight) * nPixelsY / 96.0) };
			if (fFontSize < 0)
				fFontSize = -fFontSize;

			// Create the brush we need
#pragma warning(suppress: 26486)
			CD2DSolidColorBrush brushText{ pRenderTarget, D2D1::ColorF(GetRValue(theApp.m_FullScreenTextColor), GetGValue(theApp.m_FullScreenTextColor), GetBValue(theApp.m_FullScreenTextColor)) };

			// Form the text to display
			CString sText;
			if (theApp.m_nCurrentImageIndex != -1)
			{
				CString sCurrent;
				sCurrent.Format(_T("%d"), theApp.m_nCurrentImageIndex + 1);
				CString sTotal;
#pragma warning(suppress: 26472)
				sTotal.Format(_T("%d"), static_cast<int>(theApp.m_ImagesInCurrentFolder.size()));
				CString sPosition;
				// TODO: AfxFormatString2(sPosition, IDS_INDICATOR_IMAGE, sCurrent, sTotal);
				// TODO: AfxFormatString2(sText, IDS_FULLSCREEN_CAPTION, sPathName, sPosition);
			}
			else
				sText = sPathName;

			//Setup the text alignment and get the height of the text
#pragma warning(suppress: 26485 26486 26489)
			CD2DTextFormat textFormat{ pRenderTarget, ncm.lfMessageFont.lfFaceName, fFontSize };
#pragma warning(suppress: 26429)
			IDWriteTextFormat* pTextFormat{ textFormat.Get() };
			ASSERT(pTextFormat != nullptr);
			pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
#pragma warning(suppress: 26486)
			CD2DTextLayout textLayout{ pRenderTarget, sText, textFormat, pRenderTarget->GetSize() };
			DWRITE_TEXT_METRICS dWriteTextMetrics;
			if (FAILED(textLayout.Get()->GetMetrics(&dWriteTextMetrics)))
				return FALSE;

			//Draw the text
			const CD2DRectF rectText{ 5, 5, dWriteTextMetrics.width + 9, dWriteTextMetrics.height + 9 };
#pragma warning(suppress: 26486)
			pRenderTarget->DrawText(sText, rectText, &brushText, &textFormat);
		}
	}

	return TRUE;
}

void CIntelliViewView::OnTimer(UINT_PTR nIDEvent)
{
	if ((m_nSlideshowTimerID == nIDEvent) && !theApp.m_bMessageBoxUp)
	{
		// Go to the next message
		AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_NEXT_PAGE);
	}
	else if (m_nAnimationTimerID == nIDEvent)
	{
		if (m_ImageType == IMAGE_TYPE::ANIMATED_GIF)
		{
			// Timer expired, display the next frame and set a new timer if needed
			ComposeNextGifFrame();
			InvalidateRect(nullptr, FALSE);
		}
		else if (m_ImageType == IMAGE_TYPE::MULTIPAGE_IMAGE)
		{
#pragma warning(suppress: 26472)
			if (m_nActivePage < static_cast<int>(m_nFrames - 1))
			{
				++m_nActivePage;
				m_pImage.reset();
				InvalidateRect(nullptr, FALSE);
			}
			else
				StopAnimation();
		}
	}
	else
	{
		// Let the base class do its thing
		__super::OnTimer(nIDEvent);
	}
}

void CIntelliViewView::OnZoomIn()
{
	m_nCurrentZoomLevelX = (m_nCurrentZoomLevelX / 20 * 20) + 20;
	m_bSizeToFit = false;
	theApp.m_bSizeToFit = m_bSizeToFit;
	CRect r;
	GetClientRect(&r);
	SetScrollSizes(MM_TEXT, DrawSize(r));
	Invalidate(TRUE);
}

void CIntelliViewView::OnUpdateZoomIn(CCmdUI* pCmdUI)
{
	if (m_pImage != nullptr)
#pragma warning(suppress: 26486)
		pCmdUI->Enable(m_bSizeToFit || (m_nCurrentZoomLevelX < 300));
	else
#pragma warning(suppress: 26486)
		pCmdUI->Enable(FALSE);
}

void CIntelliViewView::OnZoomOut()
{
	m_nCurrentZoomLevelX = (m_nCurrentZoomLevelX / 20 * 20) - 20;
	m_bSizeToFit = false;
	theApp.m_bSizeToFit = m_bSizeToFit;
	CRect r;
	GetClientRect(&r);
	SetScrollSizes(MM_TEXT, DrawSize(r));
	Invalidate(TRUE);
}

void CIntelliViewView::OnUpdateZoomOut(CCmdUI* pCmdUI)
{
	if (m_pImage != nullptr)
#pragma warning(suppress: 26486)
		pCmdUI->Enable(m_bSizeToFit || (m_nCurrentZoomLevelX > 20));
	else
#pragma warning(suppress: 26486)
		pCmdUI->Enable(FALSE);
}

void CIntelliViewView::OnActualSize()
{
	m_nCurrentZoomLevelX = 100;
	m_bSizeToFit = false;
	theApp.m_bSizeToFit = m_bSizeToFit;
	CRect r;
	GetClientRect(&r);
	SetScrollSizes(MM_TEXT, DrawSize(r));
	Invalidate(TRUE);
}

void CIntelliViewView::OnUpdateActualSize(CCmdUI* pCmdUI)
{
	if (m_pImage != nullptr)
#pragma warning(suppress: 26486)
		pCmdUI->Enable(m_bSizeToFit || (m_nCurrentZoomLevelX != 100));
	else
#pragma warning(suppress: 26486)
		pCmdUI->Enable(FALSE);
}

void CIntelliViewView::OnBestFit()
{
	m_bSizeToFit = !m_bSizeToFit;
	theApp.m_bSizeToFit = m_bSizeToFit;
	CRect r;
	GetClientRect(&r);
	SetScrollSizes(MM_TEXT, DrawSize(r));
	Invalidate(TRUE);
}

void CIntelliViewView::OnUpdateBestFit(CCmdUI* pCmdUI)
{
	if (m_pImage != nullptr)
#pragma warning(suppress: 26486)
		pCmdUI->SetCheck(m_bSizeToFit);
	else
#pragma warning(suppress: 26486)
		pCmdUI->Enable(FALSE);
}
