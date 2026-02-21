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

// CheckForUpdatesDlg.cpp : implementation file
//

#include "pch.h"
#include "IntelliView.h"
#include "CheckForUpdatesDlg.h"

#include "../genUp4win/genUp4win.h"
#if _WIN64
#ifdef _DEBUG
#pragma comment(lib, "../x64/Debug/genUp4win.lib")
#else
#pragma comment(lib, "../x64/Release/genUp4win.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "../Debug/genUp4win.lib")
#else
#pragma comment(lib, "../Release/genUp4win.lib")
#endif
#endif

// CCheckForUpdatesDlg dialog

IMPLEMENT_DYNAMIC(CCheckForUpdatesDlg, CDialogEx)

CCheckForUpdatesDlg::CCheckForUpdatesDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CheckForUpdatesDlg, pParent)
{
	m_nUpdateThreadID = 0;
	m_hUpdateThread = nullptr;
	m_nTimerID = 0;
}

CCheckForUpdatesDlg::~CCheckForUpdatesDlg()
{
}

/**
 * @brief Exchanges data between dialog controls and member variables
 * @param pDX Pointer to a CDataExchange object used for data exchange and validation
 */
void CCheckForUpdatesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATUS, m_ctrlStatusMessage);
	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
}

BEGIN_MESSAGE_MAP(CCheckForUpdatesDlg, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CCheckForUpdatesDlg message handlers
CCheckForUpdatesDlg* g_dlgCheckForUpdates = nullptr;
void UI_Callback(int, const std::wstring& strMessage)
{
	if (g_dlgCheckForUpdates != nullptr)
	{
		g_dlgCheckForUpdates->m_ctrlStatusMessage.SetWindowText(strMessage.c_str());
		g_dlgCheckForUpdates->m_ctrlStatusMessage.UpdateWindow();
	}
}

bool g_bThreadRunning = false;
bool g_bNewUpdateFound = false;
DWORD WINAPI UpdateThreadProc(LPVOID lpParam)
{
	UNREFERENCED_PARAMETER(lpParam);

	g_bThreadRunning = true;
	if (g_dlgCheckForUpdates != nullptr)
	{
		g_dlgCheckForUpdates->m_ctrlProgress.SetMarquee(TRUE, 30);
	}
	const DWORD nLength = 0x1000 /* _MAX_PATH */;
	TCHAR lpszFilePath[nLength] = { 0, };
	GetModuleFileName(nullptr, lpszFilePath, nLength);
	g_bNewUpdateFound = CheckForUpdates(lpszFilePath, APPLICATION_URL, UI_Callback);
	if (g_dlgCheckForUpdates != nullptr)
	{
		g_dlgCheckForUpdates->m_ctrlProgress.SetMarquee(FALSE, 30);
	}
	g_bThreadRunning = false;

	::ExitThread(0);
	// return 0;
}

/**
 * @brief Initializes the dialog when it is created
 * @return TRUE to set focus to the first control, FALSE otherwise
 * @note In debug mode, writes a config file with installer URL
 */
BOOL CCheckForUpdatesDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

#ifdef _DEBUG
	const DWORD nLength = 0x1000 /* _MAX_PATH */;
	TCHAR lpszFilePath[nLength] = { 0, };
	GetModuleFileName(nullptr, lpszFilePath, nLength);
	WriteConfigFile(lpszFilePath, INSTALLER_URL);
#endif

	g_dlgCheckForUpdates = this;
	m_hUpdateThread = ::CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)UpdateThreadProc, this, 0, &m_nUpdateThreadID);
	m_nTimerID = SetTimer(0x1234, 100, nullptr);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Handles the cancel button or ESC key press
 * @note Waits for the update thread to finish before closing the dialog
 */
void CCheckForUpdatesDlg::OnCancel()
{
	while (g_bThreadRunning)
		Sleep(1000);
	CDialogEx::OnCancel();
}

/**
 * @brief Handles timer events to monitor the update check thread status
 * @param nIDEvent ID of the timer that triggered the event
 * @note Closes the dialog when the update thread completes and posts quit message if update found
 */
void CCheckForUpdatesDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialogEx::OnTimer(nIDEvent);

	if (m_nTimerID == nIDEvent)
	{
		if (!g_bThreadRunning)
		{
			VERIFY(KillTimer(m_nTimerID));
			CDialogEx::OnCancel();
			if (g_bNewUpdateFound)
			{
				PostQuitMessage(0);
			}
		}
	}
}
