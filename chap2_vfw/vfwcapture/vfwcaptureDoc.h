// vfwcaptureDoc.h : CvfwcaptureDoc ��Ľӿ�
//


#pragma once


class CvfwcaptureDoc : public CDocument
{
protected: // �������л�����
	CvfwcaptureDoc();
	DECLARE_DYNCREATE(CvfwcaptureDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CvfwcaptureDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


