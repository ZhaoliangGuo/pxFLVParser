
// pxFLVParserDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "pxFLVParser.h"
#include "pxFLVParserDlg.h"
#include "afxdialogex.h"
#include "pxCommonDef.h"
#include "pxFileManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD WINAPI ThreadStartRecordOrAnalyze(LPVOID pParam);

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CpxFLVParserDlg 对话框

CPxFLVParserDlg::CPxFLVParserDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPxFLVParserDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bStop = false;

	::InitializeCriticalSection(&m_csListCtrl);

	m_uiLastVideoTimestamp = 0;
	m_uiLastAudioTimestamp = 0;

	m_bShowVideo = true;
	m_bShowAudio = true;
}

CPxFLVParserDlg::~CPxFLVParserDlg()
{
	::DeleteCriticalSection(&m_csListCtrl);

	m_uiLastVideoTimestamp = 0;
	m_uiLastAudioTimestamp = 0;

	m_bShowVideo = true;
	m_bShowAudio = true;
}


void CPxFLVParserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, m_editHex);
	DDX_Control(pDX, IDC_MFCEDITBROWSE1, m_editFLVPath);
	DDX_Control(pDX, IDC_LIST1, m_lcPackage);
}

BEGIN_MESSAGE_MAP(CPxFLVParserDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START_ANALYZE, &CPxFLVParserDlg::OnBnClickedButtonStartAnalyze)
	ON_MESSAGE(WM_ADD_TAG_TO_LIST,          &CPxFLVParserDlg::AddTag2ListCtrl)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CPxFLVParserDlg::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_CHECK_SHOW_VIDEO_INFO, &CPxFLVParserDlg::OnBnClickedCheckShowVideoInfo)
	ON_BN_CLICKED(IDC_CHECK_SHOW_AUDIO_INFO, &CPxFLVParserDlg::OnBnClickedCheckShowAudioInfo)
	ON_BN_CLICKED(IDC_BUTTON_STOP_ANALYZE, &CPxFLVParserDlg::OnBnClickedButtonStopAnalyze)
END_MESSAGE_MAP()


// CpxFLVParserDlg 消息处理程序

BOOL CPxFLVParserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 获取主界面的句柄
	CWnd *pWnd  = AfxGetMainWnd();
	pWnd->GetWindowText(g_strAppTitle);
	g_hAppWnd = ::FindWindow(NULL, g_strAppTitle);

	// TODO: read config file and init flv path
	Init();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPxFLVParserDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPxFLVParserDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPxFLVParserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPxFLVParserDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	//m_editHex.SetBPR(m_slider.GetPos());
	m_editHex.RedrawWindow();
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPxFLVParserDlg::OnBnClickedButtonStartAnalyze()
{
	UpdateData(TRUE);

	CString strFLVPath("");

	m_editFLVPath.GetWindowText(strFLVPath);

	if (strFLVPath.IsEmpty())
	{
		AfxMessageBox("FLV URL不能为空, 请输入 !!!");

		return ;
	}

	m_lcPackage.DeleteAllItems();

	m_uiLastVideoTimestamp = 0;
	m_uiLastAudioTimestamp = 0;

	m_bStop = false;

	AfxBeginThread((AFX_THREADPROC)ThreadStartRecordOrAnalyze,
		this,
		THREAD_PRIORITY_NORMAL);

	//AfxMessageBox(strFLVPath);

	// TODO: save flv path to ini file
	SaveConfig();

	UpdateData(FALSE);
}

void CPxFLVParserDlg::Init()
{
	UpdateData();

	// 从ini文件中读取RTMP_URL

	char szFLV_URL[_MAX_PATH] = {0};
	GetPrivateProfileString("FLV", 
		"URL", 
		"C:\\demo.flv", 
		szFLV_URL, 
		sizeof(szFLV_URL), 
		g_strConfFile);

	m_editFLVPath.SetWindowText(szFLV_URL);

	m_editFLVPath.EnableFileBrowseButton(NULL,
		_T("Flash Video Files (*.flv)|*.flv|All Files (*.*)|*.*||"));

	//m_strRTMP_URL.Format("%s", szFLV_URL);

	/*char szAVNotSyncThreshold[_MAX_PATH] = {0};
	GetPrivateProfileString("RTMP", 
	"AVNotSyncThreshold", 
	"300", 
	szAVNotSyncThreshold, 
	sizeof(szAVNotSyncThreshold), 
	g_strConfFile);

	m_strAVNotSyncThreshold.Format("%s", szAVNotSyncThreshold);*/

	m_lcPackage.ModifyStyle(0, LVS_REPORT);
	m_lcPackage.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES|LVS_EX_GRIDLINES);

	/*m_lcAgentClient.SetBkColor(RGB(96,96,96)); 
	m_lcAgentClient.SetTextBkColor(RGB(96,96,96));
	m_lcAgentClient.SetTextColor(RGB(255,255,255));*/ // 显示字体的颜色

	m_lcPackage.InsertColumn(0,_T("No."),LVCFMT_RIGHT,60,-1);
	m_lcPackage.InsertColumn(1,_T("TagType"),LVCFMT_LEFT,100,-1);
	m_lcPackage.InsertColumn(2,_T("DataSize(Bytes)"),LVCFMT_CENTER,100,-1);
	m_lcPackage.InsertColumn(3,_T("时间戳(毫秒)"),LVCFMT_CENTER,100,-1);
	m_lcPackage.InsertColumn(4,_T("相邻时间戳差值"), LVCFMT_CENTER,100,-1);
	//m_lcPackage.InsertColumn(5,_T("统计(I帧间)"), LVCFMT_LEFT,80,-1);

	((CButton*)GetDlgItem(IDC_CHECK_SHOW_VIDEO_INFO))->SetCheck(BST_CHECKED);
	((CButton*)GetDlgItem(IDC_CHECK_SHOW_AUDIO_INFO))->SetCheck(BST_CHECKED);

	UpdateData(FALSE);
}

void CPxFLVParserDlg::SaveConfig()
{
	UpdateData();

	CString strFLVPath("");
	m_editFLVPath.GetWindowText(strFLVPath);

	WritePrivateProfileString("FLV", "URL",                strFLVPath,           g_strConfFile);
	//WritePrivateProfileString("RTMP", "AVNotSyncThreshold", m_strAVNotSyncThreshold, g_strConfFile);

	UpdateData(FALSE);
}

#define MSG_BUFFER_SIZE (2048)
char szMsgBuffer[MSG_BUFFER_SIZE] = {0};

vector <SPxFLVTagHeaderEx> vsPxFLVTagHeaderEx;

#define RTMP_PACKET_BUF_SIZE (1024)
SPxFLVTagHeaderEx g_aFLVTagHeaderEx[RTMP_PACKET_BUF_SIZE] = { 0 };

DWORD WINAPI ThreadStartRecordOrAnalyze(LPVOID pParam)
{
	CPxFLVParserDlg *poRTMPAnalyzerDlg = (CPxFLVParserDlg *)pParam;

	char szFLV_URL[_MAX_PATH] = {0};
	CString strFLV_URL("");
	poRTMPAnalyzerDlg->m_editFLVPath.GetWindowText(strFLV_URL);
	strncpy(szFLV_URL, (LPCTSTR)strFLV_URL, sizeof(szFLV_URL));

	vsPxFLVTagHeaderEx.clear();

	__int64 i64FileLen = GetFileLength(szFLV_URL);

	SPxFLVFileHeader sFLVFileHeader;
	memset(&sFLVFileHeader, 0, sizeof(SPxFLVFileHeader));

	FILE *fpFLVFile = fopen(szFLV_URL, "rb");
	if (NULL == fpFLVFile)
	{
		sprintf_s(szMsgBuffer, MSG_BUFFER_SIZE, "打开失败. \n %s", szFLV_URL);
		AfxMessageBox(szMsgBuffer);
		return -1;
	}

	//poRTMPAnalyzerDlg->GetDlgItem(IDC_BUTTON_START_ANALYZE)->EnableWindow(FALSE);
	//poRTMPAnalyzerDlg->GetDlgItem(IDC_BUTTON_STOP_ANALYZE)->EnableWindow(TRUE);

	unsigned int uiReadPos = 0;

	// FLV File header (9 Bytes)
	fread((char *)&sFLVFileHeader, 1, sizeof(SPxFLVFileHeader), fpFLVFile);

	uiReadPos += sizeof(SPxFLVFileHeader);
	uiReadPos += 4; // PreviousTagSize0

	SPxFLVTagHeaderEx sPxFLVTagHeaderEx;

	SPxFLVTagHeader   sPxFLVTagHeader;
	SPxVideoTagHeader sPxVideoTagHeader;
	SPxAudioTagHeader sPxAudioTagHeader;

	unsigned int uiDataSize = 0;

	int nIndex = 0;

	while ((uiReadPos < i64FileLen) && !poRTMPAnalyzerDlg->m_bStop)
	{
		if (nIndex == (RTMP_PACKET_BUF_SIZE - 1))
		{
			nIndex = 0;
		}

		nIndex++;

		memset(&sPxFLVTagHeader,   0, sizeof(SPxFLVTagHeader));
		memset(&sPxVideoTagHeader, 0, sizeof(SPxVideoTagHeader));
		memset(&sPxAudioTagHeader, 0, sizeof(SPxAudioTagHeader));

		fseek(fpFLVFile, uiReadPos, SEEK_SET);
		fread((char *)&sPxFLVTagHeader, 1, sizeof(SPxFLVTagHeader), fpFLVFile);
		uiReadPos += sizeof(SPxFLVTagHeader);

		unsigned int uiDataSize = 0; // Length of the message. 
		                             // Number of bytes after StreamID to end of tag. 
		                             // (Equal to length of the tag – 11)
		uiDataSize =   (sPxFLVTagHeader.uchDataSize[0] << 16)
			         + (sPxFLVTagHeader.uchDataSize[1] << 8)
			         + (sPxFLVTagHeader.uchDataSize[2] << 0);

		if (kePxTagType_Video == sPxFLVTagHeader.uchTagType)
		{
			fseek(fpFLVFile, uiReadPos, SEEK_SET);
			fread((char *)&sPxVideoTagHeader, 1, sizeof(SPxVideoTagHeader), fpFLVFile);
		}
		else if (kePxTagType_Audio == sPxFLVTagHeader.uchTagType)
		{
			fseek(fpFLVFile, uiReadPos, SEEK_SET);
			fread((char *)&sPxAudioTagHeader, 1, sizeof(SPxAudioTagHeader), fpFLVFile);
		}
		else if (kePxTagType_Script == sPxFLVTagHeader.uchTagType)
		{

		}

		uiReadPos += uiDataSize;

		// test by gzl begin
		// Get PreviousTagSize
		fseek(fpFLVFile, uiReadPos, SEEK_SET);
		char szPreviousTagSize[4] = {0};
		fread((char *)&szPreviousTagSize, 1, 4, fpFLVFile);
		int nPreviousTagSize =   (szPreviousTagSize[0] << 24)
			                    + (szPreviousTagSize[1] << 16)
								+ (szPreviousTagSize[2] << 8)
								+  szPreviousTagSize[3];
		// test by gzl end


		uiReadPos += 4;

		g_aFLVTagHeaderEx[nIndex].sPxFLVTagHeader   = sPxFLVTagHeader;
		g_aFLVTagHeaderEx[nIndex].sPxVideoTagHeader = sPxVideoTagHeader;
		g_aFLVTagHeaderEx[nIndex].sPxAudioTagHeader = sPxAudioTagHeader;

		//vsPxFLVTagHeaderEx.push_back(g_aFLVTagHeaderEx[nIndex]);
		//int nPos = vsPxFLVTagHeaderEx.size() - 1;

		::PostMessage(g_hAppWnd, WM_ADD_TAG_TO_LIST, NULL, (LPARAM)&g_aFLVTagHeaderEx[nIndex]);
		Sleep(1);
	}

	if (fpFLVFile)
	{
		fclose(fpFLVFile);
		fpFLVFile = NULL;
	}

	//poRTMPAnalyzerDlg->GetDlgItem(IDC_BUTTON_START_ANALYZE)->EnableWindow(TRUE);
	//poRTMPAnalyzerDlg->GetDlgItem(IDC_BUTTON_STOP_ANALYZE)->EnableWindow(TRUE);

	AfxMessageBox("分析结束(Analyze Finished)!!");

	return 0;
}

#define LIST_COL_CNT (5)

LRESULT CPxFLVParserDlg::AddTag2ListCtrl( WPARAM wParam, LPARAM lParam )
{
	::EnterCriticalSection(&m_csListCtrl);

	SPxFLVTagHeaderEx *psFLVTagHeaderEx = (SPxFLVTagHeaderEx *)lParam;
	if (NULL == psFLVTagHeaderEx)
	{
		g_strMsg = "CPxFLVParserDlg::AddTag2ListCtrl psFLVTagHeaderEx is NULL";
		g_logFile.WriteLogInfo(g_strMsg);

		return -1;
	}

	if (kePxTagType_Video == psFLVTagHeaderEx->sPxFLVTagHeader.uchTagType)
	{
		if (!m_bShowVideo)
		{
			::LeaveCriticalSection(&m_csListCtrl);

			return 0;
		}	
	}

	if (kePxTagType_Audio == psFLVTagHeaderEx->sPxFLVTagHeader.uchTagType)
	{
		if (!m_bShowAudio)
		{
			::LeaveCriticalSection(&m_csListCtrl);

			return 0;
		}
	}

	m_lcPackage.SetRedraw(FALSE);

	int nCnt     = m_lcPackage.GetItemCount();
	int maxIndex = m_lcPackage.InsertItem(nCnt, _T(""));//The line_pos is the Index value
	CString strIndex;
	strIndex.Format("%d", nCnt + 1);

	m_lcPackage.SetItemText(maxIndex, 0, strIndex);

	CString strTagType("");

	if (kePxTagType_Script == psFLVTagHeaderEx->sPxFLVTagHeader.uchTagType)
	{
		strTagType = "Script";
	}
	else if (kePxTagType_Video == psFLVTagHeaderEx->sPxFLVTagHeader.uchTagType)
	{
		strTagType = "Video";

		if (kePxVideoFrameType_KeyFrame == psFLVTagHeaderEx->sPxVideoTagHeader.uchFrameType)
		{
			strTagType = "Video (I帧)";

			for (int i = 0; i < LIST_COL_CNT; i++)
			{
				m_lcPackage.SetItemTextColor(maxIndex, i, RGB(255, 255, 255));		
				m_lcPackage.SetItemBkColor(maxIndex,   i, RGB(61, 145,  64));
			}
		}
		else if (kePxVideoFrameType_InterFrame == psFLVTagHeaderEx->sPxVideoTagHeader.uchFrameType)
		{
			strTagType = "Video (P帧/B帧)";
		}
		else
		{
			strTagType.Format("Video %d", psFLVTagHeaderEx->sPxVideoTagHeader.uchFrameType);

			for (int i = 0; i < LIST_COL_CNT; i++)
			{
				m_lcPackage.SetItemTextColor(maxIndex, i, RGB(255, 255, 255));		
				m_lcPackage.SetItemBkColor(maxIndex,   i, RGB(153, 50, 204));
			}
		}	
	}
	else if (kePxTagType_Audio == psFLVTagHeaderEx->sPxFLVTagHeader.uchTagType)
	{
		strTagType = "Audio";
	}

	m_lcPackage.SetItemText(maxIndex, 1, strTagType);
	/*m_lcAgentClient.SetItemTextColor(maxIndex, 2, RGB(255,255,255));
	m_lcAgentClient.SetItemBkColor(maxIndex, 2, RGB(96,96,96));*/

	unsigned int uiDataSize = 0;
	uiDataSize =   (psFLVTagHeaderEx->sPxFLVTagHeader.uchDataSize[0] << 16)
		+ (psFLVTagHeaderEx->sPxFLVTagHeader.uchDataSize[1] << 8)
		+ (psFLVTagHeaderEx->sPxFLVTagHeader.uchDataSize[2] << 0);

	CString strDataSize("");
	strDataSize.Format("%u", uiDataSize);
	m_lcPackage.SetItemText(maxIndex, 2, strDataSize);

	unsigned int uiTimeStamp = 0;
	uiTimeStamp =   (psFLVTagHeaderEx->sPxFLVTagHeader.uchTimestampExtended << 24)
		          + (psFLVTagHeaderEx->sPxFLVTagHeader.uchTimestamp[0]      << 16)
		          + (psFLVTagHeaderEx->sPxFLVTagHeader.uchTimestamp[1]      << 8) 
		          + (psFLVTagHeaderEx->sPxFLVTagHeader.uchTimestamp[2]      << 0);

	CString strTimeStamp("");
	strTimeStamp.Format("%u", uiTimeStamp);

	m_lcPackage.SetItemText(maxIndex, 3, strTimeStamp);
	/*m_lcAgentClient.SetItemTextColor(maxIndex, 4, RGB(255,255,255));
	m_lcAgentClient.SetItemBkColor(maxIndex, 4, RGB(96,96,96));*/

	CString strDeltaTimestamp("");
	int nDeltaTimestamp = 0;

	bool    bOutSync = true; // 是否同步

	if (kePxTagType_Video == psFLVTagHeaderEx->sPxFLVTagHeader.uchTagType)
	{
		if (0 == m_uiLastVideoTimestamp)
		{
			nDeltaTimestamp = 0;
		}
		else
		{
			nDeltaTimestamp = uiTimeStamp - m_uiLastVideoTimestamp;
		}	
	}
	else if (kePxTagType_Audio == psFLVTagHeaderEx->sPxFLVTagHeader.uchTagType)
	{
		if (0 == m_uiLastAudioTimestamp)
		{
			nDeltaTimestamp = 0;
		}
		else
		{
			nDeltaTimestamp = uiTimeStamp - m_uiLastAudioTimestamp;
		}
	}

	strDeltaTimestamp.Format("%u", nDeltaTimestamp);

	m_lcPackage.SetItemText(maxIndex, 4, strDeltaTimestamp);

	if (nDeltaTimestamp > 200 || nDeltaTimestamp < -200)
	{
		for (int i = 0; i < LIST_COL_CNT; i++)
		{
			m_lcPackage.SetItemTextColor(maxIndex, i, RGB(255,255,255));
			//m_lcAgentClient.SetItemBkColor(maxIndex,i, RGB(210, 105, 30));
			m_lcPackage.SetItemBkColor(maxIndex,   i, RGB(227, 23,  13));
		}
	}

	if (kePxTagType_Video == psFLVTagHeaderEx->sPxFLVTagHeader.uchTagType)
	{
		m_uiLastVideoTimestamp = uiTimeStamp;
	}
	else if (kePxTagType_Audio == psFLVTagHeaderEx->sPxFLVTagHeader.uchTagType)
	{
		m_uiLastAudioTimestamp = uiTimeStamp;
	}

	//设置最后一行被选中
	m_lcPackage.SetItemState(m_lcPackage.GetItemCount() - 1, 
		LVIS_ACTIVATING | LVIS_FOCUSED | LVIS_SELECTED,  
		LVIS_SELECTED | LVIS_FOCUSED );
	//滚动到最后一行
	m_lcPackage.Scroll(CSize(0, 100000));

	m_lcPackage.SetRedraw(TRUE);

	::LeaveCriticalSection(&m_csListCtrl); 

	return 0;
}

// Test Unit
void CPxFLVParserDlg::OnBnClickedButtonTest()
{
	TestTag2ListCtrl(); 
}

SPxFLVTagHeaderEx g_sTestTagHeaderEx;

// Test Unit
void CPxFLVParserDlg::TestTag2ListCtrl()
{
	memset(&g_sTestTagHeaderEx, 0, sizeof(SPxFLVTagHeaderEx));

	SPxFLVTagHeader   sPxFLVTagHeader;
	SPxVideoTagHeader sPxVideoTagHeader;
	SPxAudioTagHeader sPxAudioTagHeader;

	sPxFLVTagHeader.uchTagType = 9;

	unsigned int uiTimestamp = 1234;
	sPxFLVTagHeader.uchTimestampExtended = (BYTE)((uiTimestamp >> 24) & 0xFF);
	sPxFLVTagHeader.uchTimestamp[0]      = (BYTE)((uiTimestamp >> 16) & 0xFF);
	sPxFLVTagHeader.uchTimestamp[1]      = (BYTE)((uiTimestamp >> 8)  & 0xFF);
	sPxFLVTagHeader.uchTimestamp[2]      = (BYTE)((uiTimestamp >> 0)  & 0xFF);

	unsigned int uiDataSize = 5678;
	sPxFLVTagHeader.uchDataSize[0] = (BYTE)((uiDataSize >> 16) & 0xFF);
	sPxFLVTagHeader.uchDataSize[1] = (BYTE)((uiDataSize >> 8)  & 0xFF);
	sPxFLVTagHeader.uchDataSize[2] = (BYTE)((uiDataSize >> 0)  & 0xFF);

	//sPxVideoTagHeader.uchFrameType         = kePxVideoFrameType_InterFrame;
	sPxVideoTagHeader.uchFrameType         = kePxVideoFrameType_KeyFrame;
	sPxVideoTagHeader.uchCodecID           = kePxVideoCodecID_AVC;
	sPxVideoTagHeader.uchAVCPacketType     = kePxAVC_NALU_Type_SLICE_IDR;
	sPxVideoTagHeader.chCompositionTime[0] = 0x00;
	sPxVideoTagHeader.chCompositionTime[1] = 0x00;
	sPxVideoTagHeader.chCompositionTime[2] = 0x00;

	g_sTestTagHeaderEx.sPxFLVTagHeader   = sPxFLVTagHeader;
	g_sTestTagHeaderEx.sPxVideoTagHeader = sPxVideoTagHeader;

	::PostMessage(GetSafeHwnd(), WM_ADD_TAG_TO_LIST, NULL, (LPARAM)&g_sTestTagHeaderEx);
}

void CPxFLVParserDlg::OnBnClickedCheckShowVideoInfo()
{
	UpdateData();

	if (((CButton *)GetDlgItem(IDC_CHECK_SHOW_VIDEO_INFO))->GetCheck() == BST_CHECKED)
	{
		m_bShowVideo = true; 
		g_strMsg.Format("Show Video analyze info.");
	}
	else
	{
		m_bShowVideo = false; 
		g_strMsg.Format("Do not Show Video analyze info.");
	}

	//::PostMessage(g_hAppWnd, WM_ADD_LOG_TO_LIST, NULL, (LPARAM)g_strMsg.GetBuffer());

	UpdateData(FALSE);
}


void CPxFLVParserDlg::OnBnClickedCheckShowAudioInfo()
{
	UpdateData();

	if (((CButton *)GetDlgItem(IDC_CHECK_SHOW_AUDIO_INFO))->GetCheck() == BST_CHECKED)
	{
		m_bShowAudio = true; 
		g_strMsg.Format("Show Audio analyze info.");
	}
	else
	{
		m_bShowAudio = false; 
		g_strMsg.Format("Do not Show Audio analyze info.");
	}

	//::PostMessage(g_hAppWnd, WM_ADD_LOG_TO_LIST, NULL, (LPARAM)g_strMsg.GetBuffer());

	UpdateData(FALSE);
}


void CPxFLVParserDlg::OnBnClickedButtonStopAnalyze()
{
	m_bStop = true;

	GetDlgItem(IDC_BUTTON_START_ANALYZE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP_ANALYZE)->EnableWindow(TRUE);
}
