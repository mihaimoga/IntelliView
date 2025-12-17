/* Copyright (c) 2024-2026 Stefan-Mihai MOGA
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

// IntelliViewView.h : interface of the CIntelliViewView class
//

#pragma once

class CIntelliViewDoc;

class CIntelliViewView : public CScrollView
{
public:
	// Enums
	enum class IMAGE_TYPE
	{
		UNDEFINED,
		SINGLE_IMAGE,
		ANIMATED_GIF,
		MULTIPAGE_IMAGE
	};
	enum class DISPOSAL_METHOD
	{
		UNDEFINED = 0,
		NONE = 1,
		BACKGROUND = 2,
		PREVIOUS = 3
	};

protected: // create from serialization only
	CIntelliViewView() noexcept;
	CIntelliViewDoc* GetDocument();
	DECLARE_DYNCREATE(CIntelliViewView)

// Attributes
public:
	IMAGE_TYPE m_ImageType;
	int m_nActivePage;
	UINT m_nFrames;
	UINT m_nCurrentZoomLevelX;
	bool m_bSizeToFit;
	UINT_PTR m_nSlideshowTimerID;
	UINT_PTR m_nAnimationTimerID;
	std::unique_ptr<CD2DBitmap> m_pImage;
	std::unique_ptr<CD2DBitmapBrush> m_pBackgroundBrushNonFullscreen;
	std::unique_ptr<CD2DSolidColorBrush> m_pBackgroundBrushFullscreen;
	CBitmapRenderTarget m_FrameComposeRT;
	std::unique_ptr<CD2DBitmap> m_pGifRawFrame;
	std::unique_ptr<CD2DBitmap> m_pGifSavedFrame; //The temporary bitmap used for disposal 3 method
	UINT m_nNextFrameIndex;
	UINT m_nTotalLoopCount; // The number of loops for which the animation will be played
	UINT m_nLoopNumber; // The current animation loop number (e.g. 1 when the animation is first played)
	bool m_bHasLoop; // Whether the gif has a loop
	DISPOSAL_METHOD m_FrameDisposal;
	UINT m_nFrameDelay;
	UINT m_cxGifImage;
	UINT m_cyGifImage;
	D2D1_RECT_F m_FramePosition;
	D2D1_COLOR_F m_BackgroundColor;
	ATL::CComPtr<IWICBitmapDecoder> m_pDecoder;
	ATL::CComPtr<IWICFormatConverter> m_pFormatConverter;
	USHORT m_nOrientationFlag;

// Operations
public:
	CIntelliViewDoc* GetDocument() const;

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CIntelliViewView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Methods
public:
	CSize DrawSize(_In_ const CRect& rClient);
	void StopAnimation();
	bool CreateRenderTargetIfNecessary();
	HRESULT GetGlobalMetadata();
	HRESULT GetRawFrame(_In_ CRenderTarget* pRenderTarget, _In_ UINT uFrameIndex, _Inout_ std::unique_ptr<CD2DBitmap>& pGifRawFrame);
	HRESULT GetGifBackgroundColor(_In_ IWICMetadataQueryReader* pMetadataQueryReader);
	HRESULT GetOrientationData(_In_ IWICMetadataQueryReader* pMetadataQueryReader);
	HRESULT RestoreSavedGifFrame();
	HRESULT ClearCurrentGifFrameArea();
	HRESULT DisposeCurrentGifFrame();
	HRESULT OverlayNextGifFrame();
	HRESULT OverlayNextGifFrameForCopy(_In_ CRenderTarget* pRenderTarget, _Inout_ CBitmapRenderTarget& FrameComposeRT);
	HRESULT SaveComposedGifFrame();
	HRESULT GetImageDetails(_In_z_ LPCTSTR pszFilename);
	HRESULT ComposeNextGifFrame();
	HRESULT CreateGifFrameComposeRenderTarget(_In_ CRenderTarget* pRenderTarget, _Inout_ CBitmapRenderTarget& FrameComposeRT);
	bool IsLastFrame() noexcept;
	bool EndOfAnimation() noexcept;
	HRESULT LoadNormalFrame(_In_ int nFrame, _In_ ID2D1RenderTarget* pRenderTarget, _Inout_ std::unique_ptr<CD2DBitmap>& bitmap);
	HRESULT LoadQOI(_In_z_ LPCTSTR lpszPathName, _In_ ID2D1RenderTarget* pRenderTarget, _Inout_ std::unique_ptr<CD2DBitmap>& bitmap);

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg LRESULT OnRecreatedResources(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDraw2D(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnZoomIn();
	afx_msg void OnUpdateZoomIn(CCmdUI* pCmdUI);
	afx_msg void OnZoomOut();
	afx_msg void OnUpdateZoomOut(CCmdUI* pCmdUI);
	afx_msg void OnActualSize();
	afx_msg void OnUpdateActualSize(CCmdUI* pCmdUI);
	afx_msg void OnBestFit();
	afx_msg void OnUpdateBestFit(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in IntelliViewView.cpp
inline CIntelliViewDoc* CIntelliViewView::GetDocument() const
   { return reinterpret_cast<CIntelliViewDoc*>(m_pDocument); }
#endif
