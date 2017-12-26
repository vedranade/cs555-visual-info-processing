
// Solution5View.h : interface of the CSolution5View class
//

#pragma once


class CSolution5View : public CView
{
protected: // create from serialization only
	CSolution5View();
	DECLARE_DYNCREATE(CSolution5View)

// Attributes
public:
	CSolution5Doc* GetDocument() const;

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
	virtual ~CSolution5View();
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
	afx_msg void OnDivide();
	afx_msg void OnKmeans();
	afx_msg void OnImagen1();
	afx_msg void OnImagen2();
	afx_msg void OnMotionvectors();
	afx_msg void OnImagen3();
	afx_msg void OnImagen4();
	afx_msg void OnImaget1();
};

#ifndef _DEBUG  // debug version in Solution5View.cpp
inline CSolution5Doc* CSolution5View::GetDocument() const
   { return reinterpret_cast<CSolution5Doc*>(m_pDocument); }
#endif

