// vfwcaptureView.h : CvfwcaptureView ��Ľӿ�
//




#pragma once
#include <vfw.h>

class CvfwcaptureView : public CView
{
protected: // �������л�����
	CvfwcaptureView();
	DECLARE_DYNCREATE(CvfwcaptureView)

// ����
public:
	CvfwcaptureDoc* GetDocument() const;

// ����
public:
	//
	HDRAWDIB m_hdd;
	//~

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// ʵ��
public:
	virtual ~CvfwcaptureView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // vfwcaptureView.cpp �еĵ��԰汾
inline CvfwcaptureDoc* CvfwcaptureView::GetDocument() const
   { return reinterpret_cast<CvfwcaptureDoc*>(m_pDocument); }
#endif

