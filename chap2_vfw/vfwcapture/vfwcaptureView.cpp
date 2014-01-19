// vfwcaptureView.cpp : CvfwcaptureView 类的实现
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

// CvfwcaptureView 构造/析构

CvfwcaptureView::CvfwcaptureView()
{
	// TODO: 在此处添加构造代码
	m_hdd=DrawDibOpen();   // 打开DrawDib_xx
}

CvfwcaptureView::~CvfwcaptureView()
{
	DrawDibClose(m_hdd);   // 关闭DrawDib_xx
}

BOOL CvfwcaptureView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CvfwcaptureView 绘制

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




	// TODO: 在此处为本机数据添加绘制代码
	RECT rect;
	GetClientRect(&rect);   //获得客户区域
/*****************Add the bitmap information.********************************************/
	((LPBITMAPINFOHEADER)bufo)->biSize = sizeof(BITMAPINFOHEADER);
	((LPBITMAPINFOHEADER)bufo)->biWidth = lpbiIn->bmiHeader.biWidth;
	((LPBITMAPINFOHEADER)bufo)->biHeight = lpbiIn->bmiHeader.biHeight;
	((LPBITMAPINFOHEADER)bufo)->biPlanes = 1;
	((LPBITMAPINFOHEADER)bufo)->biBitCount = 24;                //24位位图
	((LPBITMAPINFOHEADER)bufo)->biCompression = BI_RGB;        //RGB格式
	((LPBITMAPINFOHEADER)bufo)->biSizeImage = lpbiIn->bmiHeader.biWidth*lpbiIn->bmiHeader.biHeight*3;
	((LPBITMAPINFOHEADER)bufo)->biClrUsed = 0;
	((LPBITMAPINFOHEADER)bufo)->biXPelsPerMeter = 0;
	((LPBITMAPINFOHEADER)bufo)->biYPelsPerMeter = 0;
	((LPBITMAPINFOHEADER)bufo)->biClrImportant = 0;
/*****************Add the bitmap information.********************************************/
	DrawDibDraw(m_hdd,
				pDC->GetSafeHdc(),
				rect.left, rect.top,	 rect.right, rect.bottom,
				(LPBITMAPINFOHEADER)bufo, &bufo[40],        //bufo的前40个字节是BMP头
				0,0, -1,-1,
				0);
#endif

}


// CvfwcaptureView 诊断

#ifdef _DEBUG
void CvfwcaptureView::AssertValid() const
{
	CView::AssertValid();
}

void CvfwcaptureView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CvfwcaptureDoc* CvfwcaptureView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CvfwcaptureDoc)));
	return (CvfwcaptureDoc*)m_pDocument;
}
#endif //_DEBUG


// CvfwcaptureView 消息处理程序
