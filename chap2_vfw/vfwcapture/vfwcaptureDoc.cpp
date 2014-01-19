// vfwcaptureDoc.cpp : CvfwcaptureDoc 类的实现
//

#include "stdafx.h"
#include "vfwcapture.h"

#include "vfwcaptureDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CvfwcaptureDoc

IMPLEMENT_DYNCREATE(CvfwcaptureDoc, CDocument)

BEGIN_MESSAGE_MAP(CvfwcaptureDoc, CDocument)
END_MESSAGE_MAP()


// CvfwcaptureDoc 构造/析构

CvfwcaptureDoc::CvfwcaptureDoc()
{
	// TODO: 在此添加一次性构造代码

}

CvfwcaptureDoc::~CvfwcaptureDoc()
{
}

BOOL CvfwcaptureDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CvfwcaptureDoc 序列化

void CvfwcaptureDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CvfwcaptureDoc 诊断

#ifdef _DEBUG
void CvfwcaptureDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CvfwcaptureDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CvfwcaptureDoc 命令
