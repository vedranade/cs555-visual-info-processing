
// mfctest2View.h : interface of the Cmfctest2View class
//

#pragma once


class Cmfctest2View : public CView
{
protected: // create from serialization only
	Cmfctest2View();
	DECLARE_DYNCREATE(Cmfctest2View)

// Attributes
public:
	Cmfctest2Doc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~Cmfctest2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
	public:
	afx_msg void OnNegative();
	afx_msg void OnProcessConverttobinary();
	afx_msg void OnGreyscaledilation();
	afx_msg void OnBinaryimagedilation();
	afx_msg void OnBinaryimageerosion();
	afx_msg void OnGreyscaleerosion();
	afx_msg void OnOpening();
	afx_msg void OnClosing();
	afx_msg void OnTobinary();
};

#ifndef _DEBUG  // debug version in mfctest2View.cpp
inline Cmfctest2Doc* Cmfctest2View::GetDocument() const
   { return reinterpret_cast<Cmfctest2Doc*>(m_pDocument); }
#endif

