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

// MainFrame.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "framework.h"
#include "IntelliView.h"

#include "MainFrame.h"
#include "WebBrowserDlg.h"
#include "CheckForUpdatesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @class CMainFrame
 * @brief Main application frame window for IntelliView MDI application
 */

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(IDC_TWITTER, &CMainFrame::OnTwitter)
	ON_COMMAND(IDC_LINKEDIN, &CMainFrame::OnLinkedin)
	ON_COMMAND(IDC_FACEBOOK, &CMainFrame::OnFacebook)
	ON_COMMAND(IDC_INSTAGRAM, &CMainFrame::OnInstagram)
	ON_COMMAND(IDC_ISSUES, &CMainFrame::OnIssues)
	ON_COMMAND(IDC_DISCUSSIONS, &CMainFrame::OnDiscussions)
	ON_COMMAND(IDC_WIKI, &CMainFrame::OnWiki)
	ON_COMMAND(IDC_USER_MANUAL, &CMainFrame::OnUserManual)
	ON_COMMAND(IDC_CHECK_FOR_UPDATES, &CMainFrame::OnCheckForUpdates)
	ON_COMMAND(ID_FULLSCREEN, &CMainFrame::OnFullscreen)
	ON_UPDATE_COMMAND_UI(ID_FULLSCREEN, &CMainFrame::OnUpdateFullscreen)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

/**
 * @brief Constructs a new CMainFrame object
 * @details Initializes fullscreen state to false and window placement structure
 */
CMainFrame::CMainFrame() noexcept : m_bFullScreen{ false },
	m_wpPrev{}
{
}

/**
 * @brief Destroys the CMainFrame object
 */
CMainFrame::~CMainFrame()
{
}

/**
 * @brief Handles the WM_CREATE message to initialize the frame window
 * @param lpCreateStruct Pointer to CREATESTRUCT containing window creation parameters
 * @return 0 on success, -1 on failure
 * @details Sets up ribbon bar, MDI tabbed groups, docking manager, and visual manager
 */
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// BOOL bNameValid;

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // other styles available...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // set to FALSE to place close button at right of tab area
	mdiTabParams.m_bTabIcons = FALSE;    // set to TRUE to enable document icons on MDI taba
	mdiTabParams.m_bAutoColor = TRUE;    // set to FALSE to disable auto-coloring of MDI tabs
	mdiTabParams.m_bDocumentMenu = TRUE; // enable the document menu at the right edge of the tab area
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	/*if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2); */

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// set the visual manager used to draw all user interface elements
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Enable enhanced windows management dialog
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// Switch the order of document name and application name on the window title bar. This
	// improves the usability of the taskbar because the document name is visible with the thumbnail.
	ModifyStyle(0, FWS_PREFIXTITLE);

	return 0;
}

/**
 * @brief Translates window messages before they are dispatched
 * @param pMsg Pointer to MSG structure containing the message to process
 * @return TRUE if message was translated and should not be dispatched, FALSE otherwise
 * @details Handles ESC key to exit fullscreen mode
 */
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// Exit out of fullscreen mode if the user hits "Escape"
	if ((pMsg->message == WM_KEYDOWN) && m_bFullScreen && (pMsg->wParam == VK_ESCAPE))
		PostMessage(WM_COMMAND, ID_FULLSCREEN);

	// Let the base class do its thing
	return __super::PreTranslateMessage(pMsg);
}

/**
 * @brief Modifies the window class or styles before window creation
 * @param cs Reference to CREATESTRUCT containing window creation parameters
 * @return TRUE on success, FALSE on failure
 */
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
/**
 * @brief Validates the object's state (debug builds only)
 * @details Performs diagnostic assertions to verify object integrity
 */
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

/**
 * @brief Dumps the object's state to a diagnostic context (debug builds only)
 * @param dc Reference to CDumpContext for diagnostic output
 */
void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG

// CMainFrame message handlers

/**
 * @brief Handles the Window Manager command
 * @details Displays the windows management dialog for MDI child windows
 */
void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

/**
 * @brief Opens the developer's Twitter/X profile in the default browser
 */
void CMainFrame::OnTwitter()
{
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://x.com/stefanmihaimoga"), nullptr, nullptr, SW_SHOW);
}

/**
 * @brief Opens the developer's LinkedIn profile in the default browser
 */
void CMainFrame::OnLinkedin()
{
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://www.linkedin.com/in/stefanmihaimoga/"), nullptr, nullptr, SW_SHOW);
}

/**
 * @brief Opens the developer's Facebook profile in the default browser
 */
void CMainFrame::OnFacebook()
{
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://www.facebook.com/stefanmihaimoga"), nullptr, nullptr, SW_SHOW);
}

/**
 * @brief Opens the developer's Instagram profile in the default browser
 */
void CMainFrame::OnInstagram()
{
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://www.instagram.com/stefanmihaimoga/"), nullptr, nullptr, SW_SHOW);
}

/**
 * @brief Opens the GitHub Issues page for IntelliView in the default browser
 */
void CMainFrame::OnIssues()
{
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://github.com/mihaimoga/IntelliView/issues"), nullptr, nullptr, SW_SHOW);
}

/**
 * @brief Opens the GitHub Discussions page for IntelliView in the default browser
 */
void CMainFrame::OnDiscussions()
{
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://github.com/mihaimoga/IntelliView/discussions"), nullptr, nullptr, SW_SHOW);
}

/**
 * @brief Opens the GitHub Wiki page for IntelliView in the default browser
 */
void CMainFrame::OnWiki()
{
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://github.com/mihaimoga/IntelliView/wiki"), nullptr, nullptr, SW_SHOW);
}

/**
 * @brief Displays the user manual in a web browser dialog
 * @details Opens a modal dialog containing the application's user manual
 */
void CMainFrame::OnUserManual()
{
	CWebBrowserDlg dlgWebBrowser(this);
	dlgWebBrowser.DoModal();
}

/**
 * @brief Checks for application updates
 * @details Opens a modal dialog to check for and download application updates
 */
void CMainFrame::OnCheckForUpdates()
{
	CCheckForUpdatesDlg dlgCheckForUpdates(this);
	dlgCheckForUpdates.DoModal();
}

/**
 * @brief Toggles fullscreen mode on/off
 * @details Switches between normal window mode and fullscreen mode by adjusting
 *          window placement to cover the entire monitor. Optionally hides the cursor.
 */
void CMainFrame::OnFullscreen()
{
	WINDOWPLACEMENT wpNew{};
	if (!m_bFullScreen)
	{
		//Get the current monitor info
		HMONITOR hMonitor{ MonitorFromWindow(GetSafeHwnd(), MONITOR_DEFAULTTONEAREST) };
		MONITORINFO mi{};
		mi.cbSize = sizeof(mi);
		if (!GetMonitorInfo(hMonitor, &mi))
			return;

		// Need to hide all menubar, toolbar and status bar
		// m_wndRibbonBar.ShowPane(FALSE, FALSE, FALSE);

		//We'll need these to restore the original state
		GetWindowPlacement(&m_wpPrev);
		m_wpPrev.length = sizeof(m_wpPrev);

		// Adjust RECT to new size of window
		::AdjustWindowRectEx(&mi.rcMonitor, GetStyle(), FALSE, GetExStyle());

		// Remember this for OnGetMinMaxInfo()
		m_FullScreenWindowRect = mi.rcMonitor;

		wpNew = m_wpPrev;
		wpNew.showCmd = SW_SHOWNORMAL;
		wpNew.rcNormalPosition = mi.rcMonitor;

		m_bFullScreen = true;

		if (theApp.m_bHideCursorDuringFullScreen)
			ShowCursor(FALSE);
	}
	else
	{
		// Restore the menubar, toolbar and status bar
		m_bFullScreen = false;
		// m_wndRibbonBar.ShowPane(TRUE, FALSE, FALSE);
		wpNew = m_wpPrev;

		ShowCursor(TRUE);
	}

	// Change the window position
	SetWindowPlacement(&wpNew);
}

/**
 * @brief Updates the UI state of the fullscreen command
 * @param pCmdUI Pointer to CCmdUI object representing the user interface item
 * @details Sets the check state of the fullscreen menu/button based on current mode
 */
void CMainFrame::OnUpdateFullscreen(CCmdUI* pCmdUI)
{
#pragma warning(suppress: 26486)
	pCmdUI->SetCheck(m_bFullScreen);
}
