// vfwcapture.h : vfwcapture Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CvfwcaptureApp:
// �йش����ʵ�֣������ vfwcapture.cpp
//

class CvfwcaptureApp : public CWinApp
{
public:
	CvfwcaptureApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CvfwcaptureApp theApp;