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

// IntelliViewDoc.h : interface of the CIntelliViewDoc class
//

#pragma once

#include "IntelliViewView.h"

class CIntelliViewDoc : public CDocument
{
protected: // create from serialization only
	CIntelliViewDoc() noexcept;
	DECLARE_DYNCREATE(CIntelliViewDoc)

// Attributes
public:
	_NODISCARD CIntelliViewView* GetView() const;

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CIntelliViewDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	virtual void DeleteContents();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	afx_msg void OnFirstPage();
	afx_msg void OnUpdateFirstPage(CCmdUI* pCmdUI);
	afx_msg void OnPrevPage();
	afx_msg void OnUpdatePrevPage(CCmdUI* pCmdUI);
	afx_msg void OnNextPage();
	afx_msg void OnUpdateNextPage(CCmdUI* pCmdUI);
	afx_msg void OnLastPage();
	afx_msg void OnUpdateLastPage(CCmdUI* pCmdUI);
	afx_msg void OnOpenPage();
	afx_msg void OnUpdateOpenPage(CCmdUI* pCmdUI);
	afx_msg void OnAnimation();
	afx_msg void OnUpdateAnimation(CCmdUI* pCmdUI);

	afx_msg void OnFileRename();
	afx_msg void OnUpdateFileRename(CCmdUI* pCmdUI);
	afx_msg void OnFileMove();
	afx_msg void OnUpdateFileMove(CCmdUI* pCmdUI);
	afx_msg void OnFileCopy();
	afx_msg void OnUpdateFileCopy(CCmdUI* pCmdUI);
	afx_msg void OnFileDelete();
	afx_msg void OnUpdateFileDelete(CCmdUI* pCmdUI);
	afx_msg void OnProperties();
	afx_msg void OnUpdateProperties(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
