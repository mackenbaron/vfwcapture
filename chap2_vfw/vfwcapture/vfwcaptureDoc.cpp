// vfwcaptureDoc.cpp : CvfwcaptureDoc ���ʵ��
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


// CvfwcaptureDoc ����/����

CvfwcaptureDoc::CvfwcaptureDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CvfwcaptureDoc::~CvfwcaptureDoc()
{
}

BOOL CvfwcaptureDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CvfwcaptureDoc ���л�

void CvfwcaptureDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CvfwcaptureDoc ���

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


// CvfwcaptureDoc ����
