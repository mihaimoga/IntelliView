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

CMainFrame::CMainFrame() noexcept : m_bFullScreen{ false },
	m_wpPrev{}
{
}

CMainFrame::~CMainFrame()
{
}

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

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// Exit out of fullscreen mode if the user hits "Escape"
	if ((pMsg->message == WM_KEYDOWN) && m_bFullScreen && (pMsg->wParam == VK_ESCAPE))
		PostMessage(WM_COMMAND, ID_FULLSCREEN);

	// Let the base class do its thing
	return __super::PreTranslateMessage(pMsg);
}

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
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG

// CMainFrame message handlers

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnTwitter()
{
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://x.com/stefanmihaimoga"), nullptr, nullptr, SW_SHOW);
}

void CMainFrame::OnLinkedin()
{
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://www.linkedin.com/in/stefanmihaimoga/"), nullptr, nullptr, SW_SHOW);
}

void CMainFrame::OnFacebook()
{
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://www.facebook.com/stefanmihaimoga"), nullptr, nullptr, SW_SHOW);
}

void CMainFrame::OnInstagram()
{
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://www.instagram.com/stefanmihaimoga/"), nullptr, nullptr, SW_SHOW);
}

void CMainFrame::OnIssues()
{
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://github.com/mihaimoga/IntelliView/issues"), nullptr, nullptr, SW_SHOW);
}

void CMainFrame::OnDiscussions()
{
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://github.com/mihaimoga/IntelliView/discussions"), nullptr, nullptr, SW_SHOW);
}

void CMainFrame::OnWiki()
{
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://github.com/mihaimoga/IntelliView/wiki"), nullptr, nullptr, SW_SHOW);
}

void CMainFrame::OnUserManual()
{
	CWebBrowserDlg dlgWebBrowser(this);
	dlgWebBrowser.DoModal();
}

void CMainFrame::OnCheckForUpdates()
{
	CCheckForUpdatesDlg dlgCheckForUpdates(this);
	dlgCheckForUpdates.DoModal();
}

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

void CMainFrame::OnUpdateFullscreen(CCmdUI* pCmdUI)
{
#pragma warning(suppress: 26486)
	pCmdUI->SetCheck(m_bFullScreen);
}
