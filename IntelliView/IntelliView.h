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

// IntelliView.h : main header file for the IntelliView application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "MainFrame.h"
#include "sinstance.h"

// CIntelliViewApp:
// See IntelliView.cpp for the implementation of this class
//

class CIntelliViewApp : public CWinAppEx
{
public:
	CIntelliViewApp() noexcept;

// Member variables
public:
	CInstanceChecker m_pInstanceChecker;
	int m_nCurrentImageIndex;
	std::vector<CString> m_ImagesInCurrentFolder;
	bool m_bPromptStartEndFolder;
	bool m_bHideCursorDuringFullScreen;
	bool m_bMessageBoxUp;
	int m_nSlideshowInterval;
	bool m_bShowFilenameDuringFullScreen;
	COLORREF m_FullScreenTextColor;
	COLORREF m_FullScreenBackgroundColor;
	bool m_bAskMeOnDelete;
	bool m_bSizeToFit;
	bool m_bAutoRotate;

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CIntelliViewApp theApp;
CMainFrame* GetMainFrame();
