// vfwcaptureView.cpp : CvfwcaptureView ���ʵ��
//

#include "stdafx.h"
#include "vfwcapture.h"

#include "vfwcaptureDoc.h"
#include "vfwcaptureView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern unsigned char bufo[];
//extern int nStreamLength;
//extern int nOstreamSize;
extern LPBITMAPINFO lpbiIn;

// CvfwcaptureView

IMPLEMENT_DYNCREATE(CvfwcaptureView, CView)

BEGIN_MESSAGE_MAP(CvfwcaptureView, CView)
END_MESSAGE_MAP()

// CvfwcaptureView ����/����

CvfwcaptureView::CvfwcaptureView()
{
	// TODO: �ڴ˴���ӹ������
	m_hdd=DrawDibOpen();   // ��DrawDib_xx
}

CvfwcaptureView::~CvfwcaptureView()
{
	DrawDibClose(m_hdd);   // �ر�DrawDib_xx
}

BOOL CvfwcaptureView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CvfwcaptureView ����

void CvfwcaptureView::OnDraw(CDC* pDC)
{
	CvfwcaptureDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

#if 0

	pDC->SetStretchBltMode(STRETCH_DELETESCANS);
	StretchDIBits(pDC->m_hDC,
				  // destination rectangle
				  0,0,lpbiIn->bmiHeader.biWidth,lpbiIn->bmiHeader.biHeight,
				  // source rectangle
				  0,0,lpbiIn->bmiHeader.biWidth,lpbiIn->bmiHeader.biHeight,
				  &bufo[40], 
				  (LPBITMAPINFO )bufo,
				  DIB_RGB_COLORS,
				  SRCCOPY);
#else




	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
	RECT rect;
	GetClientRect(&rect);   //��ÿͻ�����
/*****************Add the bitmap information.********************************************/
	((LPBITMAPINFOHEADER)bufo)->biSize = sizeof(BITMAPINFOHEADER);
	((LPBITMAPINFOHEADER)bufo)->biWidth = lpbiIn->bmiHeader.biWidth;
	((LPBITMAPINFOHEADER)bufo)->biHeight = lpbiIn->bmiHeader.biHeight;
	((LPBITMAPINFOHEADER)bufo)->biPlanes = 1;
	((LPBITMAPINFOHEADER)bufo)->biBitCount = 24;                //24λλͼ
	((LPBITMAPINFOHEADER)bufo)->biCompression = BI_RGB;        //RGB��ʽ
	((LPBITMAPINFOHEADER)bufo)->biSizeImage = lpbiIn->bmiHeader.biWidth*lpbiIn->bmiHeader.biHeight*3;
	((LPBITMAPINFOHEADER)bufo)->biClrUsed = 0;
	((LPBITMAPINFOHEADER)bufo)->biXPelsPerMeter = 0;
	((LPBITMAPINFOHEADER)bufo)->biYPelsPerMeter = 0;
	((LPBITMAPINFOHEADER)bufo)->biClrImportant = 0;
/*****************Add the bitmap information.********************************************/
	DrawDibDraw(m_hdd,
				pDC->GetSafeHdc(),
				rect.left, rect.top,	 rect.right, rect.bottom,
				(LPBITMAPINFOHEADER)bufo, &bufo[40],        //bufo��ǰ40���ֽ���BMPͷ
				0,0, -1,-1,
				0);
#endif

}


// CvfwcaptureView ���

#ifdef _DEBUG
void CvfwcaptureView::AssertValid() const
{
	CView::AssertValid();
}

void CvfwcaptureView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CvfwcaptureDoc* CvfwcaptureView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CvfwcaptureDoc)));
	return (CvfwcaptureDoc*)m_pDocument;
}
#endif //_DEBUG


// CvfwcaptureView ��Ϣ�������
