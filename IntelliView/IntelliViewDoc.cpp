/* Copyright (C) 2024-2026 Stefan-Mihai MOGA
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

// IntelliViewDoc.cpp : implementation of the CIntelliViewDoc class
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
#include "RenameDlg.h"
#include "GotoPageDlg.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @class CIntelliViewDoc
 * @brief Document class for IntelliView image viewer application
 * @details Manages document data, file operations, and multi-page/animated image navigation
 */

// CIntelliViewDoc

IMPLEMENT_DYNCREATE(CIntelliViewDoc, CDocument)

BEGIN_MESSAGE_MAP(CIntelliViewDoc, CDocument)
	ON_COMMAND(ID_FIRST_PAGE, &CIntelliViewDoc::OnFirstPage)
	ON_UPDATE_COMMAND_UI(ID_FIRST_PAGE, &CIntelliViewDoc::OnUpdateFirstPage)
	ON_COMMAND(ID_PREV_PAGE, &CIntelliViewDoc::OnPrevPage)
	ON_UPDATE_COMMAND_UI(ID_PREV_PAGE, &CIntelliViewDoc::OnUpdatePrevPage)
	ON_COMMAND(ID_NEXT_PAGE, &CIntelliViewDoc::OnNextPage)
	ON_UPDATE_COMMAND_UI(ID_NEXT_PAGE, &CIntelliViewDoc::OnUpdateNextPage)
	ON_COMMAND(ID_LAST_PAGE, &CIntelliViewDoc::OnLastPage)
	ON_UPDATE_COMMAND_UI(ID_LAST_PAGE, &CIntelliViewDoc::OnUpdateLastPage)
	ON_COMMAND(ID_OPENPAGE, &CIntelliViewDoc::OnOpenPage)
	ON_UPDATE_COMMAND_UI(ID_OPENPAGE, &CIntelliViewDoc::OnUpdateOpenPage)
	ON_COMMAND(ID_ANIMATION, &CIntelliViewDoc::OnAnimation)
	ON_UPDATE_COMMAND_UI(ID_ANIMATION, &CIntelliViewDoc::OnUpdateAnimation)
	ON_COMMAND(ID_FILE_RENAME, &CIntelliViewDoc::OnFileRename)
	ON_UPDATE_COMMAND_UI(ID_FILE_RENAME, &CIntelliViewDoc::OnUpdateFileRename)
	ON_COMMAND(ID_FILE_MOVE, &CIntelliViewDoc::OnFileMove)
	ON_UPDATE_COMMAND_UI(ID_FILE_MOVE, &CIntelliViewDoc::OnUpdateFileMove)
	ON_COMMAND(ID_FILE_COPY, &CIntelliViewDoc::OnFileCopy)
	ON_UPDATE_COMMAND_UI(ID_FILE_COPY, &CIntelliViewDoc::OnUpdateFileCopy)
	ON_COMMAND(ID_FILE_DELETE, &CIntelliViewDoc::OnFileDelete)
	ON_UPDATE_COMMAND_UI(ID_FILE_DELETE, &CIntelliViewDoc::OnUpdateFileDelete)
	ON_COMMAND(ID_PROPERTIES, &CIntelliViewDoc::OnProperties)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES, &CIntelliViewDoc::OnUpdateProperties)
END_MESSAGE_MAP()

// CIntelliViewDoc construction/destruction

/**
 * @brief Constructs a new CIntelliViewDoc object
 */
CIntelliViewDoc::CIntelliViewDoc() noexcept
{
}

/**
 * @brief Destroys the CIntelliViewDoc object
 */
CIntelliViewDoc::~CIntelliViewDoc()
{
}

/**
 * @brief Creates a new document
 * @return TRUE on success, FALSE on failure
 * @details Called by the framework to create a new document instance
 */
BOOL CIntelliViewDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

// CIntelliViewDoc serialization

/**
 * @brief Serializes the document data
 * @param ar Reference to CArchive for reading/writing data
 * @details Currently not implemented for this image viewer application
 */
void CIntelliViewDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
/**
 * @brief Draws a thumbnail representation of the document
 * @param dc Reference to CDC (device context) to draw on
 * @param lprcBounds Pointer to RECT structure defining thumbnail bounds
 * @details Used by Windows for thumbnail preview generation
 */
void CIntelliViewDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
/**
 * @brief Initializes search content for the document
 * @details Sets up searchable content for Windows Search integration
 */
void CIntelliViewDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

/**
 * @brief Sets the search content for the document
 * @param value Reference to CString containing the search content
 * @details Adds or removes search content chunks for Windows Search indexing
 */
void CIntelliViewDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CIntelliViewDoc diagnostics

#ifdef _DEBUG
/**
 * @brief Validates the object's state (debug builds only)
 * @details Performs diagnostic assertions to verify object integrity
 */
void CIntelliViewDoc::AssertValid() const
{
	CDocument::AssertValid();
}

/**
 * @brief Dumps the object's state to a diagnostic context (debug builds only)
 * @param dc Reference to CDumpContext for diagnostic output
 */
void CIntelliViewDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

// CIntelliViewDoc commands

/**
 * @brief Retrieves the associated view object
 * @return Pointer to CIntelliViewView, or nullptr if no view exists
 * @details Finds the first CIntelliViewView attached to this document
 */
CIntelliViewView* CIntelliViewDoc::GetView() const
{
	// find the first view - if there are no views
	// we must return nullptr
	POSITION pos{ GetFirstViewPosition() };
	if (pos == nullptr)
		return nullptr;

	// find the first view that is a CScintillaView
	while (pos != nullptr)
	{
#pragma warning(suppress: 26429)
		CView* pView{ GetNextView(pos) };
		ASSERT(pView != nullptr);
		if (pView->IsKindOf(RUNTIME_CLASS(CIntelliViewView)))
#pragma warning(suppress: 26466)
			return static_cast<CIntelliViewView*>(pView);
	}

	// can't find one, then return nullptr
	return nullptr;
}

/**
 * @brief Deletes the document contents
 * @details Resets the image pointer in the associated view and clears document data
 */
void CIntelliViewDoc::DeleteContents()
{
	CIntelliViewView* pView{ GetView() };
	if (pView != nullptr)
#pragma warning(suppress: 26489)
		pView->m_pImage.reset();

	// Let the parent class do its thing
	__super::DeleteContents();
}

/**
 * @brief Opens and loads an image document from disk
 * @param lpszPathName Pointer to string containing the full path to the image file
 * @return TRUE on success, FALSE on failure
 * @details Loads image files including QOI format and other standard formats, detects multi-page images
 */
BOOL CIntelliViewDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	DeleteContents();

	// Validate our parameters
#pragma warning(suppress: 26429)
	CIntelliViewView* pView{ GetView() };
	ASSERT(pView != nullptr);
#pragma warning(suppress: 26489)
	ASSERT(!pView->m_pImage); //NOLINT(clang-analyzer-core.CallAndMessage)

	// Load up the new file
	pView->StopAnimation(); //NOLINT(clang-analyzer-core.CallAndMessage)
	bool bLoadError{ false };

	if (!pView->CreateRenderTargetIfNecessary())
		return FALSE;
#pragma warning(suppress: 26429)
	CRenderTarget* pRenderTarget{ pView->GetRenderTarget() };
	ASSERT(pRenderTarget != nullptr);
#pragma warning(suppress: 26489)
	pView->m_pImage.reset();
	pView->GetImageDetails(lpszPathName);
	CString sExt;
	_tsplitpath_s(lpszPathName, nullptr, 0, nullptr, 0, nullptr, 0, sExt.GetBuffer(_MAX_EXT), _MAX_EXT);
	sExt.ReleaseBuffer();
	if (sExt.CompareNoCase(_T(".QOI")) == 0)
	{
		if (FAILED(pView->LoadQOI(lpszPathName, pRenderTarget->GetRenderTarget(), pView->m_pImage)))
		{
			pView->m_pImage.reset();
			bLoadError = true;
		}
		else
			pView->m_ImageType = CIntelliViewView::IMAGE_TYPE::SINGLE_IMAGE;
	}
	else
	{
		if (FAILED(pView->LoadNormalFrame(0, pRenderTarget->GetRenderTarget(), pView->m_pImage)))
		{
			pView->m_pImage.reset();
			bLoadError = true;
		}
	}

	if (bLoadError)
	{
		// pView->KillSlideshow();
		CString sMsg;
		AfxFormatString1(sMsg, IDS_FAILED_TO_OPEN_DOC, lpszPathName);
		AfxMessageBox(sMsg);
		return FALSE;
	}

	return TRUE;
}

/**
 * @brief Navigates to the first page of a multi-page image
 * @details Resets active page index to 0 and refreshes the view
 */
void CIntelliViewDoc::OnFirstPage()
{
#pragma warning(suppress: 26429)
	CIntelliViewView* pView{ GetView() };
	ASSERT(pView != nullptr);
	pView->m_nActivePage = 0; //NOLINT(clang-analyzer-core.NullDereference)
#pragma warning(suppress: 26489)
	pView->m_pImage.reset();
	pView->InvalidateRect(nullptr, FALSE);
}

/**
 * @brief Updates the UI state of the first page command
 * @param pCmdUI Pointer to CCmdUI object representing the user interface item
 * @details Enables/disables and checks the first page button based on document state
 */
void CIntelliViewDoc::OnUpdateFirstPage(CCmdUI* pCmdUI)
{
	const CIntelliViewView* pView{ GetView() };
	ASSERT(pView != nullptr);
	const bool bEnable{ pView->m_ImageType == CIntelliViewView::IMAGE_TYPE::MULTIPAGE_IMAGE }; //NOLINT(clang-analyzer-core.NullDereference)
	if (bEnable)
	{
#pragma warning(suppress: 26486)
		pCmdUI->Enable(TRUE);
#pragma warning(suppress: 26486)
		pCmdUI->SetCheck(pView->m_nActivePage == 0);
	}
	else
#pragma warning(suppress: 26486)
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Navigates to the previous page of a multi-page image
 * @details Decrements the active page index and refreshes the view
 */
void CIntelliViewDoc::OnPrevPage()
{
#pragma warning(suppress: 26429)
	CIntelliViewView* pView{ GetView() };
	ASSERT(pView != nullptr);
	--pView->m_nActivePage; //NOLINT(clang-analyzer-core.NullDereference)
#pragma warning(suppress: 26489)
	pView->m_pImage.reset();
	pView->InvalidateRect(nullptr, FALSE);
}

/**
 * @brief Updates the UI state of the previous page command
 * @param pCmdUI Pointer to CCmdUI object representing the user interface item
 * @details Enables the button only if on a multi-page image and not on the first page
 */
void CIntelliViewDoc::OnUpdatePrevPage(CCmdUI* pCmdUI)
{
	const CIntelliViewView* pView{ GetView() };
	ASSERT(pView != nullptr);
#pragma warning(suppress: 26486)
	pCmdUI->Enable((pView->m_ImageType == CIntelliViewView::IMAGE_TYPE::MULTIPAGE_IMAGE) && (pView->m_nActivePage > 0)); //NOLINT(clang-analyzer-core.NullDereference)
}

/**
 * @brief Navigates to the next page of a multi-page image
 * @details Increments the active page index and refreshes the view
 */
void CIntelliViewDoc::OnNextPage()
{
#pragma warning(suppress: 26429)
	CIntelliViewView* pView{ GetView() };
	ASSERT(pView != nullptr);
	++pView->m_nActivePage; //NOLINT(clang-analyzer-core.NullDereference)
#pragma warning(suppress: 26489)
	pView->m_pImage.reset();
	pView->InvalidateRect(nullptr, FALSE);
}

/**
 * @brief Updates the UI state of the next page command
 * @param pCmdUI Pointer to CCmdUI object representing the user interface item
 * @details Enables the button only if on a multi-page image and not on the last page
 */
void CIntelliViewDoc::OnUpdateNextPage(CCmdUI* pCmdUI)
{
	const CIntelliViewView* pView{ GetView() };
	ASSERT(pView != nullptr);
#pragma warning(suppress: 26472 26486)
	pCmdUI->Enable((pView->m_ImageType == CIntelliViewView::IMAGE_TYPE::MULTIPAGE_IMAGE) && (pView->m_nActivePage < static_cast<int>(pView->m_nFrames - 1))); //NOLINT(clang-analyzer-core.NullDereference)
}

/**
 * @brief Navigates to the last page of a multi-page image
 * @details Sets the active page to the last frame and refreshes the view
 */
void CIntelliViewDoc::OnLastPage()
{
#pragma warning(suppress: 26429)
	CIntelliViewView* pView{ GetView() };
	ASSERT(pView != nullptr);
	pView->m_nActivePage = pView->m_nFrames - 1; //NOLINT(clang-analyzer-core.NullDereference)
#pragma warning(suppress: 26489)
	pView->m_pImage.reset();
	pView->InvalidateRect(nullptr, FALSE);
}

/**
 * @brief Updates the UI state of the last page command
 * @param pCmdUI Pointer to CCmdUI object representing the user interface item
 * @details Enables/disables and checks the last page button based on document state
 */
void CIntelliViewDoc::OnUpdateLastPage(CCmdUI* pCmdUI)
{
	const CIntelliViewView* pView{ GetView() };
	ASSERT(pView != nullptr);
	const bool bEnable{ pView->m_ImageType == CIntelliViewView::IMAGE_TYPE::MULTIPAGE_IMAGE }; //NOLINT(clang-analyzer-core.NullDereference)
	if (bEnable)
	{
#pragma warning(suppress: 26486)
		pCmdUI->Enable(TRUE);
#pragma warning(suppress: 26472 26486)
		pCmdUI->SetCheck(pView->m_nActivePage == static_cast<int>(pView->m_nFrames - 1));
	}
	else
#pragma warning(suppress: 26486)
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Opens a specific page by number in a multi-page image
 * @details Displays a dialog for the user to enter a page number and navigates to it
 */
void CIntelliViewDoc::OnOpenPage()
{
#pragma warning(suppress: 26429)
	CIntelliViewView* pView{ GetView() };
	ASSERT(pView != nullptr);
	CGotoPageDlg dlg;
	dlg.m_nMaxPageNumber = pView->m_nFrames; //NOLINT(clang-analyzer-core.NullDereference)
	if (dlg.DoModal() != IDOK)
		return;
	pView->m_nActivePage = dlg.m_nPageNumber - 1;
#pragma warning(suppress: 26489)
	pView->m_pImage.reset();
	pView->InvalidateRect(nullptr, FALSE);
}

/**
 * @brief Updates the UI state of the open page command
 * @param pCmdUI Pointer to CCmdUI object representing the user interface item
 * @details Enables the command only for multi-page images
 */
void CIntelliViewDoc::OnUpdateOpenPage(CCmdUI* pCmdUI)
{
	const CIntelliViewView* pView{ GetView() };
	ASSERT(pView != nullptr);
#pragma warning(suppress: 26486)
	pCmdUI->Enable(pView->m_ImageType == CIntelliViewView::IMAGE_TYPE::MULTIPAGE_IMAGE); //NOLINT(clang-analyzer-core.NullDereference)
}

/**
 * @brief Toggles animation playback for multi-page or animated GIF images
 * @details Starts or stops automatic page advancement or GIF frame composition
 */
void CIntelliViewDoc::OnAnimation()
{
#pragma warning(suppress: 26429)
	CIntelliViewView* pView{ GetView() };
	ASSERT(pView != nullptr);
	if (pView->m_nAnimationTimerID == 0) //NOLINT(clang-analyzer-core.NullDereference)
	{
		if (pView->m_ImageType == CIntelliViewView::IMAGE_TYPE::MULTIPAGE_IMAGE)
		{
#pragma warning(suppress: 26472)
			if (pView->m_nActivePage == static_cast<int>(pView->m_nFrames - 1))
				pView->m_nActivePage = 0;
			pView->m_nAnimationTimerID = pView->SetTimer(2, theApp.m_nSlideshowInterval * 1000, nullptr);
		}
		else if (pView->m_ImageType == CIntelliViewView::IMAGE_TYPE::ANIMATED_GIF)
			pView->ComposeNextGifFrame();
	}
	else
		pView->StopAnimation();
}

/**
 * @brief Updates the UI state of the animation command
 * @param pCmdUI Pointer to CCmdUI object representing the user interface item
 * @details Enables/disables and checks the animation button based on image type and animation state
 */
void CIntelliViewDoc::OnUpdateAnimation(CCmdUI* pCmdUI)
{
	const CIntelliViewView* pView{ GetView() };
	ASSERT(pView != nullptr);
#pragma warning(suppress: 26486)
	if ((pView->m_ImageType == CIntelliViewView::IMAGE_TYPE::MULTIPAGE_IMAGE) || (pView->m_ImageType == CIntelliViewView::IMAGE_TYPE::ANIMATED_GIF)) //NOLINT(clang-analyzer-core.NullDereference)
	{
#pragma warning(suppress: 26486)
		pCmdUI->Enable(true);
#pragma warning(suppress: 26486)
		pCmdUI->SetCheck(pView->m_nAnimationTimerID != 0);
	}
	else
#pragma warning(suppress: 26486)
		pCmdUI->Enable(false);
}

/**
 * @brief Renames the current image file
 * @details Displays a rename dialog and uses Shell API to rename the file with undo support
 */
void CIntelliViewDoc::OnFileRename()
{
	// Get the name of the current file
	CString sOriginalPath{ GetPathName() };
	std::filesystem::path path(sOriginalPath.GetString());
	CString sPath{ path.filename().c_str() };

	// Bring up the rename dialog
	CRenameDlg dlg;
	dlg.m_sFilename = sPath;
	if (dlg.DoModal() == IDOK)
	{
		// Let the shell do the rename
		SHFILEOPSTRUCT shfo{};
		shfo.hwnd = AfxGetMainWnd()->GetSafeHwnd();
		shfo.wFunc = FO_RENAME;
		shfo.fFlags = FOF_ALLOWUNDO;
		CString sFrom{ sOriginalPath };
		const int nFromLength{ sFrom.GetLength() };
#pragma warning(suppress: 26472)
		std::vector<TCHAR> pszFrom{ static_cast<size_t>(nFromLength) + 2, std::allocator<TCHAR>{} };
		_tcscpy_s(pszFrom.data(), pszFrom.size(), sFrom);
#pragma warning(suppress: 26446 26472)
		pszFrom[static_cast<size_t>(nFromLength) + 1] = _T('\0');
		shfo.pFrom = pszFrom.data();
		path.replace_filename(dlg.m_sFilename.GetString());
		CString sTo{ path.c_str() };
		const int nToLength{ sTo.GetLength() };
#pragma warning(suppress: 26472)
		std::vector<TCHAR> pszTo{ static_cast<size_t>(nToLength) + 2, std::allocator<TCHAR>{} };
		_tcscpy_s(pszTo.data(), pszTo.size(), sTo);
#pragma warning(suppress: 26446 26472)
		pszTo[static_cast<size_t>(nToLength) + 1] = _T('\0');
		shfo.pTo = pszTo.data();

		// Let the shell perform the actual deletion
		const int nSuccess{ SHFileOperation(&shfo) };
		if (nSuccess == 0)
		{
			// Open the newly renamed file
			CDocTemplate* pTemplate{ GetDocTemplate() };
#pragma warning(suppress: 26496)
			AFXASSUME(pTemplate != nullptr);
			if (pTemplate->OpenDocumentFile(sTo, FALSE, TRUE) == nullptr)
				pTemplate->OpenDocumentFile(nullptr, FALSE, TRUE);
			// Since we have deleted the current file
			// move onto the next file in the directory
			GetView()->SendMessage(WM_COMMAND, ID_FILE_CLOSE, 0L);
		}
		/* else
		{
			// Reset this document to the current image before the failed rename above
			CDocTemplate* pTemplate{ GetDocTemplate() };
#pragma warning(suppress: 26496)
			AFXASSUME(pTemplate != nullptr);
			if (pTemplate->OpenDocumentFile(sPath, FALSE, TRUE) == nullptr)
				pTemplate->OpenDocumentFile(nullptr, FALSE, TRUE);
		} */
	}
}

/**
 * @brief Updates the UI state of the file rename command
 * @param pCmdUI Pointer to CCmdUI object representing the user interface item
 * @details Enables the command only when an image is loaded
 */
void CIntelliViewDoc::OnUpdateFileRename(CCmdUI* pCmdUI)
{
#pragma warning(suppress: 26486 26489)
	pCmdUI->Enable(GetView()->m_pImage != nullptr);
}

/**
 * @brief Moves the current image file to a different directory
 * @details Prompts user to select a destination folder and uses Shell API to move the file
 */
void CIntelliViewDoc::OnFileMove()
{
	// Get the name of the current file
	CString sPath{ GetPathName() };

	// Prompt the user for where they want to move the file to
	CString sTitle;
	if (!sTitle.LoadString(IDS_FOLDER_MOVE_TITLE))
		return;
	CString sDir;
	if (theApp.GetShellManager()->BrowseForFolder(sDir, AfxGetMainWnd(), nullptr, sTitle, BIF_RETURNONLYFSDIRS, nullptr))
	{
		// Ensure the directory has a "\" at its end
		const int nLen{ sDir.GetLength() };
		if (sDir[nLen - 1] != _T('\\'))
			sDir += _T("\\");

		// Let the shell do the move
		SHFILEOPSTRUCT shfo{};
		shfo.hwnd = AfxGetMainWnd()->GetSafeHwnd();
		shfo.wFunc = FO_MOVE;
		shfo.fFlags = FOF_ALLOWUNDO;
		CString sFrom{ sPath };
		const int nFromLength{ sFrom.GetLength() };
#pragma warning(suppress: 26472)
		std::vector<TCHAR> pszFrom{ static_cast<size_t>(nFromLength) + 2, std::allocator<TCHAR>{} };
		_tcscpy_s(pszFrom.data(), pszFrom.size(), sFrom);
#pragma warning(suppress: 26446 26472)
		pszFrom[static_cast<size_t>(nFromLength) + 1] = _T('\0');
		shfo.pFrom = pszFrom.data();
		std::filesystem::path path1{ sDir.GetString() };
		std::filesystem::path path2{ sPath.GetString() };
		path1.replace_filename(path2.filename());
		CString sTo{ path1.c_str() };
		const int nToLength{ sTo.GetLength() };
#pragma warning(suppress: 26472)
		std::vector<TCHAR> pszTo{ static_cast<size_t>(nToLength) + 2, std::allocator<TCHAR>{} };
		_tcscpy_s(pszTo.data(), pszTo.size(), sTo);
#pragma warning(suppress: 26446 26472)
		pszTo[static_cast<size_t>(nToLength) + 1] = _T('\0');
		shfo.pTo = pszTo.data();

		// Let the shell perform the actual deletion
		const int nSuccess{ SHFileOperation(&shfo) };
		if (nSuccess == 0)
		{
			// Open the newly renamed file
			CDocTemplate* pTemplate{ GetDocTemplate() };
#pragma warning(suppress: 26496)
			AFXASSUME(pTemplate != nullptr);
			if (pTemplate->OpenDocumentFile(sTo, FALSE, TRUE) == nullptr)
				pTemplate->OpenDocumentFile(nullptr, FALSE, TRUE);
			// Since we have deleted the current file
			// move onto the next file in the directory
			GetView()->SendMessage(WM_COMMAND, ID_FILE_CLOSE, 0L);
		}
		/* else
		{
			// Reset this document to the current image before the failed rename above
			CDocTemplate* pTemplate{ GetDocTemplate() };
#pragma warning(suppress: 26496)
			AFXASSUME(pTemplate != nullptr);
			if (pTemplate->OpenDocumentFile(sPath, FALSE, TRUE) == nullptr)
				pTemplate->OpenDocumentFile(nullptr, FALSE, TRUE);
		} */
	}
}

/**
 * @brief Updates the UI state of the file move command
 * @param pCmdUI Pointer to CCmdUI object representing the user interface item
 * @details Enables the command only when an image is loaded
 */
void CIntelliViewDoc::OnUpdateFileMove(CCmdUI* pCmdUI)
{
#pragma warning(suppress: 26486 26489)
	pCmdUI->Enable(GetView()->m_pImage != nullptr);
}

/**
 * @brief Copies the current image file to a different directory
 * @details Prompts user to select a destination folder and uses Shell API to copy the file
 */
void CIntelliViewDoc::OnFileCopy()
{
	// Get the name of the current file
	CString sPath{ GetPathName() };

	// Prompt the user for where they want to copy the file to
	CString sTitle;
	if (!sTitle.LoadString(IDS_FOLDER_COPY_TITLE))
		return;
	CString sDir;
	if (theApp.GetShellManager()->BrowseForFolder(sDir, AfxGetMainWnd(), nullptr, sTitle, BIF_RETURNONLYFSDIRS, nullptr))
	{
		// Ensure the directory has a "\" at its end
		const int nLen{ sDir.GetLength() };
		if (sDir[nLen - 1] != _T('\\'))
			sDir += _T("\\");

		// Let the shell do the copy
		SHFILEOPSTRUCT shfo{};
		shfo.hwnd = AfxGetMainWnd()->GetSafeHwnd();
		shfo.wFunc = FO_COPY;
		shfo.fFlags = FOF_ALLOWUNDO;
		CString sFrom{ sPath };
		const int nFromLength{ sFrom.GetLength() };
#pragma warning(suppress: 26472)
		std::vector<TCHAR> pszFrom{ static_cast<size_t>(nFromLength) + 2, std::allocator<TCHAR>{} };
		_tcscpy_s(pszFrom.data(), pszFrom.size(), sFrom);
#pragma warning(suppress: 26446 26472)
		pszFrom[static_cast<size_t>(nFromLength) + 1] = _T('\0');
		shfo.pFrom = pszFrom.data();
		std::filesystem::path path1{ sDir.GetString() };
		std::filesystem::path path2{ sPath.GetString() };
		path1.replace_filename(path2.filename());
		CString sTo{ path1.c_str() };
		const int nToLength{ sTo.GetLength() };
#pragma warning(suppress: 26472)
		std::vector<TCHAR> pszTo{ static_cast<size_t>(nToLength) + 2, std::allocator<TCHAR>{} };
		_tcscpy_s(pszTo.data(), pszTo.size(), sTo);
#pragma warning(suppress: 26446 26472)
		pszTo[static_cast<size_t>(nToLength) + 1] = _T('\0');
		shfo.pTo = pszTo.data();

		// Let the shell perform the actual copy
		const int nSuccess{ SHFileOperation(&shfo) };
		if (nSuccess == 0)
		{
			// Set the document title to the new filename
			SetPathName(sTo);
		}
	}
}

/**
 * @brief Updates the UI state of the file copy command
 * @param pCmdUI Pointer to CCmdUI object representing the user interface item
 * @details Enables the command only when an image is loaded
 */
void CIntelliViewDoc::OnUpdateFileCopy(CCmdUI* pCmdUI)
{
#pragma warning(suppress: 26486 26489)
	pCmdUI->Enable(GetView()->m_pImage != nullptr);
}

/**
 * @brief Deletes the current image file
 * @details Uses Shell API to delete the file, supports undo unless SHIFT key is held
 */
void CIntelliViewDoc::OnFileDelete()
{
	// Create a Multi SZ string with the file to delete
	CString sPath{ GetPathName() };
#pragma warning(suppress: 26472)
	const size_t nChars{ static_cast<size_t>(sPath.GetLength()) + 2 };

	// Let the shell do the delete
	SHFILEOPSTRUCT shfo{};
	const CWnd* pMainWnd{ AfxGetMainWnd() };
#pragma warning(suppress: 26496)
	AFXASSUME(pMainWnd != nullptr);
	shfo.hwnd = pMainWnd->GetSafeHwnd(); //NOLINT(clang-analyzer-core.CallAndMessage)
	shfo.wFunc = FO_DELETE;

	// Undo is not allowed if the SHIFT key is held down
	if (!(GetKeyState(VK_SHIFT) & 0x8000))
		shfo.fFlags = FOF_ALLOWUNDO;

	// Should we display any UI
	// if (!theApp.m_bAskMeOnDelete)
	shfo.fFlags |= FOF_NOCONFIRMATION;

	std::vector<TCHAR> pszFrom{ nChars, std::allocator<TCHAR>{} };
	_tcscpy_s(pszFrom.data(), pszFrom.size(), sPath);
#pragma warning(suppress: 26446)
	pszFrom[nChars - 1] = _T('\0');
	shfo.pFrom = pszFrom.data();

	// Let the shell perform the actual deletion
	const int nSuccess{ SHFileOperation(&shfo) };
	if (nSuccess == 0)
	{
		// Since we have deleted the current file
		// move onto the next file in the directory
		GetView()->SendMessage(WM_COMMAND, ID_FILE_CLOSE, 0L);
	}
}

/**
 * @brief Updates the UI state of the file delete command
 * @param pCmdUI Pointer to CCmdUI object representing the user interface item
 * @details Enables the command only when an image is loaded
 */
void CIntelliViewDoc::OnUpdateFileDelete(CCmdUI* pCmdUI)
{
#pragma warning(suppress: 26486 26489)
	pCmdUI->Enable(GetView()->m_pImage != nullptr);
}

/**
 * @brief Displays the Windows properties dialog for the current image file
 * @details Uses ShellExecuteEx to show the file's property sheet
 */
void CIntelliViewDoc::OnProperties()
{
	CString sPathName{ GetPathName() };
	SHELLEXECUTEINFO sei;
	memset(&sei, 0, sizeof(sei));
	sei.cbSize = sizeof(sei);
	sei.hwnd = AfxGetMainWnd()->GetSafeHwnd();
	sei.nShow = SW_SHOW;
	sei.lpFile = sPathName.GetBuffer(sPathName.GetLength());
	sei.lpVerb = _T("properties");
	sei.fMask = SEE_MASK_INVOKEIDLIST;
#pragma warning(suppress: 26486)
	ShellExecuteEx(&sei);
	sPathName.ReleaseBuffer();
}

/**
 * @brief Updates the UI state of the properties command
 * @param pCmdUI Pointer to CCmdUI object representing the user interface item
 * @details Enables the command only when an image is loaded
 */
void CIntelliViewDoc::OnUpdateProperties(CCmdUI* pCmdUI)
{
#pragma warning(suppress: 26486 26489)
	pCmdUI->Enable(GetView()->m_pImage != nullptr);
}
