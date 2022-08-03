﻿
// SLTMDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "SLTM.h"
#include "SLTMDlg.h"
#include "afxdialogex.h"
#ifdef HKWS
#include "plaympeg4.h"
#endif

#include "hv/HttpServer.h"
#include "hv/requests.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SETTING_FILE  _T("C:\\SLTM\\SLTMsetting.ini")

// CSLTMDlg 对话框
float g_fTempData0[PIC_HEIGHT*PIC_WIDTH];
float g_fTempData1[PIC_HEIGHT*PIC_WIDTH];
float g_fTempData2[PIC_HEIGHT*PIC_WIDTH];

int g_iFPS[3] = { 0, 0, 0 };

CString g_devTitleName;
HttpService router;
http_server_t server;

CSLTMDlg::CSLTMDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SLTM_DIALOG, pParent)
	, m_maxFrameRate(0)
	, m_reflectiveEnable(FALSE)
	, m_reflectiveTemp(0)
	, m_emissivity(0)
	, m_distance(0)
	, m_refreshInterval(0)
	, m_DataLength(0)
	, m_jpegPicEnabled(FALSE)
	, m_visJpegPicEnabled(FALSE)
	//, lUserID(-1)
	, lChannel(0)
	, llRealHandle(-1)
	, llRealHandle2(-1)
	, llRealHandle3(-1)
	, lRealTimeInfoHandle(-1)
	, m_StaticLog(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSLTMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_LOG, m_StaticLog);
	DDX_Control(pDX, IDC_CAMERA1, m_staticImage1);
	DDX_Control(pDX, IDC_CAMERA2, m_staticImage2);
	DDX_Control(pDX, IDC_CAMERA3, m_staticImage3);
}

BEGIN_MESSAGE_MAP(CSLTMDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_QUIT, &CSLTMDlg::OnBnClickedButtonQuit)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CSLTMDlg::OnBnClickedButtonLogin)
	ON_BN_CLICKED(IDC_BUTTON_LOGOUT, &CSLTMDlg::OnBnClickedButtonLogout)
	ON_BN_CLICKED(IDC_BUTTON_Get_Hot_Pic, &CSLTMDlg::OnBnClickedButtonGetHotPic)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_TESTDATA, &CSLTMDlg::OnBnClickedButtonTestdata)
END_MESSAGE_MAP()


// CSLTMDlg 消息处理程序

BOOL CSLTMDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	// 执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_MAXIMIZE);

	// 读取IP与初始化海康威视SDK
	ReadPara();
	GetDlgItem(IDC_STATIC_TITLE)->SetWindowText(g_devTitleName);
	// NET_DVR_Init();
	StartHTTPServer();
	SetTimer(1, 1000, NULL);
#if 0
	clDevice.setIP((LPSTR)(LPCTSTR)g_csDeviceIP[0]);
	clDevice2.setIP((LPSTR)(LPCTSTR)g_csDeviceIP[1]);
	clDevice3.setIP((LPSTR)(LPCTSTR)g_csDeviceIP[2]);
#endif


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
// 来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
// 这将由框架自动完成。

void CSLTMDlg::OnPaint()
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

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CSLTMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/*************************************************
函数名:    	ReadPara & WritePara
函数描述:	读写相机设备参数，目前只有3个IP
输入参数:
输出参数:
返回值:
**************************************************/
BOOL WritePara()
{
	CString devIP[3];
	
	devIP[0] = CString("192.168.0.101");
	devIP[1] = CString("192.168.0.102");
	devIP[2] = CString("192.168.0.103");

	CString devTitle = CString("1#钢包");
	//WritePrivateProfileString(Section名,Key名,CString数据,文件地址);
	
	BOOL bwrite[4];
	bwrite[0] = WritePrivateProfileString(_T("Device"), _T("IP0"), devIP[0], SETTING_FILE);
	bwrite[1] = WritePrivateProfileString(_T("Device"), _T("IP1"), devIP[1], SETTING_FILE);
	bwrite[2] = WritePrivateProfileString(_T("Device"), _T("IP2"), devIP[2], SETTING_FILE);
	bwrite[3] = WritePrivateProfileString(_T("Title"), _T("Name"), devTitle, SETTING_FILE);
	if (bwrite[0] && bwrite[1] && bwrite[2] && bwrite[3])
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
BOOL CSLTMDlg::ReadPara()
{
	CFileFind finder;// 定义标准类对象
	BOOL isFind;
	isFind = finder.FindFile(SETTING_FILE);// 是否查找到文件（内为文件路径）
	if (!isFind)
		WritePara();

	CString devIP[3];
	CString devTitle;
	BOOL bwrite[4];
	// Section 名,Key 名,默认值,存储字符串,字符串所允许的最大长度(15),文件路径
	bwrite[0] = GetPrivateProfileString(_T("Device"), _T("IP0"), _T("NULL"), devIP[0].GetBufferSetLength(16), 16, SETTING_FILE);
	bwrite[1] = GetPrivateProfileString(_T("Device"), _T("IP1"), _T("NULL"), devIP[1].GetBufferSetLength(16), 16, SETTING_FILE);
	bwrite[2] = GetPrivateProfileString(_T("Device"), _T("IP2"), _T("NULL"), devIP[2].GetBufferSetLength(16), 16, SETTING_FILE);
	bwrite[3] = GetPrivateProfileString(_T("Title"), _T("Name"), _T("NULL"), devTitle.GetBufferSetLength(16), 16, SETTING_FILE);
	// 此操作对应于GetBufferSetLength(),紧跟其后，不能忽略。此操作的作用是将GetBufferSetLength()申请的多余的内存空间释放掉，以便于可以进行后续的如字符串+操作
	devIP[0].ReleaseBuffer();
	devIP[1].ReleaseBuffer();
	devIP[2].ReleaseBuffer();

	if (!(bwrite[0] && bwrite[1] && bwrite[2] && bwrite[3]))
	{
		return FALSE;
	}
	for (int i = 0; i < 3; i++)
	{
		g_csDeviceIP[i] = devIP[i];
		g_devTitleName = devTitle;
	}
	return TRUE;
}


/*************************************************
函数名:    	GetP2PParam
函数描述:	海康威视参数设置
输入参数:
输出参数:
返回值:
**************************************************/
#ifdef HKWS
int CSLTMDlg::GetP2PParam()
{
	XmlNode xmlNodeList[100];
	int cmdCount = 0;
	CString Log;
	CString csLogString;
	int ret = 0;

	char OutBuffer[10240] = { 0 };
	char InBuffer[10240] = { 0 };
	NET_DVR_XML_CONFIG_INPUT    lpInputParam = { 0 };
	NET_DVR_XML_CONFIG_OUTPUT   lpOutputParam = { 0 };

	int channel = 0;

	ST_XML_GET_NODE_VALUE_TABLE xml_node_tbl[] =
	{
		{ "id", XML_NODE_INT, NULL, NULL, NULL },
		{ "maxFrameRate", XML_NODE_INT, &m_maxFrameRate, NULL, NULL },
		{ "reflectiveEnable", XML_NODE_BOOL, &m_reflectiveEnable, NULL, NULL },
		{ "reflectiveTemperature", XML_NODE_FLOAT, &m_reflectiveTemp, NULL, NULL },
		{ "emissivity", XML_NODE_FLOAT, &m_emissivity, NULL, NULL },
		{ "distance", XML_NODE_INT, &m_distance, NULL, NULL },
		{ "refreshInterval", XML_NODE_INT, &m_refreshInterval, NULL, NULL },
		{ "temperatureDataLength", XML_NODE_INT, &m_DataLength, NULL, NULL },
		{ "jpegPicEnabled", XML_NODE_BOOL, &m_jpegPicEnabled, NULL, NULL },
		{ "visiblePicEnable", XML_NODE_BOOL, &m_visJpegPicEnabled, NULL, NULL },	//此节点为兼容之前的节点
		{ "visiblePicEnabled", XML_NODE_BOOL, &m_visJpegPicEnabled, NULL, NULL },
	};

	lpInputParam.dwSize = sizeof(lpInputParam);
	lpOutputParam.dwSize = sizeof(lpOutputParam);

	if (0 == channel)
	{
		lpInputParam.lpRequestUrl = "GET /ISAPI/Thermal/channels/1/thermometry/pixelToPixelParam";
		lpInputParam.dwRequestUrlLen = sizeof("GET /ISAPI/Thermal/channels/1/thermometry/pixelToPixelParam");
	}
	else if (1 == channel)
	{
		lpInputParam.lpRequestUrl = "GET /ISAPI/Thermal/channels/2/thermometry/pixelToPixelParam";
		lpInputParam.dwRequestUrlLen = sizeof("GET /ISAPI/Thermal/channels/2/thermometry/pixelToPixelParam");
	}
	else
	{
		lpInputParam.lpRequestUrl = "GET /ISAPI/Thermal/channels/1/thermometry/pixelToPixelParam";
		lpInputParam.dwRequestUrlLen = sizeof("GET /ISAPI/Thermal/channels/1/thermometry/pixelToPixelParam");
	}

	lpOutputParam.lpOutBuffer = OutBuffer;
	lpOutputParam.dwOutBufferSize = sizeof(OutBuffer);

	if (NET_DVR_STDXMLConfig(clDevice.UserID, &lpInputParam, &lpOutputParam))
	{
		cmdCount = getXmlNodeList((char*)lpOutputParam.lpOutBuffer, "<PixelToPixelParam", xmlNodeList);
		if (0 == cmdCount)
		{
			SetDlgItemText(IDC_STATIC_LOG, "No Test Parameter");
		}
		else
		{
			getXmlNodeValue(xml_node_tbl, sizeof(xml_node_tbl) / sizeof(xml_node_tbl[0]), xmlNodeList, cmdCount);
		}

		m_StaticLog.Format("%s", "Get pixelToPixelParam success!");
		csLogString.Format("Get P2P param success! channel:%d", channel);
		ret = 0;
	}
	else
	{
		Log.Format("Get pixelToPixelParam failed!%s(%d)", NET_DVR_GetErrorMsg(), NET_DVR_GetLastError());
		m_StaticLog.Format("%s", Log);
		csLogString.Format("Get P2P param failed! channel:%d, %s(%d)", channel, NET_DVR_GetErrorMsg(), NET_DVR_GetLastError());
		ret = -1;
	}

	if (NET_DVR_STDXMLConfig(clDevice2.UserID, &lpInputParam, &lpOutputParam))
	{
		cmdCount = getXmlNodeList((char*)lpOutputParam.lpOutBuffer, "<PixelToPixelParam", xmlNodeList);
		if (0 == cmdCount)
		{
			SetDlgItemText(IDC_STATIC_LOG, "No Test Parameter");
		}
		else
		{
			getXmlNodeValue(xml_node_tbl, sizeof(xml_node_tbl) / sizeof(xml_node_tbl[0]), xmlNodeList, cmdCount);
		}

		m_StaticLog.Format("%s", "Get pixelToPixelParam success!");
		csLogString.Format("Get P2P param success! channel:%d", channel);
		ret = 0;
	}
	else
	{
		Log.Format("Get pixelToPixelParam failed!%s(%d)", NET_DVR_GetErrorMsg(), NET_DVR_GetLastError());
		m_StaticLog.Format("%s", Log);
		csLogString.Format("Get P2P param failed! channel:%d, %s(%d)", channel, NET_DVR_GetErrorMsg(), NET_DVR_GetLastError());
		ret = -1;
	}

	if (NET_DVR_STDXMLConfig(clDevice3.UserID, &lpInputParam, &lpOutputParam))
	{
		cmdCount = getXmlNodeList((char*)lpOutputParam.lpOutBuffer, "<PixelToPixelParam", xmlNodeList);
		if (0 == cmdCount)
		{
			SetDlgItemText(IDC_STATIC_LOG, "No Test Parameter");
		}
		else
		{
			getXmlNodeValue(xml_node_tbl, sizeof(xml_node_tbl) / sizeof(xml_node_tbl[0]), xmlNodeList, cmdCount);
		}

		m_StaticLog.Format("%s", "Get pixelToPixelParam success!");
		csLogString.Format("Get P2P param success! channel:%d", channel);
		ret = 0;
	}
	else
	{
		Log.Format("Get pixelToPixelParam failed!%s(%d)", NET_DVR_GetErrorMsg(), NET_DVR_GetLastError());
		m_StaticLog.Format("%s", Log);
		csLogString.Format("Get P2P param failed! channel:%d, %s(%d)", channel, NET_DVR_GetErrorMsg(), NET_DVR_GetLastError());
		ret = -1;
	}

	return ret;
}
#endif

/*************************************************
函数名:    	MatToCImage
函数描述:	Mat转CImage
输入参数:	cimage-CImage图片数据， mat-Mat 图片数据
输出参数:
返回值:
**************************************************/
void MatToCImage(Mat& mat, CImage& cimage)
{
	if (0 == mat.total())
	{
		return;
	}


	int nChannels = mat.channels();
	if ((1 != nChannels) && (3 != nChannels))
	{
		return;
	}
	int nWidth = mat.cols;
	int nHeight = mat.rows;


	//重建cimage
	cimage.Destroy();
	cimage.Create(nWidth, nHeight, 8 * nChannels);


	//拷贝数据


	uchar* pucRow;									//指向数据区的行指针
	uchar* pucImage = (uchar*)cimage.GetBits();		//指向数据区的指针
	int nStep = cimage.GetPitch();					//每行的字节数,注意这个返回值有正有负


	if (1 == nChannels)								//对于单通道的图像需要初始化调色板
	{
		RGBQUAD* rgbquadColorTable;
		int nMaxColors = 256;
		rgbquadColorTable = new RGBQUAD[nMaxColors];
		cimage.GetColorTable(0, nMaxColors, rgbquadColorTable);
		for (int nColor = 0; nColor < nMaxColors; nColor++)
		{
			rgbquadColorTable[nColor].rgbBlue = (uchar)nColor;
			rgbquadColorTable[nColor].rgbGreen = (uchar)nColor;
			rgbquadColorTable[nColor].rgbRed = (uchar)nColor;
		}
		cimage.SetColorTable(0, nMaxColors, rgbquadColorTable);
		delete[]rgbquadColorTable;
	}


	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (mat.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			if (1 == nChannels)
			{
				*(pucImage + nRow * nStep + nCol) = pucRow[nCol];
			}
			else if (3 == nChannels)
			{
				for (int nCha = 0; nCha < 3; nCha++)
				{
					*(pucImage + nRow * nStep + nCol * 3 + nCha) = pucRow[nCol * 3 + nCha];
				}
			}
		}
	}
}


/*************************************************
函数名:    	CImageToMat
函数描述:	CImage转Mat
输入参数:	cimage-CImage图片数据， mat-Mat 图片数据
输出参数:
返回值:
**************************************************/
void CImageToMat(CImage& cimage, Mat& mat)
{
	if (true == cimage.IsNull())
	{
		return;
	}

	int nChannels = cimage.GetBPP() / 8;
	if ((1 != nChannels) && (3 != nChannels))
	{
		return;
	}
	int nWidth = cimage.GetWidth();
	int nHeight = cimage.GetHeight();


	//重建mat
	if (1 == nChannels)
	{
		mat.create(nHeight, nWidth, CV_8UC1);
	}
	else if (3 == nChannels)
	{
		mat.create(nHeight, nWidth, CV_8UC3);
	}
	else if (4 == nChannels)
	{
		mat.create(nHeight, nWidth, CV_8UC4);
	}

	//拷贝数据
	uchar* pucRow;									//指向数据区的行指针
	uchar* pucImage = (uchar*)cimage.GetBits();		//指向数据区的指针
	int nStep = cimage.GetPitch();					//每行的字节数,注意这个返回值有正有负

	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (mat.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			if (1 == nChannels)
			{
				pucRow[nCol] = *(pucImage + nRow * nStep + nCol);
			}
			else if (3 == nChannels || 4 == nChannels)
			{
				for (int nCha = 0; nCha < nChannels; nCha++)
				{
					pucRow[nCol * 3 + nCha] = *(pucImage + nRow * nStep + nCol * nChannels + nCha);
				}
			}
		}
	}
}

#ifdef HKWS
CameraLink::CameraLink()
{
	struLoginInfo.bUseAsynLogin = 0; //同步登录方式
	struLoginInfo.wPort = DevicePort; //设备服务端口
	strcpy_s(struLoginInfo.sUserName, UserName); //设备登录用户名
	strcpy_s(struLoginInfo.sPassword, Password); //设备登录密码
}

LONG CameraLink::getUserID()
{
	return this->UserID;
}

void CameraLink::setIP(char *inIP)
{
	this->IP = inIP;
}

string CameraLink::Connect()
{

	strcpy_s(struLoginInfo.sDeviceAddress, IP); //设备IP地址
	UserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);
	if (UserID < 0)
	{
		char *cc = NET_DVR_GetErrorMsg();
		string sOutput(cc);
		sOutput = "Login Error:" + sOutput;
		int errnum = (int)(NET_DVR_GetLastError());
		sOutput = sOutput + to_string(errnum);
		return sOutput;
	}
	return "";
}

string CameraLink::Disconnect()
{
	if (UserID < 0)
	{
		// "注销：未登录！"
		string sOutput = "Logout: No Login!";
		return sOutput;
	}

	if (NET_DVR_Logout(UserID))
	{
		// "注销：注销成功！"
		UserID = -1;
		
		return "";
	}
	else
	{
		char *cc = NET_DVR_GetErrorMsg();
		string sOutput(cc);
		sOutput = "Logout Error:" + sOutput;
		int errnum = (int)(NET_DVR_GetLastError());
		sOutput = sOutput + to_string(errnum);
		return sOutput;
	}

}
#endif
/*************************************************
函数名:    	OnBnClickedButtonLogin
函数描述:	登录功能按钮
输入参数:
输出参数:
返回值:
**************************************************/
void CSLTMDlg::OnBnClickedButtonLogin()
{
	// TODO:  在此添加控件通知处理程序代码
#if 0
	UpdateData(TRUE);
	CString LogString;
	do
	{
		string sLink = clDevice.Connect();
		string sLink2 = clDevice2.Connect();
		string sLink3 = clDevice3.Connect();
		if (sLink!=""|| sLink2 != "" || sLink3 != "")
		//if(sLink != "")
		{
			//m_StaticLog.Format("1#%s", sLink);
			m_StaticLog.Format("1#%s 2#%s 3#%s", sLink.c_str(), sLink2.c_str(), sLink3.c_str());
			break;
		}
		//lUserID = clDevice.UserID;

		GetP2PParam();

		m_StaticLog.Format("登陆：登陆成功！");
		GetDlgItem(IDC_BUTTON_LOGIN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_LOGOUT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_Get_Hot_Pic)->EnableWindow(TRUE);

	} while (FALSE);
	UpdateData(FALSE);
#endif
	char CameraStall = 'B';
	if (_ChannelControl0.IsRunning())
	{

	}
	else {
		
		_ChannelControl0.ConnectIp((LPSTR)(LPCTSTR)g_csDeviceIP[0], 0);
		_ChannelControl0.SetCameraStall(CameraStall);
	}
	if (_ChannelControl1.IsRunning())
	{

	}
	else {
		
		_ChannelControl1.ConnectIp((LPSTR)(LPCTSTR)g_csDeviceIP[1], 1);
		_ChannelControl1.SetCameraStall(CameraStall);
	}

	if (_ChannelControl2.IsRunning())
	{

	}
	else {
		_ChannelControl2.SetCameraStall(CameraStall);
		_ChannelControl2.ConnectIp((LPSTR)(LPCTSTR)g_csDeviceIP[2], 2);
	}

}


void CSLTMDlg::OnBnClickedButtonLogout()
{
#if 0
	UpdateData(TRUE);
	CString LogString;
	int iItemCount = 0;

	do
	{
		string sLink = clDevice.Disconnect();
		string sLink2 = clDevice2.Disconnect();
		string sLink3 = clDevice3.Disconnect();
		if (sLink != "" || sLink2 != "" || sLink3 != "")
		{
			m_StaticLog.Format("1#%s 2#%s 3#%s", sLink.c_str(), sLink2.c_str(), sLink3.c_str());
			break;
		}
		m_StaticLog.Format("注销：注销成功！");
		GetDlgItem(IDC_BUTTON_LOGIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_LOGOUT)->EnableWindow(FALSE);
		//GetDlgItem(IDC_BUTTON_GET_HOT_PIC)->EnableWindow(FALSE);

	} while (FALSE);
	UpdateData(FALSE);
#endif
	if (_ChannelControl0.IsRunning())
	{
		_ChannelControl0.Disconnect();
	}
	if (_ChannelControl1.IsRunning())
	{
		_ChannelControl1.Disconnect();
	}
	if (_ChannelControl2.IsRunning())
	{
		_ChannelControl2.Disconnect();
	}

	m_staticImage1.Invalidate(TRUE);
	m_staticImage2.Invalidate(TRUE);
	m_staticImage3.Invalidate(TRUE);
	
}


/*************************************************
函数名:    	JpgData2Gui
函数描述:	在图片控件中绘制JPG图像 默认控件变量为m_staticImage
输入参数:	pJpg-图片数据， nJpgLen-图片数据长度
输出参数:
返回值:
**************************************************/
void CSLTMDlg::JpgData2Gui(char* pJpg, int nJpgLen, int Key)
{
	CStatic* DCposition = NULL;
	switch(Key)
	{
		case 1:
			DCposition = &m_staticImage1;
			break;
		case 2:
			DCposition = &m_staticImage2;
			break;
		case 3:
			DCposition = &m_staticImage3;
			break;
	}

	//m_iRawFps++;
	if (this->GetSafeHwnd())
	{
		if (pJpg && nJpgLen)
		{
			HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, nJpgLen);
			void *  pData = GlobalLock(hGlobal);
			memcpy(pData, pJpg, nJpgLen);
			GlobalUnlock(hGlobal);

			IStream *  pStream = NULL;
			if (CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK)
			{
				static CImage image;
				image.Destroy();
				if (SUCCEEDED(image.Load(pStream)))
				{
					//CImageToMat(image, g_afMat);
					//// Use pDC here
					//g_iPointLeftTopX = _ttoi(g_csPointLeftTopX);
					//g_iPointLeftTopY = _ttoi(g_csPointLeftTopY);
					//g_iPointRightBottomX = _ttoi(g_csPointRightBottomX);
					//g_iPointRightBottomY = _ttoi(g_csPointRightBottomY);

					//if (g_iPointLeftTopX >= 0 && g_iPointLeftTopX < g_iPointRightBottomX
					//	&& g_iPointLeftTopY >= 0 && g_iPointLeftTopY < g_iPointRightBottomY
					//	&& g_iPointRightBottomX < PIC_WIDTH && g_iPointRightBottomY < PIC_HEIGHT
					//	&& g_bShowArea)
					//{
					//	CDC* pDC = CDC::FromHandle(image.GetDC());
					//	CPen p(PS_DOT, 0.9, RGB(255, 255, 255));
					//	pDC->SelectStockObject(NULL_BRUSH);
					//	pDC->SelectObject(&p);
					//	pDC->Rectangle(g_iPointLeftTopX, g_iPointLeftTopY, g_iPointRightBottomX, g_iPointRightBottomY);
					//	image.ReleaseDC();

					//	cv::Point poA = cv::Point(g_iPointLeftTopX, g_iPointLeftTopY);
					//	cv::Point poB = cv::Point(g_iPointRightBottomX, g_iPointRightBottomY);
					//	cv::Rect rectArea = cv::Rect(poA, poB);

					//	cv::Mat matArea = g_afMat(rectArea);

					//	cv::Mat TempDataMat(PIC_HEIGHT, PIC_WIDTH, CV_32FC1, &g_afTemp);
					//	cv::Mat matAreatmp = TempDataMat(rectArea);

					//	cv::Scalar meanValue;
					//	meanValue = cv::mean(matAreatmp);

					//	double dminTemp = 10000.0;
					//	double dmaxTemp = -10000.0;

					//	minMaxIdx(matAreatmp, &dminTemp, &dmaxTemp);

					//	CString csValue;
					//	//csValue.Format("区域温度均值为：[%.2f, %.2f, %.2f, %.2f]", meanValue[0], meanValue[1], meanValue[2], meanValue[3]);
					//	csValue.Format("区域温度均值为：%.2f ℃\n区域最高温度：%.2f ℃\n区域最低温度：%.2f ℃\n", meanValue[0], dmaxTemp, dminTemp);
					//	GetDlgItem(IDC_STATIC_AREA)->SetWindowText(csValue);
					//	static CImage ciArea;
					//	MatToCImage(matArea, ciArea);
					//	AreaCImage2Gui(&ciArea);
					//}
					//CImage2Gui(&image);
					RECT rect;
					::GetClientRect(DCposition->GetSafeHwnd(), &rect);

					HDC hdc = ::GetDC(DCposition->GetSafeHwnd());
					::SetStretchBltMode(hdc, HALFTONE); //不掉用会导致图像失真

					image.StretchBlt(hdc, rect);

					::ReleaseDC(DCposition->GetSafeHwnd(), hdc);

				}
				pStream->Release();
			}
			GlobalFree(hGlobal);
		}
	}

}

#ifdef HKWS
/*************************************************
函数名:    	AnalysisHotPicData
函数描述:	对取得的图像温度数据分析函数
输入参数:	buf-温度数据，Height-图像高，Width-图像宽，filePath-文件保存路径（未使用），b_save_file-是否保存（未使用）
输出参数:	p_maxTemp-最高温，p_minTemp最低温
返回值:
**************************************************/
int AnalysisHotPicData(char* buf, int Height, int Width, char* filePath, bool b_save_file, float* p_maxTemp, float* p_minTemp)
{
	//自动取图部分

	/*int m_pointThreshold = 3000;
	int m_spanTime = 60;
	int m_tempThreshold = 100;

	int iIndex = 0;
	char temp[10] = { 0 };
	FILE* fp = NULL;

	double minTemp = 10000.0;
	double maxTemp = -10000.0;
	int pointNumsum = 0;

	/*
	//memcpy(g_afTemp, buf, 384 * 288 * 4);

	//cv::Mat TempDataMat(Height, Width, CV_32FC1, &g_afTemp);
	//cv::minMaxIdx(TempDataMat, &minTemp, &maxTemp);

	//*p_maxTemp = (float)maxTemp;
	//*p_minTemp = (float)minTemp;

	//Mat imgColor = Mat(PIC_HEIGHT, PIC_WIDTH, CV_8UC1);
	//TempDataMat.convertTo(imgColor, CV_8UC1, 500.0 / 255);

	//applyColorMap(imgColor, imgGray, COLORMAP_HOT);


	// 转温度Mat为灰度Mat 用于自动化图片收集

	
	cv::Mat imgGray = cv::Mat(Height, Width, CV_8UC1);
	cv::MatConstIterator_<float> it_in = TempDataMat.begin<float>();
	cv::MatConstIterator_<float> itend_in = TempDataMat.end<float>();
	cv::MatIterator_<uchar> it_out = imgGray.begin<uchar>();
	cv::MatIterator_<uchar> itend_out = imgGray.end<uchar>();
	while (it_in != itend_in)
	{
		float fTemp = (*it_in);
		if (fTemp>m_tempThreshold)
		{
			(*it_out) = 255;
			pointNumsum++;
		}
		else
		{
			(*it_out) = 0;
		}
		it_in++;
		it_out++;
	}
	*/

	/*
	if (pointNumsum > m_pointThreshold)
	{
		int m_x0 = 0, m_y0 = 0, m_sum = 0;
		for (int iWriteHeight = 0; iWriteHeight < Height; ++iWriteHeight)
		{
			for (int iWriteWidth = 0; iWriteWidth < Width; ++iWriteWidth)
			{
				int key = imgGray.at<uchar>(iWriteHeight, iWriteWidth);
				if (key == 255)
				{
					m_x0 += iWriteWidth;
					m_y0 += iWriteHeight;
					m_sum += 1;
				}
			}
		}

		Point m_center;
		m_center.x = m_x0 / m_sum;
		m_center.y = m_y0 / m_sum;
		CString timeCStr;
		CTime tm; tm = CTime::GetCurrentTime();

		CTimeSpan span = tm - tOld1;
		LONG tspan = span.GetTotalSeconds();
		timeCStr.Format("%ld", tm.GetTime());

		cv::String timestr;
		timestr = timeCStr.GetBuffer(0);
		timestr = timestr + ".jpg";
		timestr = "D:\\192.168.0.164\\" + timestr;

		CString m_FileTitle;
		m_FileTitle = timeCStr + ".tmp";
		m_FileTitle = "D:\\192.168.0.164\\" + m_FileTitle;

		if (tspan > m_spanTime  && m_center.x > 185 && m_center.x < 195)
		{
			CFile m_tmptmp;
			imwrite(timestr, imgGray);
			m_tmptmp.Open(m_FileTitle, CFile::modeCreate | CFile::modeWrite);
			m_tmptmp.Write((char*)g_afTemp, sizeof(float)* 384 * 288);
			tOld1 = tm;
			m_tmptmp.Close();
		}

	}
	*/
	return 0;
}

class CameraDev
{
public:
	CameraDev();
	CameraDev::~CameraDev();
	LONG lUserID = -1;
	BOOL bHotPicSignal = FALSE;
	int iDeviceKey = 0;

	NET_DVR_JPEGPICTURE_WITH_APPENDDATA struJpegWithAppendData = { 0 };

	const int ciPictureBufSize = 2 * 1024 * 1024;//2M
	const int ciVisPictureBufSize = 4 * 1024 * 1024;//2M
	char* ucJpegBuf = new char[ciPictureBufSize];
	char* ucAppendDataBuf = new char[ciPictureBufSize];
	char* ucvisJpegBuf = new char[ciVisPictureBufSize];
	int channel = 1;
	float minTemp = 0, maxTemp = 0;

	int ret = 0;
	int failedTime = 0;
	int getHotPic_time = 0;
	void SetUserID(LONG ID, int iPosition);
protected:
private:
};

CameraDev::CameraDev()
{
	memset(ucJpegBuf, 0, ciPictureBufSize);
	memset(ucAppendDataBuf, 0, ciPictureBufSize);
	memset(ucvisJpegBuf, 0, ciVisPictureBufSize);
	struJpegWithAppendData.pJpegPicBuff = ucJpegBuf;
	struJpegWithAppendData.pP2PDataBuff = ucAppendDataBuf;
	struJpegWithAppendData.pVisiblePicBuff = ucvisJpegBuf;
}
CameraDev::~CameraDev()
{
	delete[] ucJpegBuf;
	delete[] ucAppendDataBuf;
	delete[] ucvisJpegBuf;
}
void CameraDev::SetUserID(LONG ID, int iPosition)
{
	this->lUserID = ID;
	this->iDeviceKey = iPosition;
}

UINT ThreadGetHotPicDataMutil(LPVOID lpParam)
{
	CSLTMDlg* pDlg = (CSLTMDlg*)lpParam;
	CameraDev a;
	a.SetUserID(pDlg->clDevice.UserID, 1);
	int i; CString stLogString;
	if (a.lUserID < 0)
	{
		pDlg->SetDlgItemText(IDC_STATIC_LOG, "抓热图：有设备还未登陆！");
	}
	else
	{
		pDlg->clDevice.getHotPic = TRUE;

		for (i = 0; (TRUE == pDlg->clDevice.getHotPic); i++)
		{
			//获取热图的SDK接口
			BOOL ERRGET;

			CString ot;
			clock_t start = clock();
			ERRGET = NET_DVR_CaptureJPEGPicture_WithAppendData(a.lUserID, a.channel, &a.struJpegWithAppendData);
			
			if (TRUE != ERRGET)
			{
				stLogString.Format("抓热图失败！错误信息：%s(%d)", NET_DVR_GetErrorMsg(), NET_DVR_GetLastError());
				pDlg->SetDlgItemText(IDC_STATIC_LOG, stLogString);
				a.failedTime++;
				continue;
			}
			pDlg->JpgData2Gui(a.struJpegWithAppendData.pJpegPicBuff, a.struJpegWithAppendData.dwJpegPicLen, a.iDeviceKey);
			clock_t stop1 = clock();
			long t1 = (stop1 - start);
			ot.Format("1#: %d\n", t1);
			OutputDebugString(ot);
		}
	}
	return 0;
}

UINT ThreadGetHotPicDataMutil2(LPVOID lpParam)
{
	CSLTMDlg* pDlg = (CSLTMDlg*)lpParam;
	CameraDev a;
	a.SetUserID(pDlg->clDevice2.UserID, 2);
	int i; CString stLogString;
	if (a.lUserID < 0)
	{
		pDlg->SetDlgItemText(IDC_STATIC_LOG, "抓热图：有设备还未登陆！");
	}
	else
	{
		pDlg->clDevice2.getHotPic = TRUE;

		for (i = 0; (TRUE == pDlg->clDevice2.getHotPic); i++)
		{
			//获取热图的SDK接口
			BOOL ERRGET;

			CString ot;
			clock_t start = clock();
			ERRGET = NET_DVR_CaptureJPEGPicture_WithAppendData(a.lUserID, a.channel, &a.struJpegWithAppendData);

			if (TRUE != ERRGET)
			{
				stLogString.Format("抓热图失败！错误信息：%s(%d)", NET_DVR_GetErrorMsg(), NET_DVR_GetLastError());
				pDlg->SetDlgItemText(IDC_STATIC_LOG, stLogString);
				a.failedTime++;
				continue;
			}
			pDlg->JpgData2Gui(a.struJpegWithAppendData.pJpegPicBuff, a.struJpegWithAppendData.dwJpegPicLen, a.iDeviceKey);
			clock_t stop1 = clock();
			long t1 = (stop1 - start);
			ot.Format("2#: %d\n", t1);
			OutputDebugString(ot);
		}
	}
	return 0;
}

UINT ThreadGetHotPicDataMutil3(LPVOID lpParam)
{
	CSLTMDlg* pDlg = (CSLTMDlg*)lpParam;
	CameraDev a;
	a.SetUserID(pDlg->clDevice3.UserID, 3);
	int i; CString stLogString;
	if (a.lUserID < 0)
	{
		pDlg->SetDlgItemText(IDC_STATIC_LOG, "抓热图：有设备还未登陆！");
	}
	else
	{
		pDlg->clDevice3.getHotPic = TRUE;

		for (i = 0; (TRUE == pDlg->clDevice3.getHotPic); i++)
		{
			//获取热图的SDK接口
			BOOL ERRGET;

			CString ot;
			clock_t start = clock();
			ERRGET = NET_DVR_CaptureJPEGPicture_WithAppendData(a.lUserID, a.channel, &a.struJpegWithAppendData);
			clock_t stop1 = clock();
			if (TRUE != ERRGET)
			{
				stLogString.Format("抓热图失败！错误信息：%s(%d)", NET_DVR_GetErrorMsg(), NET_DVR_GetLastError());
				pDlg->SetDlgItemText(IDC_STATIC_LOG, stLogString);
				a.failedTime++;
				continue;
			}
			//pDlg->JpgData2Gui(a.struJpegWithAppendData.pJpegPicBuff, a.struJpegWithAppendData.dwJpegPicLen, a.iDeviceKey);
			
			long t1 = (stop1 - start);
			ot.Format("3#: %d\n", t1);
			OutputDebugString(ot);
		}
	}
	return 0;
}

UINT ThreadGetHotPic1(LPVOID lpParam)
{
	CSLTMDlg* pDlg = (CSLTMDlg*)lpParam;
	CameraDev a;
	CameraDev b;
	CameraDev c;
	a.SetUserID(pDlg->clDevice.UserID, 1);
	b.SetUserID(pDlg->clDevice2.UserID, 2);
	c.SetUserID(pDlg->clDevice3.UserID, 3);
	int i; CString stLogString;
	if (a.lUserID < 0 || b.lUserID < 0 || c.lUserID < 0)
	{
		pDlg->SetDlgItemText(IDC_STATIC_LOG, "抓热图：有设备还未登陆！");
	}
	else
	{
		pDlg->b_getHotPic = TRUE;
		
		for (i = 0; (TRUE == pDlg->b_getHotPic); i++)
		{
			//获取热图的SDK接口

			BOOL ERRGET1, ERRGET2, ERRGET3;

			CString ot;
			clock_t start = clock();
			ERRGET1 = NET_DVR_CaptureJPEGPicture_WithAppendData(a.lUserID, a.channel, &a.struJpegWithAppendData);
			clock_t stop1 = clock();
			long t1 = (stop1 - start);

			ERRGET2 = NET_DVR_CaptureJPEGPicture_WithAppendData(b.lUserID, b.channel, &b.struJpegWithAppendData);
			clock_t stop2 = clock();
			long t2 = (stop2 - stop1);

			ERRGET3 = NET_DVR_CaptureJPEGPicture_WithAppendData(c.lUserID, c.channel, &c.struJpegWithAppendData);

			clock_t stop3 = clock();
			long t3 = (stop3 - stop2);



			if (TRUE != ERRGET1 || TRUE != ERRGET2 || TRUE != ERRGET3)
			{
				stLogString.Format("抓热图失败！错误信息：%s(%d)", NET_DVR_GetErrorMsg(), NET_DVR_GetLastError());
				pDlg->SetDlgItemText(IDC_STATIC_LOG, stLogString);
				a.failedTime++;
				continue;
			}

			pDlg->JpgData2Gui(a.struJpegWithAppendData.pJpegPicBuff, a.struJpegWithAppendData.dwJpegPicLen, a.iDeviceKey);
			pDlg->JpgData2Gui(b.struJpegWithAppendData.pJpegPicBuff, b.struJpegWithAppendData.dwJpegPicLen, b.iDeviceKey);
			pDlg->JpgData2Gui(c.struJpegWithAppendData.pJpegPicBuff, c.struJpegWithAppendData.dwJpegPicLen, c.iDeviceKey);

			clock_t end = clock();
			long t4 = (end - start);
			ot.Format("%d %d %d %d\n", t1, t2, t3, t4);
			OutputDebugString(ot);
			//判断抓图数据是否正确
			//if (a.struJpegWithAppendData.dwP2PDataLen != 4 * a.struJpegWithAppendData.dwJpegPicWidth * a.struJpegWithAppendData.dwJpegPicHeight)
			//{
			//	stLogString.Format("抓拍：返回的数据长度有误！P2PDataLen[%d]!=4*JpegPicWidth[%d]*JpegPicHeight[%d]",
			//		a.struJpegWithAppendData.dwP2PDataLen, a.struJpegWithAppendData.dwJpegPicWidth, a.struJpegWithAppendData.dwJpegPicHeight);
			//	pDlg->SetDlgItemText(IDC_STATIC_LOG, stLogString);
			//	a.failedTime++;
			//	continue;
			//}

			//a.minTemp = 10000.0;
			//a.maxTemp = -10000.0;
			//ret = AnalysisHotPicData(struJpegWithAppendData.pP2PDataBuff, struJpegWithAppendData.dwJpegPicHeight, struJpegWithAppendData.dwJpegPicWidth,
				//NULL, 0, &maxTemp, &minTemp);

			/*pDlg->JpgData2Gui(a.struJpegWithAppendData.pJpegPicBuff, a.struJpegWithAppendData.dwJpegPicLen, a.iDeviceKey);*/
		}
	}

	// 清理控件上的画面
	((CStatic*)pDlg->GetDlgItem(IDC_CAMERA1))->Invalidate(TRUE);
	((CStatic*)pDlg->GetDlgItem(IDC_CAMERA2))->Invalidate(TRUE);
	((CStatic*)pDlg->GetDlgItem(IDC_CAMERA3))->Invalidate(TRUE);
	pDlg->b_getHotPic = FALSE;
	return 1;
}


/****************************************************
函数名:   ThreadGetHotPicData
函数描述:  读图线程
输入参数:  无
输出参数:  无
返回值:
*****************************************************/
UINT ThreadGetHotPicData(LPVOID lpParam)
{
	CString stLogString;
	int ret = 0;
	CString csLogString;
	int failedTime = 0;
	int i = 0;

	CSLTMDlg* pDlg = (CSLTMDlg*)lpParam;
	int getHotPic_time = 0;

	if (pDlg->clDevice.UserID < 0)
	{
		pDlg->SetDlgItemText(IDC_STATIC_LOG, "抓热图：还未登陆！");
	}
	else
	{
		//将按钮置为停止状态
		pDlg->b_getHotPic = TRUE;
		pDlg->SetDlgItemTextA(IDC_BUTTON_Get_Hot_Pic, _T("停止画面"));

		NET_DVR_JPEGPICTURE_WITH_APPENDDATA struJpegWithAppendData = { 0 };

		const int ciPictureBufSize = 2 * 1024 * 1024;//2M
		const int ciVisPictureBufSize = 4 * 1024 * 1024;//2M
		char* ucJpegBuf = new char[ciPictureBufSize];
		char* ucAppendDataBuf = new char[ciPictureBufSize];
		char* ucvisJpegBuf = new char[ciVisPictureBufSize];

		memset(ucJpegBuf, 0, ciPictureBufSize);
		memset(ucAppendDataBuf, 0, ciPictureBufSize);
		memset(ucvisJpegBuf, 0, ciVisPictureBufSize);

		struJpegWithAppendData.pJpegPicBuff = ucJpegBuf;
		struJpegWithAppendData.pP2PDataBuff = ucAppendDataBuf;
		struJpegWithAppendData.pVisiblePicBuff = ucvisJpegBuf;

		float minTemp = 0, maxTemp = 0;
		int channel = 1;

		for (i = 0; ((i < getHotPic_time) || (0 == getHotPic_time)) && (TRUE == pDlg->b_getHotPic); i++)
		{
			//获取热图的SDK接口
			if (TRUE != NET_DVR_CaptureJPEGPicture_WithAppendData(pDlg->clDevice.UserID, channel, &struJpegWithAppendData))
			{
				stLogString.Format("抓热图失败！错误信息：%s(%d)", NET_DVR_GetErrorMsg(), NET_DVR_GetLastError());
				pDlg->SetDlgItemText(IDC_STATIC_LOG, stLogString);
				csLogString.Format("Get the %dth hot pic failed! %s(%d)", i + 1, NET_DVR_GetErrorMsg(), NET_DVR_GetLastError());
				failedTime++;
				continue;
			}

			//判断抓图数据是否正确
			if (struJpegWithAppendData.dwP2PDataLen != 4 * struJpegWithAppendData.dwJpegPicWidth * struJpegWithAppendData.dwJpegPicHeight)
			{
				stLogString.Format("抓拍：返回的数据长度有误！P2PDataLen[%d]!=4*JpegPicWidth[%d]*JpegPicHeight[%d]",
					struJpegWithAppendData.dwP2PDataLen, struJpegWithAppendData.dwJpegPicWidth, struJpegWithAppendData.dwJpegPicHeight);
				pDlg->SetDlgItemText(IDC_STATIC_LOG, stLogString);
				csLogString.Format("Get %dth hot pic length error!P2PDataLen[%d]!=4*JpegPicWidth[%d]*JpegPicHeight[%d]", i + 1,
					struJpegWithAppendData.dwP2PDataLen, struJpegWithAppendData.dwJpegPicWidth, struJpegWithAppendData.dwJpegPicHeight);
				failedTime++;
				continue;
			}

			minTemp = 10000.0;
			maxTemp = -10000.0;
			//ret = AnalysisHotPicData(struJpegWithAppendData.pP2PDataBuff, struJpegWithAppendData.dwJpegPicHeight, struJpegWithAppendData.dwJpegPicWidth,
				//NULL, 0, &maxTemp, &minTemp);

			pDlg->JpgData2Gui(struJpegWithAppendData.pJpegPicBuff, struJpegWithAppendData.dwJpegPicLen, 1);
		}

		delete[] ucJpegBuf;
		delete[] ucAppendDataBuf;
		delete[] ucvisJpegBuf;
	}

	// 清理控件上的画面
	((CStatic*)pDlg->GetDlgItem(IDC_CAMERA1))->Invalidate(TRUE);
	//按钮置为获取状态
	pDlg->b_getHotPic = FALSE;
	pDlg->SetDlgItemTextA(IDC_BUTTON_Get_Hot_Pic, _T(" 获取图像"));

	return 1;
}


void CSLTMDlg::OnBnClickedButtonGetHotPic()
{
	UpdateData(TRUE);
	if (b_getHotPic == FALSE)
	//if (FALSE == clDevice.getHotPic)
	{
		//开始获取热图
		//AfxBeginThread(ThreadGetHotPicData, this);
		AfxBeginThread(ThreadGetHotPic1, this);
		//AfxBeginThread(ThreadGetHotPicDataMutil, this);
		//AfxBeginThread(ThreadGetHotPicDataMutil2, this);
		//AfxBeginThread(ThreadGetHotPicDataMutil3, this);

		Sleep(300);

		if (b_getHotPic == TRUE)
		//if (clDevice.getHotPic == TRUE)
		{
			this->SetDlgItemTextA(IDC_BUTTON_Get_Hot_Pic, _T("停止画面"));
			//GetDlgItem(IDC_BUTTON_CAP)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_LOGOUT)->EnableWindow(FALSE);
		}

		NET_DVR_CLIENTINFO ClientInfo;
		ClientInfo.lChannel = 1; //Channel number 设备通道号

		ClientInfo.lLinkMode = 0;    //Main Stream
		ClientInfo.sMultiCastIP = NULL;

		BOOL bPreviewBlock = false;       //请求码流过程是否阻塞，0：否，1：是

		//预览取流 
		//llRealHandle = NET_DVR_RealPlay_V30(lUserID, &ClientInfo, g_RealDataCallBack_V30, this, TRUE);

		//ClientInfo.hPlayWnd = GetDlgItem(IDC_CAMERA1)->GetSafeHwnd();  //窗口为空，设备SDK不解码只取流
		//llRealHandle = NET_DVR_RealPlay_V30(clDevice.UserID, &ClientInfo, NULL, this, bPreviewBlock);

		//ClientInfo.hPlayWnd = GetDlgItem(IDC_CAMERA2)->GetSafeHwnd();
		//llRealHandle2 = NET_DVR_RealPlay_V30(clDevice2.UserID, &ClientInfo, NULL, this, bPreviewBlock);

		//ClientInfo.hPlayWnd = GetDlgItem(IDC_CAMERA3)->GetSafeHwnd();
		//llRealHandle3 = NET_DVR_RealPlay_V30(clDevice3.UserID, &ClientInfo, NULL, this, bPreviewBlock);

	}
	else
	{	
		//按钮置为获取状态
		/*clDevice.getHotPic = FALSE;
		clDevice2.getHotPic = FALSE;
		clDevice3.getHotPic = FALSE;*/
		b_getHotPic = FALSE;

		this->SetDlgItemTextA(IDC_BUTTON_Get_Hot_Pic, _T(" 获取图像"));
		//GetDlgItem(IDC_BUTTON_CAP)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_LOGOUT)->EnableWindow(TRUE);

		//NET_DVR_StopRealPlay(llRealHandle);
		//NET_DVR_StopRealPlay(llRealHandle2);
		//NET_DVR_StopRealPlay(llRealHandle3);
		//llRealHandle = -1;
		//llRealHandle3 = -1;
		//llRealHandle2 = -1;
	}

	UpdateData(FALSE);
}

#else
void CSLTMDlg::OnBnClickedButtonGetHotPic()
{

}
#endif

void CSLTMDlg::OnBnClickedButtonQuit()
{
	http_server_stop(&server);
	CDialogEx::OnOK();
}


void CSLTMDlg::OnClose()
{
	
	CDialogEx::OnClose();
}


void CSLTMDlg::BmpData2Gui1(unsigned char* pBits, int width, int height)
{
	//fps[1] ++;
	if (this->GetSafeHwnd())
	{
		g_iFPS[1] ++;
		BITMAPINFO       dibInfo;
		// 组装位图信息头  
		dibInfo.bmiHeader.biSize = sizeof(BITMAPINFO);
		dibInfo.bmiHeader.biWidth = width;
		dibInfo.bmiHeader.biHeight = -height;
		dibInfo.bmiHeader.biPlanes = 1;
		dibInfo.bmiHeader.biBitCount = 24;
		dibInfo.bmiHeader.biCompression = 0;
		dibInfo.bmiHeader.biSizeImage = width * height * 3;
		dibInfo.bmiHeader.biXPelsPerMeter = 0x0ec4;
		dibInfo.bmiHeader.biYPelsPerMeter = 0x0ec4;
		dibInfo.bmiHeader.biClrUsed = 0;
		dibInfo.bmiHeader.biClrImportant = 0;

		CClientDC dc(&m_staticImage2);

		SetStretchBltMode(dc.GetSafeHdc(), STRETCH_HALFTONE);

		RECT rect;
		m_staticImage2.GetClientRect(&rect);

		int nResult = StretchDIBits(dc.GetSafeHdc(), 0, 0, rect.right - rect.left, rect.bottom - rect.top, 0, 0, width, height, pBits, &dibInfo, DIB_RGB_COLORS, SRCCOPY);


	}
}

void CSLTMDlg::BmpData2Gui2(unsigned char* pBits, int width, int height)
{
	//fps[2] ++;
	if (this->GetSafeHwnd())
	{
		g_iFPS[2] ++;
		BITMAPINFO       dibInfo;
		// 组装位图信息头  
		dibInfo.bmiHeader.biSize = sizeof(BITMAPINFO);
		dibInfo.bmiHeader.biWidth = width;
		dibInfo.bmiHeader.biHeight = -height;
		dibInfo.bmiHeader.biPlanes = 1;
		dibInfo.bmiHeader.biBitCount = 24;
		dibInfo.bmiHeader.biCompression = 0;
		dibInfo.bmiHeader.biSizeImage = width * height * 3;
		dibInfo.bmiHeader.biXPelsPerMeter = 0x0ec4;
		dibInfo.bmiHeader.biYPelsPerMeter = 0x0ec4;
		dibInfo.bmiHeader.biClrUsed = 0;
		dibInfo.bmiHeader.biClrImportant = 0;

		CClientDC dc(&m_staticImage3);

		SetStretchBltMode(dc.GetSafeHdc(), STRETCH_HALFTONE);

		RECT rect;
		m_staticImage3.GetClientRect(&rect);

		int nResult = StretchDIBits(dc.GetSafeHdc(), 0, 0, rect.right - rect.left, rect.bottom - rect.top, 0, 0, width, height, pBits, &dibInfo, DIB_RGB_COLORS, SRCCOPY);


	}
}

void CSLTMDlg::BmpData2Gui0(unsigned char* pBits, int width, int height)
{
	if (this->GetSafeHwnd())
	{
		g_iFPS[0] ++;
		BITMAPINFO       dibInfo;
		// 组装位图信息头  
		dibInfo.bmiHeader.biSize = sizeof(BITMAPINFO);
		dibInfo.bmiHeader.biWidth = width;
		dibInfo.bmiHeader.biHeight = -height;
		dibInfo.bmiHeader.biPlanes = 1;
		dibInfo.bmiHeader.biBitCount = 24;
		dibInfo.bmiHeader.biCompression = 0;
		dibInfo.bmiHeader.biSizeImage = width * height * 3;
		dibInfo.bmiHeader.biXPelsPerMeter = 0x0ec4;
		dibInfo.bmiHeader.biYPelsPerMeter = 0x0ec4;
		dibInfo.bmiHeader.biClrUsed = 0;
		dibInfo.bmiHeader.biClrImportant = 0;

		CClientDC dc(&m_staticImage1);

		SetStretchBltMode(dc.GetSafeHdc(), STRETCH_HALFTONE);

		RECT rect;
		m_staticImage1.GetClientRect(&rect);

		int nResult = StretchDIBits(dc.GetSafeHdc(), 0, 0, rect.right - rect.left, rect.bottom - rect.top, 0, 0, width, height, pBits, &dibInfo, DIB_RGB_COLORS, SRCCOPY);


	}
}


string writeJson(CString baonumber, CString baomaxtemp, CString baoarea, CString baotime)
{
	hv::Json root;
	string out;

	root["msg"]["baonumber"] = (LPSTR)(LPCTSTR)baonumber;
	//root["msg"]["baomaxtemp"] = baomaxtemp.GetBuffer(0);
	//root["msg"]["baoarea"] = baoarea.GetBuffer(0);
	root["msg"]["baotime"] = (LPSTR)(LPCTSTR)baotime;

	out = root.dump();
	return out;
}


void CSLTMDlg::DataTransfer(unsigned char* pBGR, int bgrLen, float* tempMatrix, int width, int height, int dev)
{
	if (width == PIC_WIDTH && height == PIC_HEIGHT)
	{
		if (dev == 0)
		{
			memcpy(g_fTempData0, tempMatrix, width * height * sizeof(float));
		}
		else if(dev == 1)
		{
			memcpy(g_fTempData1, tempMatrix, width * height * sizeof(float));
		}
		else if (dev == 2)
		{
			memcpy(g_fTempData2, tempMatrix, width * height * sizeof(float));
		}
	}
}

void SaveSingleTempDataToFile(float *pData, char *pName, int dev)
{
	int len_float = sizeof(float);
	FILE *conti_buffer;//写连续保存的文件的句柄
	char *path_save;//连续保存数据的文件路径
	CString sFileSaveName;
	sFileSaveName.Format("D:\\savetemp\\%d\\%s.tmp", dev, pName);
	path_save = (LPSTR)(LPCTSTR)sFileSaveName;
	conti_buffer = fopen(path_save, "w+b");
	if (conti_buffer == NULL)
	{
		return;
	}
	fseek(conti_buffer, 0, SEEK_SET);   //将文件指针指向文件头/
	fwrite(pData, len_float, PIC_WIDTH*PIC_HEIGHT, conti_buffer);
	fclose(conti_buffer);
}
std::string writeJson(string baoNumber, string baoArea, string baoTime)
{
	hv::Json root;
	std::string out;

	root["msg"]["baoNumber"] = baoNumber;
	root["msg"]["baoArea"] = baoArea;
	root["msg"]["baoTime"] = baoTime;

	out = root.dump();
	return out;
}

UINT SendJsonMessage(LPVOID pParam)
{
	string jsonStr = writeJson("11", "1", "2022-07-27 11:38:15");
	auto resp = requests::post("127.0.0.1:8000/assets/report/", jsonStr);
	return 0;
}

UINT HTTPServerProc(LPVOID pParam)
{
	CSLTMDlg* pDlg = (CSLTMDlg*)pParam;

	router.GET("/start", [pDlg](HttpRequest* req, HttpResponse* resp) {
		pDlg->SetInfraredData("收到开启画面指令");
		pDlg->OnBnClickedButtonLogin();
		return resp->String("开启画面");
	});

	router.GET("/stop", [pDlg](HttpRequest* req, HttpResponse* resp) {
		pDlg->SetInfraredData("收到关闭画面指令");
		pDlg->OnBnClickedButtonLogout();
		return resp->String("关闭画面");
	});

	router.GET("/heartbeat", [pDlg](HttpRequest* req, HttpResponse* resp) {
		CTime tNow = CTime::GetCurrentTime();

		__int64 iNow = tNow.GetTime();
		int a = iNow % 10000;
		return resp->String(to_string(a).c_str());
	});

	router.GET("/fps", [pDlg](HttpRequest* req, HttpResponse* resp) {
		string sfps1 = to_string(g_iFPS[0]);
		string sfps2 = to_string(g_iFPS[1]);
		string sfps3 = to_string(g_iFPS[2]);
		string sfpssend = sfps1 + " " + sfps2 + " " + sfps3;
		return resp->String(sfpssend.c_str());
	});

	server.port = 8765;
	server.service = &router;
	http_server_run(&server);
	
	return 0;
}


void CSLTMDlg::StartHTTPServer()
{
	AfxBeginThread(HTTPServerProc, this);
	return;
}

void CSLTMDlg::SetInfraredData(char *str)
{
	CString strOutput(str);
	GetDlgItem(IDC_STATIC_LOG)->SetWindowText(strOutput);
}


// Threshold-阈值 Centroid-质心
#define TempThreshold 110.0 //温度阈值
#define PointThreshold 10000 //高温点阈值

static inline bool ContoursSortFun(vector<cv::Point> contour1, vector<cv::Point> contour2)
{
	return (cv::contourArea(contour1) > cv::contourArea(contour2));
}
/************************************************************************/
/* 摄像头图像质心判断模块                                                 */
/************************************************************************/
bool CSLTMDlg::GetCentroid(float* tempMatrix, Point &pCentroid, Point &pMaxTempPoint)
{
	Mat tempDataMat(PIC_HEIGHT, PIC_WIDTH, CV_32FC1, tempMatrix);
	Point centroid(-1, -1);			  // 质心初始化为(-1, -1)
	
	// 通过OpenCV自带函数求取最高温最低温位置与温度值
	double minVal, maxVal;
	int minIdx[2] = {}, maxIdx[2] = {};
	minMaxIdx(tempDataMat, &minVal, &maxVal, minIdx, maxIdx);
	
	Point point(maxIdx[1], maxIdx[0]);// 默认最高温位置

	// 获取二值化图与统计高温点
	Mat imgBinarization(PIC_HEIGHT, PIC_WIDTH, CV_8UC1);
	int pointsum = 0;		// 高温点数目
	int Xsum = 0, Ysum = 0; // XY坐标值总计
	for (int i = 0; i < PIC_HEIGHT; i++)
	{
		for (int j = 0; j < PIC_WIDTH; j++)
		{
			if (tempDataMat.at<float>(i, j) > TempThreshold)
			{
				pointsum++;
				imgBinarization.at<uchar>(i, j) = 255;
				Xsum += j;
				Ysum += i;
			}
			else
			{
				imgBinarization.at<uchar>(i, j) = 0;
			}
		}
	}

	// 阈值判断 小于点数阈值不再继续
	if (pointsum > PointThreshold)
	{
		Mat dst(PIC_HEIGHT, PIC_WIDTH, CV_8UC1);

		//对图像进行开操作，排除干扰点
		Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
		morphologyEx(imgBinarization, dst, 2, kernel);

		/* 新方法，使用OpenCV findContours查找轮廓，排除铁包顶部敞开部分            */
		vector<vector<cv::Point>> contours;
		vector<Vec4i> hierarchy;

		/* CV_RETR_EXTERNAL只检测最外围轮廓                                     */
		/* CV_CHAIN_APPROX_SIMPLE仅保存轮廓的拐点信息                           */

		findContours(dst, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		Mat imageContours = Mat::zeros(dst.size(), CV_8UC1);
		Mat Contours = Mat::zeros(dst.size(), CV_8UC1);

		sort(contours.begin(), contours.end(), ContoursSortFun); //轮廓按面积由大至小排序

		double lengthWidthRatio = 0.0; // 长宽比
		Point pMaxPointPosition(0, 0); // 最高温位置

		Rect rects1 = boundingRect(contours[0]);

		// 长宽比
		lengthWidthRatio = (rects1.height*1.0) / rects1.width;

		// 排除铁包上部0.3区域求温度值（可能有裸露的铁包包口部分）	
		Point startpoint = rects1.tl();

		startpoint.x = startpoint.x + (int)(0.3 * rects1.width);
		startpoint.y = startpoint.y + (int)(0.3 * rects1.height);

		Mat TempDataMatREP(tempDataMat, Rect(startpoint, rects1.br()));

		double REPminVal, REPmaxVal;
		int   REPminIdx[2] = {}, REPmaxIdx[2] = {};	// 修正后的minnimum Index, maximum Index
		minMaxIdx(TempDataMatREP, &REPminVal, &REPmaxVal, REPminIdx, REPmaxIdx);

		pMaxPointPosition.x = startpoint.x + REPmaxIdx[1];
		pMaxPointPosition.y = startpoint.y + REPmaxIdx[0];

		maxVal = REPmaxVal;

		point = pMaxPointPosition; //将高温点修正到铁包上
		centroid.x = (Xsum / pointsum);
		centroid.y = (Ysum / pointsum);

		pCentroid = centroid;
		pMaxTempPoint = point;
		return true;
	}
	else
	{
		pCentroid = centroid;
		pMaxTempPoint = point;
		return false;
	}
}


//{
//	double quyuArea = 0.0;
//	quyuArea = contourArea(contours[0]);
//
//	CString timeCStr;
//	CTime tm; tm = CTime::GetCurrentTime();
//	CTimeSpan span = tm - tOld1;
//	LONG tspan = span.GetTotalSeconds();
//
//	//图像存储条件
//	if (centroid.y > 140 && centroid.y < 150 && tspan > 120 && quyuArea > 10000)
//	{
//		tOld1 = tm;
//		timeCStr.Format("%ld", tm.GetTime());
//		//SaveSingleTemp(tempMatrix, timeCStr, 1);
//
//		String timestr;
//		timestr = timeCStr.GetBuffer(0);
//		timestr = timestr + ".jpg";
//		timestr = "D:\\savetemp\\1\\" + timestr;
//		//imwrite(timestr, imgGray);
//
//		if (pDlg)
//		{
//			//pDlg->CheackFileMove(tm.GetTime());
//		}
//	}
//}

HBRUSH CSLTMDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_TITLE)
	{
		CFont m_font;
		m_font.CreatePointFont(150, "MICROSOFT YAHEI");//代表15号字体，华文行楷
		pDC->SetTextColor(RGB(0, 0, 250));
		pDC->SelectObject(&m_font);
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CSLTMDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{
		CString csFPS;
		csFPS.Format("%d %d %d", g_iFPS[0], g_iFPS[1], g_iFPS[2]);

		GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(csFPS);
		memset(g_iFPS, 0, sizeof(g_iFPS));
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CSLTMDlg::OnBnClickedButtonTestdata()
{
	// TODO: 在此添加控件通知处理程序代码
	AfxBeginThread(SendJsonMessage, this);
}
