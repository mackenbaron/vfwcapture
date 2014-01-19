// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "vfwcapture.h"
#include "MainFrm.h"
//#include "FileEncoder.h"
#include <vector>
#include "MP4Stream.h"
#include "MP4Content.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
#define XDIM 720				//ͼ���ȣ�Ԥ����㹻��
#define YDIM 576				//ͼ��߶ȣ�Ԥ����㹻��

unsigned char bufo[XDIM*YDIM*3+40];//�洢����ʾ��ͼ�񣬸�ʽΪRGB24


LPBITMAPINFO    lpbiIn;			//�����ʽ
COMPVARS        pc;				//�������ýṹ��
BOOL			bPreview=FALSE;	//�Ƿ���Ԥ��
CFrameWnd	m_wndSource;		//�������´���
HWND		m_hWndCap;			//VFW�豸����
CRect		disRect;			//��ʾ���ڵĿͻ�����
CMainFrame *pMainFrame=NULL;	//MainFrame ָ��
int				CurrentID;		//������Ƶ����Ƶ������Ƶ
enum		VFW_STATE{PREVIEW,NOWORK};//
VFW_STATE	m_vfwState = PREVIEW;     //��ǰ״̬
///////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

	BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
		ON_WM_CREATE()
		ON_COMMAND(ID_VFW_INIT, &CMainFrame::OnVfwInit)
		ON_COMMAND(ID_VFW_FORMAT, &CMainFrame::OnVfwFormat)
		ON_COMMAND(ID_VFW_CAPTURE, &CMainFrame::OnVfwCapture)
		ON_WM_CLOSE()
	END_MESSAGE_MAP()

	static UINT indicators[] =
	{
		ID_SEPARATOR,           // ״̬��ָʾ��
		ID_INDICATOR_CAPS,
		ID_INDICATOR_NUM,
		ID_INDICATOR_SCRL,
	};

	/////////////////////////////////�ﾶ��ʼ//////////////////////////////////////////////
	DWORD m_Frame;
	DWORD m_WaveFrame;
	COMPVARS m_Com;
	BITMAPINFO m_Ininfo,m_Outinfo;
	PAVIFILE m_pFile;//avi �ļ�ָ��
	AVISTREAMINFO strhdr;//avi��Ϣ
	PAVISTREAM vediostream;//AVI��
	CString m_FilePath;
	PAVISTREAM wavestream;
	PCMWAVEFORMAT format;
	//#include "FileEncoder.h" FileEncoder wavecoder;
	MP4Stream m_Stream;
	EncodeInfo m_Info;



	void EncodeVideo(LPVIDEOHDR lpVHdr)
	{
		if(m_vfwState  != NOWORK){
			m_Stream.AddVideo((char*)lpVHdr->lpData,lpVHdr->dwBytesUsed);
			//	m_Stream.EncodeVideo(lpVHdr->lpData,lpVHdr->dwBytesUsed);
		}
		//#include "FileEncoder.h"wavecoder.VideoEncoder(lpVHdr);
	}
	void EncodeAudio(LPWAVEHDR lpVHdr)
	{
		if(m_vfwState  != NOWORK){
			m_Stream.AddAudio(lpVHdr->lpData,lpVHdr->dwBytesRecorded);
			//m_Stream.EncodeAudio((short *)lpVHdr->lpData,lpVHdr->dwBytesRecorded);
		}
		//#include "FileEncoder.h"wavecoder.AudioEncoder(lpVHdr);
	}

	/////////////////////////////////�ﾶ����///////////////////////////////////////////////



	// CMainFrame ����/����

	CMainFrame::CMainFrame()
	{
		// TODO: �ڴ���ӳ�Ա��ʼ������
	}

	CMainFrame::~CMainFrame()
	{
	}

	int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
			return -1;

#if 0 //	
		if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
			| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
			!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
		{
			TRACE0("δ�ܴ���������\n");
			return -1;      // δ�ܴ���
		}

		if (!m_wndStatusBar.Create(this) ||
			!m_wndStatusBar.SetIndicators(indicators,
			sizeof(indicators)/sizeof(UINT)))
		{
			TRACE0("δ�ܴ���״̬��\n");
			return -1;      // δ�ܴ���
		}

		// TODO: �������Ҫ��������ͣ������ɾ��������
		m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
		EnableDocking(CBRS_ALIGN_ANY);
		DockControlBar(&m_wndToolBar);

#endif //~

		bPreview=FALSE;        //��ʾģʽΪOverlay
		lpbiIn  = new BITMAPINFO;
		pMainFrame=this;

		::GetClientRect(m_wndSource.m_hWnd,&disRect);

		return 0;
	}

	BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
	{
		if( !CFrameWnd::PreCreateWindow(cs) )
			return FALSE;
		// TODO: �ڴ˴�ͨ���޸�
		//  CREATESTRUCT cs ���޸Ĵ��������ʽ

		//
		cs.cx = 640;//352+8;
		cs.cy = 480;//288+54;
		cs.lpszName=_T("VFW�ɼ�");
		cs.style&=~FWS_ADDTOTITLE;
		//~

		return TRUE;
	}



	// CMainFrame ���

#ifdef _DEBUG
	void CMainFrame::AssertValid() const
	{
		CFrameWnd::AssertValid();
	}

	void CMainFrame::Dump(CDumpContext& dc) const
	{
		CFrameWnd::Dump(dc);
	}

#endif //_DEBUG
	/****************************************************************************
	*	VFW callback routines
	***************************************************************************/

	// Function name	: EXPORT ErrorCallbackProc
	// Description	    : 
	// Return type		: LRESULT CALLBACK 
	// Argument         : HWND hWnd
	// Argument         : int nErrID
	// Argument         : LPSTR lpErrorText
	LRESULT CALLBACK EXPORT ErrorCallbackProc(HWND hWnd, int nErrID, LPSTR lpErrorText)
	{
		if(nErrID==0)
			return TRUE;
		MessageBox(NULL,(LPCWSTR)lpErrorText,_T(""),MB_OK);
		return TRUE;
	}


	// Function name	: PASCAL StatusCallbackProc
	// Description	    : 
	// Return type		: LRESULT FAR 
	// Argument         : HWND hWnd
	// Argument         : int nID
	// Argument         : LPSTR lpStatusText
	LRESULT FAR PASCAL StatusCallbackProc(HWND hWnd, int nID, LPSTR lpStatusText)
	{
		if(nID==IDS_CAP_END){
			if((CurrentID==IDS_CAP_STAT_VIDEOAUDIO)||(CurrentID==IDS_CAP_STAT_VIDEOONLY))
				return TRUE;
		}
		CurrentID = nID;
		return (LRESULT) TRUE;
	}


	// Function name	: PASCAL VideoCallbackProc
	// Description	    : Encode the captured frame
	// Return type		: LRESULT FAR 
	// Argument         : HWND hWnd
	// Argument         : LPVIDEOHDR lpVHdr
	LRESULT FAR PASCAL VideoCallbackProc(HWND hWnd, LPVIDEOHDR lpVHdr)
	{
		//unsigned char *bufi;
		//bufi = new unsigned char[lpVHdr->dwBytesUsed+40];	//original image

		//memcpy((void *)(bufi), lpVHdr->lpData, lpVHdr->dwBytesUsed);

		//{
		//	pMainFrame->conv.YV12_to_RGB24(bufi,
		//		bufi+(lpbiIn->bmiHeader.biWidth*lpbiIn->bmiHeader.biHeight),
		//		bufi+(lpbiIn->bmiHeader.biWidth*lpbiIn->bmiHeader.biHeight*5/4),
		//		&bufo[40],
		//		lpbiIn->bmiHeader.biWidth,
		//		lpbiIn->bmiHeader.biHeight);
		//}

		//pMainFrame->GetActiveView()->InvalidateRect(NULL,FALSE);											

		//delete bufi;
		EncodeVideo(lpVHdr);
		return (LRESULT) TRUE;
	}
	// Function name	: PASCAL VideoCallbackProc
	// Description	    : Encode the captured frame
	// Return type		: LRESULT FAR 
	// Argument         : HWND hWnd
	// Argument         : LPVIDEOHDR lpVHdr
	LRESULT FAR PASCAL WaveCallbackProc(HWND hWnd, LPWAVEHDR lpVHdr)
	{
		//unsigned char *bufi;
		//bufi = new unsigned char[lpVHdr->dwBytesUsed];	//original image
		//std::vector<unsigned char> st(lpVHdr->dwBytesUsed);

		//memcpy((void *)(bufi), lpVHdr->lpData, lpVHdr->dwBytesUsed);
		//memcpy(st.data(), lpVHdr->lpData, lpVHdr->dwBytesUsed);

		//delete bufi;
		EncodeAudio(lpVHdr);
		return (LRESULT) TRUE;
	}
	// CMainFrame ��Ϣ�������


	void CMainFrame::OnVfwInit()
	{
		// TODO: �ڴ���������������
		DWORD fsize;
		// ������Ƶ����
		//if(!m_wndSource.CreateEx(
		//	WS_EX_TOPMOST
		//	//WS_EX_DLGMODALFRAME
		//	,NULL,
		//	_T("Source"),WS_OVERLAPPED|WS_CAPTION,
		//	//CRect(0,0,352,288)
		//	CRect(0,0,640,480)
		//	,NULL,0))
		//	return;


		m_hWndCap = capCreateCaptureWindow(_T("Capture Window"),WS_CHILD|WS_VISIBLE,
			0,0,
			640,480,
			//320, 240, 
			this->GetSafeHwnd(),
			//m_wndSource.m_hWnd,
			1);
		capSetCallbackOnError(m_hWndCap,(FARPROC)ErrorCallbackProc);          //����ص�
		capSetCallbackOnStatus(m_hWndCap,(FARPROC)StatusCallbackProc);        //״̬�ص�
		capSetCallbackOnVideoStream(m_hWndCap,(FARPROC)VideoCallbackProc);  //��ȡ��Ƶ��
		capSetCallbackOnWaveStream(m_hWndCap,(FARPROC)WaveCallbackProc);	//��ȡ��Ƶ��
		// ������Ƶ�豸
		capDriverConnect(m_hWndCap,0);	//(HWND m_hWndCap, int index);//index : 0--9
		// ��ȡ�����������ܲ���
		capDriverGetCaps(m_hWndCap,&m_caps,sizeof(CAPDRIVERCAPS));
		//if (m_caps.fHasOverlay)
		capOverlay(m_hWndCap,TRUE);
		// ����Ԥ�����ʿ�ʼԤ��
		capPreviewRate(m_hWndCap,1000/10);
		capPreview(m_hWndCap,TRUE);

		int numBM;LPWAVEFORMATEX format;
		numBM =  capGetAudioFormat(m_hWndCap, (LPARAM) NULL,(WPARAM) sizeof(WAVEFORMATEX));
		format = (LPWAVEFORMATEX) malloc(numBM); 
		numBM =capGetAudioFormat(m_hWndCap,  (LPARAM) format,numBM);
		format->nSamplesPerSec=44100;
		format->nAvgBytesPerSec=44100;
		format->nBlockAlign=2;
		format->wBitsPerSample=16;
		format->nChannels=1;
		capSetAudioFormat(m_hWndCap, format,numBM);

		fsize = capGetVideoFormatSize(m_hWndCap);
		capGetVideoFormat(m_hWndCap, lpbiIn, fsize);
		m_vfwState  = PREVIEW;

		m_Info.m_WaveFormat=format;
		m_Info.m_BitmapInfo=lpbiIn;
		//initatbas();
		//#include "FileEncoder.h"wavecoder.SetHWndCap(m_hWndCap);
		//#include "FileEncoder.h"	wavecoder.InitEncode(&format);

		//AfxMessageBox(_T("��ʼ���ɹ���"));
	}



	void CMainFrame::OnVfwFormat()
	{
		// TODO: �ڴ���������������
		DWORD fsize;
		if(m_caps.fHasDlgVideoFormat){
			capDlgVideoFormat(m_hWndCap);                 //��ʾ��Ƶ��ʽ�Ի���
			fsize = capGetVideoFormatSize(m_hWndCap);     //��ȡ�µĸ�ʽ��С
			capGetVideoFormat(m_hWndCap, lpbiIn, fsize);  //��ȡ�¸�ʽ����
		}
		if(m_caps.fHasDlgVideoSource){
			capDlgVideoSource(m_hWndCap);            //��ʾͼ��Դ���������ȡ�ɫ�Ⱥͱ��Ͷȵ�
		}

	}

	void CMainFrame::OnVfwCapture()
	{
		// TODO: �ڴ���������������
		CAPTUREPARMS CapParms;
		bPreview =! bPreview;                         //����Ԥ������
		if(bPreview){
			capCaptureGetSetup(m_hWndCap,&CapParms,sizeof(CAPTUREPARMS)); //��ȡ����
			CapParms.dwIndexSize=324000;
			CapParms.fMakeUserHitOKToCapture=!CapParms.fMCIControl;
			CapParms.wPercentDropForError=100;      //��֡�ٷֱ�
			CapParms.wNumVideoRequested=5;        //��Ƶ�������������
			CapParms.wChunkGranularity=0;
			CapParms.fYield=TRUE;                 //������̨�߳̽�����Ƶ��׽
			CapParms.fCaptureAudio=TRUE;         //��������Ƶ
			CapParms.vKeyAbort=0;                 //��ֹ����ļ�
			CapParms.fAbortLeftMouse=CapParms.fAbortRightMouse=FALSE;  //������Ҽ�����ֹͣ��׽
			CapParms.dwRequestMicroSecPerFrame=1000001/10;             //�����֡��25f/s
			CapParms.AVStreamMaster=0;
			CapParms.dwAudioBufferSize = 1024*8;// AUDIOBUFFER* 8;
			CapParms.wNumAudioRequested = 0;//40 
			//CapParms.fLimitEnabled=TRUE;
			//CapParms.wTimeLimit=1000001/25;
			//capSetCallbackOnYield(m_hWndCap,NULL);                    //�ص�����Ϊ��
			capCaptureSetSetup(m_hWndCap,&CapParms,sizeof(CAPTUREPARMS));//���ò�׽����
			//���Ҫ������Ƶ������Ҫʹ�ú���ָ���������ļ������򽫻��Զ�����AVI�ļ�
			capCaptureSequenceNoFile(m_hWndCap);
			m_vfwState  = PREVIEW;

			m_Info.m_AudioBitRate =1200;
			m_Info.m_VideoBitRate=1000;
			m_Info.m_FileName="out.mp4";
			m_Info.m_Path="";
			m_Info.m_FrameFps=10;
			m_Info.m_SoundSecond=200;
			m_Stream.InitCode(&m_Info);

			//��ʼ��ѹ���� �ﾶ
			//InitEncode();
		}else{
			capCaptureAbort(m_hWndCap);
			m_vfwState  = NOWORK;

			//�ر�ѹ������avifile �ﾶ
			m_Stream.Close();

		}
	}

	void CMainFrame::OnClose()
	{


		// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
		capSetCallbackOnError(m_hWndCap,NULL);         //ע���ص�����
		capSetCallbackOnStatus(m_hWndCap,NULL);         //ע���ص�����
		capSetCallbackOnVideoStream(m_hWndCap,NULL);   //ע���ص�����
		capSetCallbackOnWaveStream(m_hWndCap,NULL);
		Sleep(1000);
		if (m_vfwState==PREVIEW){
			//ע���ص�����
			capCaptureAbort(m_hWndCap);                    //��ֹ����
			capDriverDisconnect(m_hWndCap);                 //�Ͽ�������������
			Sleep(100);                                     //�ȴ�ǰ��Ĳ������

			delete lpbiIn;
			//delete lpbiTmp;
			//delete lpbiOut;
			Sleep(100);                                     //ȷ��ǰ��Ĳ������

		}
		CFrameWnd::OnClose();
	}

