// MainFrm.cpp : CMainFrame 类的实现
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
#define XDIM 720				//图像宽度，预设的足够大
#define YDIM 576				//图像高度，预设的足够大

unsigned char bufo[XDIM*YDIM*3+40];//存储待显示的图像，格式为RGB24


LPBITMAPINFO    lpbiIn;			//输入格式
COMPVARS        pc;				//编码设置结构体
BOOL			bPreview=FALSE;	//是否在预览
CFrameWnd	m_wndSource;		//创建的新窗口
HWND		m_hWndCap;			//VFW设备窗口
CRect		disRect;			//显示窗口的客户区域
CMainFrame *pMainFrame=NULL;	//MainFrame 指针
int				CurrentID;		//捕获音频、视频或音视频
enum		VFW_STATE{PREVIEW,NOWORK};//
VFW_STATE	m_vfwState = PREVIEW;     //当前状态
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
		ID_SEPARATOR,           // 状态行指示器
		ID_INDICATOR_CAPS,
		ID_INDICATOR_NUM,
		ID_INDICATOR_SCRL,
	};

	/////////////////////////////////田径开始//////////////////////////////////////////////
	DWORD m_Frame;
	DWORD m_WaveFrame;
	COMPVARS m_Com;
	BITMAPINFO m_Ininfo,m_Outinfo;
	PAVIFILE m_pFile;//avi 文件指针
	AVISTREAMINFO strhdr;//avi信息
	PAVISTREAM vediostream;//AVI流
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

	/////////////////////////////////田径结束///////////////////////////////////////////////



	// CMainFrame 构造/析构

	CMainFrame::CMainFrame()
	{
		// TODO: 在此添加成员初始化代码
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
			TRACE0("未能创建工具栏\n");
			return -1;      // 未能创建
		}

		if (!m_wndStatusBar.Create(this) ||
			!m_wndStatusBar.SetIndicators(indicators,
			sizeof(indicators)/sizeof(UINT)))
		{
			TRACE0("未能创建状态栏\n");
			return -1;      // 未能创建
		}

		// TODO: 如果不需要工具栏可停靠，则删除这三行
		m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
		EnableDocking(CBRS_ALIGN_ANY);
		DockControlBar(&m_wndToolBar);

#endif //~

		bPreview=FALSE;        //显示模式为Overlay
		lpbiIn  = new BITMAPINFO;
		pMainFrame=this;

		::GetClientRect(m_wndSource.m_hWnd,&disRect);

		return 0;
	}

	BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
	{
		if( !CFrameWnd::PreCreateWindow(cs) )
			return FALSE;
		// TODO: 在此处通过修改
		//  CREATESTRUCT cs 来修改窗口类或样式

		//
		cs.cx = 640;//352+8;
		cs.cy = 480;//288+54;
		cs.lpszName=_T("VFW采集");
		cs.style&=~FWS_ADDTOTITLE;
		//~

		return TRUE;
	}



	// CMainFrame 诊断

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
	// CMainFrame 消息处理程序


	void CMainFrame::OnVfwInit()
	{
		// TODO: 在此添加命令处理程序代码
		DWORD fsize;
		// 创建视频窗口
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
		capSetCallbackOnError(m_hWndCap,(FARPROC)ErrorCallbackProc);          //出错回调
		capSetCallbackOnStatus(m_hWndCap,(FARPROC)StatusCallbackProc);        //状态回调
		capSetCallbackOnVideoStream(m_hWndCap,(FARPROC)VideoCallbackProc);  //读取视频流
		capSetCallbackOnWaveStream(m_hWndCap,(FARPROC)WaveCallbackProc);	//读取音频流
		// 连接视频设备
		capDriverConnect(m_hWndCap,0);	//(HWND m_hWndCap, int index);//index : 0--9
		// 获取驱动器的性能参数
		capDriverGetCaps(m_hWndCap,&m_caps,sizeof(CAPDRIVERCAPS));
		//if (m_caps.fHasOverlay)
		capOverlay(m_hWndCap,TRUE);
		// 设置预览速率开始预览
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

		//AfxMessageBox(_T("初始化成功！"));
	}



	void CMainFrame::OnVfwFormat()
	{
		// TODO: 在此添加命令处理程序代码
		DWORD fsize;
		if(m_caps.fHasDlgVideoFormat){
			capDlgVideoFormat(m_hWndCap);                 //显示视频格式对话框
			fsize = capGetVideoFormatSize(m_hWndCap);     //读取新的格式大小
			capGetVideoFormat(m_hWndCap, lpbiIn, fsize);  //读取新格式数据
		}
		if(m_caps.fHasDlgVideoSource){
			capDlgVideoSource(m_hWndCap);            //显示图像源参数：亮度、色度和饱和度等
		}

	}

	void CMainFrame::OnVfwCapture()
	{
		// TODO: 在此添加命令处理程序代码
		CAPTUREPARMS CapParms;
		bPreview =! bPreview;                         //捕获预览开关
		if(bPreview){
			capCaptureGetSetup(m_hWndCap,&CapParms,sizeof(CAPTUREPARMS)); //获取参数
			CapParms.dwIndexSize=324000;
			CapParms.fMakeUserHitOKToCapture=!CapParms.fMCIControl;
			CapParms.wPercentDropForError=100;      //丢帧百分比
			CapParms.wNumVideoRequested=5;        //视频缓冲区最大数量
			CapParms.wChunkGranularity=0;
			CapParms.fYield=TRUE;                 //产生后台线程进行视频捕捉
			CapParms.fCaptureAudio=TRUE;         //不捕获音频
			CapParms.vKeyAbort=0;                 //终止捕获的键
			CapParms.fAbortLeftMouse=CapParms.fAbortRightMouse=FALSE;  //鼠标左右键控制停止捕捉
			CapParms.dwRequestMicroSecPerFrame=1000001/10;             //捕获的帧率25f/s
			CapParms.AVStreamMaster=0;
			CapParms.dwAudioBufferSize = 1024*8;// AUDIOBUFFER* 8;
			CapParms.wNumAudioRequested = 0;//40 
			//CapParms.fLimitEnabled=TRUE;
			//CapParms.wTimeLimit=1000001/25;
			//capSetCallbackOnYield(m_hWndCap,NULL);                    //回调函数为空
			capCaptureSetSetup(m_hWndCap,&CapParms,sizeof(CAPTUREPARMS));//配置捕捉参数
			//如果要捕获视频流，则要使用函数指定不生成文件。否则将会自动生成AVI文件
			capCaptureSequenceNoFile(m_hWndCap);
			m_vfwState  = PREVIEW;

			m_Info.m_AudioBitRate =1200;
			m_Info.m_VideoBitRate=1000;
			m_Info.m_FileName="out.mp4";
			m_Info.m_Path="";
			m_Info.m_FrameFps=10;
			m_Info.m_SoundSecond=200;
			m_Stream.InitCode(&m_Info);

			//初始化压缩器 田径
			//InitEncode();
		}else{
			capCaptureAbort(m_hWndCap);
			m_vfwState  = NOWORK;

			//关闭压缩器和avifile 田径
			m_Stream.Close();

		}
	}

	void CMainFrame::OnClose()
	{


		// TODO: 在此添加消息处理程序代码和/或调用默认值
		capSetCallbackOnError(m_hWndCap,NULL);         //注销回调函数
		capSetCallbackOnStatus(m_hWndCap,NULL);         //注销回调函数
		capSetCallbackOnVideoStream(m_hWndCap,NULL);   //注销回调函数
		capSetCallbackOnWaveStream(m_hWndCap,NULL);
		Sleep(1000);
		if (m_vfwState==PREVIEW){
			//注销回调函数
			capCaptureAbort(m_hWndCap);                    //终止捕获
			capDriverDisconnect(m_hWndCap);                 //断开与驱动的连接
			Sleep(100);                                     //等待前面的操作完毕

			delete lpbiIn;
			//delete lpbiTmp;
			//delete lpbiOut;
			Sleep(100);                                     //确保前面的操作完毕

		}
		CFrameWnd::OnClose();
	}

