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

#include <map>
#include <fstream>
#include "hv/HttpServer.h"
#include "hv/requests.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SETTING_FILE  _T("C:\\SLTM\\SLTMsetting.ini")

//#define LOCAL_TEST 1

#ifdef LOCAL_TEST
#define WRITE_FILE_PATH _T("\\\\127.0.0.1\\Share\\savetemp\\")
#define END_FILE_PATH _T("\\\\127.0.0.1\\Share\\data\\")
#define CAPTURE_FILE_PATH _T("\\\\127.0.0.1\\Share\\capture\\");
#else
#define WRITE_FILE_PATH _T("\\\\192.168.1.200\\Share\\savetemp\\")
#define END_FILE_PATH _T("\\\\192.168.1.200\\Share\\data\\")
#define CAPTURE_FILE_PATH _T("\\\\192.168.1.200\\Share\\capture\\");

#endif
// CSLTMDlg 对话框

// 全局温度数据与图片数据
float g_fTempData[3][PIC_HEIGHT*PIC_WIDTH];
Mat g_mPicData[3];
CTime ctInitTime[3];

// 文件保存用
CString g_Baohao;
int g_BaohaoList[11];
LONG g_BoahaoTimeList[11];
int g_BaohaoKey;
//新文件名称，判断文件是否需要删除用
CString g_NowFileFath;
Mat fakeimage = imread("C:\\1.jpg");

std::map<CString, CString>g_mapPositionName = {
    {"1", "1号LF精炼位"}, {"2","1号炉钢包"}, {"3","2号炉钢包"}, {"4","1号RH精炼位"},
    {"5", "2号LF精炼位"}, {"6","3号炉钢包"}, {"7","4号炉钢包"}, {"8","2号RH精炼位"},
    {"9", "1步铁包"}, {"10","2步铁包"}
};
struct Threshold
{
    int iThreshold[3];
};

struct JudgeBOOL
{
    bool iJudge[3][5];
};

//Threshold devThreshold[10] = {
//	{5000,15000,3000},{10000,10000,10000},{10000,8000,3000},{12000,10000,10000},{10000,10000,3000},
//	{4000,8000,3000},{4000,8000,3000},{10000,10000,10000},{4000,8000,3000},{4000,8000,3000},
//};

Threshold devThreshold[10] = {
    {5000,5000,3000},{5000,5000,3000},{5000,5000,3000},{5000,5000,3000},{5000,5000,3000},
    {3000,3000,3000},{3000,5000,3000},{5000,5000,3000},{3000,3000,3000},{3000,3000,3000},
};


/*
&& (fMiddleMeanValue > 200 || bJudgeCon[0])
&& (fTopValue > 100 || bJudgeCon[1])
&& (fBottomMeanValue > 100 || bJudgeCon[2])
&& (fRightMeanValue < 20 || bJudgeCon[3])
&& (fLeftMeanValue < 20 || bJudgeCon[4]))
*/
//JudgeBOOL devJudgeBool[10] = {
//	{0,0,1,1,1,0,0,1,1,1,0,1,1,1,1}, // pos1 LF1
//	{0,0,0,1,1,0,0,0,1,1,0,0,0,1,1}, // pos2 钢包1
//	{0,0,0,0,1,0,0,1,1,1,0,0,1,1,1}, // pos3 钢包2
//	{0,0,0,0,0,0,0,0,1,1,0,0,1,1,1}, // pos4 RH1
//	{0,0,0,1,1,0,0,0,0,1,0,0,1,1,1}, // pos5 LF2
//	{0,0,0,1,1,0,0,0,1,1,0,0,1,1,1}, // pos6 钢包3
//	{0,0,0,1,1,0,0,0,1,1,0,0,1,1,1}, // pos7 钢包4
//	{0,0,0,0,0,0,0,0,0,0,0,0,1,1,1}, // pos8 RH2
//	{0,0,1,1,1,0,0,0,1,0,0,0,1,1,1}, // pos9 铁包1
//	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}  // pos10 铁包2
//};

JudgeBOOL devJudgeBool[10] = {
   {0,1,1,1,1,0,1,1,1,1,0,1,1,1,1}, // pos1 LF1
   {0,1,1,1,1,0,1,1,1,1,0,1,1,1,1}, // pos2 钢包1
   {0,1,1,1,1,0,1,1,1,1,0,1,1,1,1}, // pos3 钢包2
   {0,1,1,1,1,0,1,1,1,1,0,1,1,1,1}, // pos4 RH1
   {0,1,1,1,1,0,1,1,1,1,0,1,1,1,1}, // pos5 LF2
   {0,1,1,1,1,0,1,1,1,1,0,1,1,1,1}, // pos6 钢包3
   {0,1,1,1,1,0,1,1,1,1,0,1,1,1,1}, // pos7 钢包4
   {0,1,1,1,1,0,1,1,1,1,0,1,1,1,1}, // pos8 RH2
   {0,1,1,1,1,0,1,1,1,1,0,1,1,1,1}, // pos9 铁包1
   {0,1,1,1,1,0,1,1,1,1,0,1,1,1,1}, // pos10 铁包2
};


std::map<CString, Threshold>g_mapPointThreshold = {
    {"1", devThreshold[0]},{"2", devThreshold[1]},{"3", devThreshold[2]},{"4", devThreshold[3]},{"5", devThreshold[4]},
    {"6", devThreshold[5]},{"7", devThreshold[6]},{"8", devThreshold[7]},{"9", devThreshold[8]},{"10", devThreshold[9]}
};

std::map<CString, JudgeBOOL>g_mapSaveJudge = {
    {"1", devJudgeBool[0]},{"2", devJudgeBool[1]},{"3", devJudgeBool[2]},{"4", devJudgeBool[3]},{"5", devJudgeBool[4]},
    {"6", devJudgeBool[5]},{"7", devJudgeBool[6]},{"8", devJudgeBool[7]},{"9", devJudgeBool[8]},{"10", devJudgeBool[9]}
};


CString g_devPosition;

int g_iFPS[3] = { 0, 0, 0 };
int g_showFPS[3] = { 0, 0, 0 };

int g_temp_frame_fps[3] = { 0, 0, 0 };

__time64_t g_tGetFileTime[3] = { 0, 0, 0 };

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
    //	ON_WM_ERASEBKGND()
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
    // 背景画刷
    m_brushBack.CreateSolidBrush(RGB(9, 2, 88));

    // 读取IP与初始化海康威视SDK
    ReadPara();
    GetDlgItem(IDC_STATIC_TITLE)->SetWindowText(g_devTitleName);

    StartHTTPServer();
    SetTimer(nIDEventUpdateFPS, 1000, NULL);


    for (int i = 0; i < 3; i++)
    {
        g_mPicData[i] = Mat(PIC_HEIGHT, PIC_WIDTH, CV_8UC3, Scalar(0, 0, 0));
        ctInitTime[i] = CTime(2021, 10, 10, 10, 10, 10);
    }

#if HKWS
    NET_DVR_Init();
    clDevice[0].setIP("192.168.1.31");
    clDevice[1].setIP("192.168.1.32");
    clDevice[2].setIP("192.168.1.33");
    g_devPosition = "7";
    g_devTitleName = g_mapPositionName[g_devPosition];
    GetDlgItem(IDC_STATIC_TITLE)->SetWindowText(g_devTitleName);
#endif

    SetTimer(nIDEventUpdateAlert, 1000, NULL);
    GetDlgItem(IDC_STATIC_THROUGH)->ShowWindow(FALSE);
    GetDlgItem(IDC_STATIC_TEMPALERT)->ShowWindow(FALSE);
    GetDlgItem(IDC_STATIC_LOG)->ShowWindow(TRUE);

#ifdef LOCAL_TEST
#else
    OnBnClickedButtonLogin();
#endif

    GetTimeList();
    SetTimer(nIDEventGetTimeList, 1000, 0);

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
// 来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
// 这将由框架自动完成。

void CSLTMDlg::OnPaint()
{

    if (IsIconic()) // 如果最小化就画图标
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

        CPaintDC dc(this);
        CRect rect;
        GetClientRect(&rect);
        // 设置背景
        dc.FillSolidRect(rect, RGB(9, 2, 88));

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

    CString devTitle = CString("1");
    //WritePrivateProfileString(Section名,Key名,CString数据,文件地址);

    BOOL bwrite[4];
    bwrite[0] = WritePrivateProfileString(_T("Device"), _T("IP0"), devIP[0], SETTING_FILE);
    bwrite[1] = WritePrivateProfileString(_T("Device"), _T("IP1"), devIP[1], SETTING_FILE);
    bwrite[2] = WritePrivateProfileString(_T("Device"), _T("IP2"), devIP[2], SETTING_FILE);
    bwrite[3] = WritePrivateProfileString(_T("Title"), _T("Position"), devTitle, SETTING_FILE);
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

    BOOL bwrite[4];
    // Section 名,Key 名,默认值,存储字符串,字符串所允许的最大长度(15),文件路径
    bwrite[0] = GetPrivateProfileString(_T("Device"), _T("IP0"), _T("NULL"), devIP[0].GetBufferSetLength(16), 16, SETTING_FILE);
    bwrite[1] = GetPrivateProfileString(_T("Device"), _T("IP1"), _T("NULL"), devIP[1].GetBufferSetLength(16), 16, SETTING_FILE);
    bwrite[2] = GetPrivateProfileString(_T("Device"), _T("IP2"), _T("NULL"), devIP[2].GetBufferSetLength(16), 16, SETTING_FILE);
    bwrite[3] = GetPrivateProfileString(_T("Title"), _T("Position"), _T("NULL"), g_devPosition.GetBufferSetLength(16), 16, SETTING_FILE);
    // 此操作对应于GetBufferSetLength(),紧跟其后，不能忽略。此操作的作用是将GetBufferSetLength()申请的多余的内存空间释放掉，以便于可以进行后续的如字符串+操作
    devIP[0].ReleaseBuffer();
    devIP[1].ReleaseBuffer();
    devIP[2].ReleaseBuffer();
    g_devPosition.ReleaseBuffer();
    if (!(bwrite[0] && bwrite[1] && bwrite[2] && bwrite[3]))
    {
        return FALSE;
    }
    for (int i = 0; i < DEV_NUM; i++)
    {
        g_csDeviceIP[i] = devIP[i];
        g_devTitleName = g_mapPositionName[g_devPosition];
    }
    return TRUE;
}


void SaveSingleTemp(float *pData, CString sFileSaveName)
{
    int len_float = sizeof(float);
    FILE *conti_buffer;//写连续保存的文件的句柄
    char *path_save;//连续保存数据的文件路径

    path_save = sFileSaveName.GetBuffer(0);
    conti_buffer = fopen(path_save, "w+b");
    if (conti_buffer == NULL)
    {
        OutputDebugString("文件创建失败！");
        return;
    }
    fseek(conti_buffer, 0, SEEK_SET);   //将文件指针指向文件头/
    fwrite(pData, len_float, PIC_WIDTH*PIC_HEIGHT, conti_buffer);
    fclose(conti_buffer);
}


void CSLTMDlg::WriteTempDataFile(int n)
{
    CString pos; pos.Format("-%d", n);
    std::ofstream ofile;

    CTime tm; tm = CTime::GetCurrentTime();
    CString csTime;
    csTime.Format("%ld", tm.GetTime());

    CString TempFileName = WRITE_FILE_PATH + g_devPosition + "\\" + csTime + pos + ".tmp";
    CString PicFileName = WRITE_FILE_PATH + g_devPosition + "\\" + csTime + pos + ".jpg";

    string s_path((LPSTR)(LPCTSTR)PicFileName);

    //OutputDebugString(s_path.c_str());

    imwrite(s_path, g_mPicData[n]);
    SaveSingleTemp(g_fTempData[n], TempFileName);
    //CheackFileMove(tm.GetTime(), n);
    return;
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

    for (int i = 0; i < 3; i++)
    {
        if (NET_DVR_STDXMLConfig(clDevice[i].UserID, &lpInputParam, &lpOutputParam))
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
#if HKWS
    UpdateData(TRUE);
    CString LogString;
    do
    {
        string sLink1 = clDevice[0].Connect();
        string sLink2 = clDevice[1].Connect();
        string sLink3 = clDevice[2].Connect();
        if (sLink1 != "" || sLink2 != "" || sLink3 != "")
            m_StaticLog.Format("1#%s 2#%s 3#%s", sLink1.c_str(), sLink2.c_str(), sLink3.c_str());

        GetP2PParam();
        //m_StaticLog.Format("登陆：登陆成功！");
        GetDlgItem(IDC_BUTTON_LOGIN)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_LOGOUT)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_Get_Hot_Pic)->EnableWindow(TRUE);
    } while (FALSE);
    OnBnClickedButtonGetHotPic();
    UpdateData(FALSE);
#else
    for (int i = 0; i < DEV_NUM; i++)
    {
        if (!CameraCtrl[i].IsRunning())
        {
            // IP
            CameraCtrl[i].ConnectIp((LPSTR)(LPCTSTR)g_csDeviceIP[i], i);

            // 设置B档 高温档位
            CameraCtrl[i].SetCameraStall('B');
        }
    }
#endif
}


void CSLTMDlg::OnBnClickedButtonLogout()
{
#if HKWS
    UpdateData(TRUE);
    CString LogString;
    int iItemCount = 0;

    do
    {
        string sLink1 = clDevice[0].Disconnect();
        string sLink2 = clDevice[1].Disconnect();
        string sLink3 = clDevice[2].Disconnect();
        if (sLink1 != "" || sLink2 != "" || sLink3 != "")
        {
            m_StaticLog.Format("1#%s 2#%s 3#%s", sLink1.c_str(), sLink2.c_str(), sLink3.c_str());
            break;
        }
        m_StaticLog.Format("注销：注销成功！");
        GetDlgItem(IDC_BUTTON_LOGIN)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_LOGOUT)->EnableWindow(FALSE);

    } while (FALSE);
    UpdateData(FALSE);

#else
    for (int i = 0; i < DEV_NUM; i++)
    {
        if (CameraCtrl[i].IsRunning())
        {
            CameraCtrl[i].Disconnect();
        }
    }
    m_staticImage1.Invalidate(TRUE);
    m_staticImage2.Invalidate(TRUE);
    m_staticImage3.Invalidate(TRUE);

#endif
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
    CStatic* DCposition = mapStaticPosition[Key - 1];

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
                    Mat tempmat(PIC_HEIGHT, PIC_WIDTH, CV_8UC3, pData);
                    g_mPicData[Key - 1] = tempmat;

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


void CSLTMDlg::JpgData2Gui(const Mat& image, int Key)
{
    CStatic* DCposition = mapStaticPosition[Key - 1];

    if (this->GetSafeHwnd())
    {
        if (!image.empty())
        {
            // Convert the image data to a JPEG format
            vector<uchar> buffer;
            imencode(".jpg", image, buffer);

            HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, buffer.size());
            void* pData = GlobalLock(hGlobal);
            memcpy(pData, buffer.data(), buffer.size());
            GlobalUnlock(hGlobal);

            IStream* pStream = NULL;
            if (CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK)
            {
                static CImage image;
                image.Destroy();
                if (SUCCEEDED(image.Load(pStream)))
                {
                    Mat tempmat(PIC_HEIGHT, PIC_WIDTH, CV_8UC3, pData);
                    g_mPicData[Key - 1] = tempmat;

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
    a.SetUserID(pDlg->clDevice[0].UserID, 1);
    CString stLogString;
    if (a.lUserID < 0)
    {
        pDlg->SetDlgItemText(IDC_STATIC_LOG, "抓热图：有设备还未登陆！");
    }
    else
    {
        pDlg->clDevice[0].getHotPic = TRUE;

        for (; (TRUE == pDlg->clDevice[0].getHotPic);)
        {
            //获取热图的SDK接口
            BOOL ERRGET;
            ERRGET = NET_DVR_CaptureJPEGPicture_WithAppendData(a.lUserID, a.channel, &a.struJpegWithAppendData);
            if (TRUE != ERRGET)
            {
                stLogString.Format("抓热图失败！错误信息：%s(%d)", NET_DVR_GetErrorMsg(), NET_DVR_GetLastError());
                pDlg->SetDlgItemText(IDC_STATIC_LOG, stLogString);
                a.failedTime++;
                continue;
            }
            else
            {
                if (a.struJpegWithAppendData.dwP2PDataLen == PIC_WIDTH * PIC_HEIGHT * sizeof(float))
                {
                    memcpy(g_fTempData[0], a.struJpegWithAppendData.pP2PDataBuff, a.struJpegWithAppendData.dwP2PDataLen);
                    int dev = 0;
                    cv::Point pMax;
                    float fmaxTemp;
                    g_iFPS[dev]++;
                    Mat tempmat(a.struJpegWithAppendData.dwJpegPicHeight, a.struJpegWithAppendData.dwJpegPicWidth, CV_8UC3, a.struJpegWithAppendData.pJpegPicBuff);
                    g_mPicData[dev] = tempmat;
                    pDlg->HandleTempFrame(g_fTempData[dev], pDlg->iCentroid[dev].pCentroid, pMax, fmaxTemp, dev);
                }
            }
        }
    }
    return 0;
}

UINT ThreadGetHotPicDataMutil2(LPVOID lpParam)
{
    CSLTMDlg* pDlg = (CSLTMDlg*)lpParam;
    CameraDev a;
    a.SetUserID(pDlg->clDevice[1].UserID, 2);
    CString stLogString;
    if (a.lUserID < 0)
    {
        pDlg->SetDlgItemText(IDC_STATIC_LOG, "抓热图：有设备还未登陆！");
    }
    else
    {
        pDlg->clDevice[1].getHotPic = TRUE;

        for (; (TRUE == pDlg->clDevice[1].getHotPic);)
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
            else
            {
                if (a.struJpegWithAppendData.dwP2PDataLen == PIC_WIDTH * PIC_HEIGHT * sizeof(float))
                {
                    memcpy(g_fTempData[1], a.struJpegWithAppendData.pP2PDataBuff, a.struJpegWithAppendData.dwP2PDataLen);
                    int dev = 1;
                    cv::Point pMax;
                    float fmaxTemp;
                    g_iFPS[dev]++;
                    Mat tempmat(a.struJpegWithAppendData.dwJpegPicHeight, a.struJpegWithAppendData.dwJpegPicWidth, CV_8UC3, a.struJpegWithAppendData.pJpegPicBuff);
                    g_mPicData[dev] = tempmat;
                    pDlg->HandleTempFrame(g_fTempData[dev], pDlg->iCentroid[dev].pCentroid, pMax, fmaxTemp, dev);
                }

            }
        }
    }
    return 0;
}

UINT ThreadGetHotPicDataMutil3(LPVOID lpParam)
{
    CSLTMDlg* pDlg = (CSLTMDlg*)lpParam;
    CameraDev a;

    a.SetUserID(pDlg->clDevice[2].UserID, 3);
    CString stLogString;
    if (a.lUserID < 0)
    {
        pDlg->SetDlgItemText(IDC_STATIC_LOG, "抓热图：有设备还未登陆！");
    }
    else
    {
        pDlg->clDevice[2].getHotPic = TRUE;

        for (; (TRUE == pDlg->clDevice[2].getHotPic);)
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
            else
            {
                if (a.struJpegWithAppendData.dwP2PDataLen == PIC_WIDTH * PIC_HEIGHT * sizeof(float))
                {
                    memcpy(g_fTempData[2], a.struJpegWithAppendData.pP2PDataBuff, a.struJpegWithAppendData.dwP2PDataLen);
                    int dev = 2;
                    cv::Point pMax;
                    float fmaxTemp;
                    g_iFPS[dev]++;
                    Mat tempmat(a.struJpegWithAppendData.dwJpegPicHeight, a.struJpegWithAppendData.dwJpegPicWidth, CV_8UC3, a.struJpegWithAppendData.pJpegPicBuff);
                    g_mPicData[dev] = tempmat;
                    pDlg->HandleTempFrame(g_fTempData[dev], pDlg->iCentroid[dev].pCentroid, pMax, fmaxTemp, dev);
                }

            }
        }
    }
    return 0;
}



/*UINT ThreadGetHotPic1(LPVOID lpParam)
//{
//	CSLTMDlg* pDlg = (CSLTMDlg*)lpParam;
//	CameraDev a;
//	CameraDev b;
//	CameraDev c;
//	a.SetUserID(pDlg->clDevice[0].UserID, 1);
//	b.SetUserID(pDlg->clDevice[1].UserID, 2);
//	c.SetUserID(pDlg->clDevice[2].UserID, 3);
//
//	int i; CString stLogString;
//	if (a.lUserID < 0 || b.lUserID < 0 || c.lUserID < 0)
//	{
//		pDlg->SetDlgItemText(IDC_STATIC_LOG, "抓热图：有设备还未登陆！");
//	}
//	else
//	{
//		pDlg->b_getHotPic = TRUE;
//
//		for (i = 0; (TRUE == pDlg->b_getHotPic); i++)
//		{
//			//获取热图的SDK接口
//
//			BOOL ERRGET1, ERRGET2, ERRGET3;
//
//			CString ot;
//			clock_t start = clock();
//			ERRGET1 = NET_DVR_CaptureJPEGPicture_WithAppendData(a.lUserID, a.channel, &a.struJpegWithAppendData);
//			clock_t stop1 = clock();
//			long t1 = (stop1 - start);
//
//			ERRGET2 = NET_DVR_CaptureJPEGPicture_WithAppendData(b.lUserID, b.channel, &b.struJpegWithAppendData);
//			clock_t stop2 = clock();
//			long t2 = (stop2 - stop1);
//
//			ERRGET3 = NET_DVR_CaptureJPEGPicture_WithAppendData(c.lUserID, c.channel, &c.struJpegWithAppendData);
//
//			clock_t stop3 = clock();
//			long t3 = (stop3 - stop2);
//
//
//
//			if (TRUE != ERRGET1 || TRUE != ERRGET2 || TRUE != ERRGET3)
//			{
//				stLogString.Format("抓热图失败！错误信息：%s(%d)", NET_DVR_GetErrorMsg(), NET_DVR_GetLastError());
//				pDlg->SetDlgItemText(IDC_STATIC_LOG, stLogString);
//				a.failedTime++;
//				continue;
//			}
//
//			pDlg->JpgData2Gui(a.struJpegWithAppendData.pJpegPicBuff, a.struJpegWithAppendData.dwJpegPicLen, a.iDeviceKey);
//			pDlg->JpgData2Gui(b.struJpegWithAppendData.pJpegPicBuff, b.struJpegWithAppendData.dwJpegPicLen, b.iDeviceKey);
//			pDlg->JpgData2Gui(c.struJpegWithAppendData.pJpegPicBuff, c.struJpegWithAppendData.dwJpegPicLen, c.iDeviceKey);
//
//			clock_t end = clock();
//			long t4 = (end - start);
//			ot.Format("%d %d %d %d\n", t1, t2, t3, t4);
//			OutputDebugString(ot);
//		}
//	}
//
//	// 清理控件上的画面
//	((CStatic*)pDlg->GetDlgItem(IDC_CAMERA1))->Invalidate(TRUE);
//	((CStatic*)pDlg->GetDlgItem(IDC_CAMERA2))->Invalidate(TRUE);
//	((CStatic*)pDlg->GetDlgItem(IDC_CAMERA3))->Invalidate(TRUE);
//	pDlg->b_getHotPic = FALSE;
//	return 1;
//}
*/
void CSLTMDlg::OnBnClickedButtonGetHotPic()
{
    UpdateData(TRUE);
    if (b_getHotPic == FALSE)
    {
        //开始获取热图

        //AfxBeginThread(ThreadGetHotPic1, this);
        AfxBeginThread(ThreadGetHotPicDataMutil, this);
        AfxBeginThread(ThreadGetHotPicDataMutil2, this);
        AfxBeginThread(ThreadGetHotPicDataMutil3, this);

        Sleep(300);

        if (b_getHotPic == TRUE)
        {
            this->SetDlgItemTextA(IDC_BUTTON_Get_Hot_Pic, _T("停止画面"));
            GetDlgItem(IDC_BUTTON_LOGOUT)->EnableWindow(FALSE);
        }

        NET_DVR_CLIENTINFO ClientInfo;
        ClientInfo.lChannel = 1; //Channel number 设备通道号

        ClientInfo.lLinkMode = 0;    //Main Stream
        ClientInfo.sMultiCastIP = NULL;

        BOOL bPreviewBlock = false;       //请求码流过程是否阻塞，0：否，1：是


        for (int i = 0; i < 3; i++)
        {
            ClientInfo.hPlayWnd = GetDlgItem(1000 + i)->GetSafeHwnd();  //窗口为空，设备SDK不解码只取流
            clDevice[i].llRealHandle = NET_DVR_RealPlay_V30(clDevice[i].UserID, &ClientInfo, NULL, this, bPreviewBlock);
        }


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

        for (int i = 0; i < 3; i++)
        {
            NET_DVR_StopRealPlay(clDevice[i].llRealHandle);
            clDevice[i].llRealHandle = -1;
        }
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
#ifdef HKWS
    NET_DVR_Cleanup();
#endif

    CDialogEx::OnClose();
}


std::map<CString, string>g_mapPositionNameForHTTP = {
    {"1", "LF1"}, {"2","F1"}, {"3","F2"}, {"4","RH1"},
    {"5", "LF2"}, {"6","F3"}, {"7","F4"}, {"8","RH2"},
    {"9", "T1"}, {"10","T2"}
};

/*
{
    "timelist":[
        {
            "number":"07",
            "time":"1663147878"
        },
        {
            "number":"07",
            "time":"1663147872"
        },
        {
            "number":"07",
            "time":"1663147867"
        },
        {
            "number":"07",
            "time":"1663147862"
        },
        {
            "number":"07",
            "time":"1663147857"
        },
        {
            "number":"01",
            "time":"1663147848"
        },
        {
            "number":"01",
            "time":"1663147832"
        },
        {
            "number":"07",
            "time":"1663147825"
        },
        {
            "number":"07",
            "time":"1663147817"
        },
        {
            "number":"00",
            "time":"1663147811"
        }
    ]
}
*/
void CSLTMDlg::GetTimeList()
{

#ifdef LOCAL_TEST
    string url = "127.0.0.1:8000/assets/timelist/?location=" + g_mapPositionNameForHTTP[g_devPosition];
#else
    //string url = "192.168.1.200:8000/homepage/timelist/?location=" + g_mapPositionNameForHTTP[g_devPosition];
    string url = "192.168.1.200:8000/homepage/timecheck/?location=" + g_mapPositionNameForHTTP[g_devPosition];
#endif

    auto resp = requests::get(url.c_str());
    if (resp == NULL) {

    }
    else {
        //printf("%d %s\r\n", resp->status_code, resp->status_message());
        int statuscode = resp->status_code;

        if (statuscode == 200)
        {
            string requestJson = resp->body.c_str();
            hv::Json root;
            root = hv::Json::parse(requestJson);

            int tilength = root["timelist"].size();
            g_BaohaoKey = 0;
            for (int k = 0; k < tilength; k++)
            {
                g_BaohaoKey++;
                string tempstr = root["timelist"][k]["number"];
                int tempint = atoi(tempstr.c_str());
                g_BaohaoList[k] = tempint;
                tempstr = root["timelist"][k]["time"];
                LONG templong = atol(tempstr.c_str());
                g_BoahaoTimeList[k] = templong;
            }
        }
        if (g_BaohaoKey)
        {
            CTime m_showTime = g_BoahaoTimeList[0];
            CString m_showstr;
            m_showstr = m_showTime.Format("%Y-%m-%d %H:%M:%S");

            CTimeSpan span = CTime::GetCurrentTime() - m_showTime;

            CString m_shownum;
            m_shownum.Format("%d", g_BaohaoList[0]);

            m_showstr = m_showstr + "   包号为：" + m_shownum;

            (span.GetTotalMinutes() > 45) ? m_showstr = "" : m_showstr;
            GetDlgItem(IDC_STATIC_TIMELIST)->SetWindowText(m_showstr);
        }
    }
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

void CSLTMDlg::cmptime(CTime t_local, CString csCmp, CString &m_oldFileName, CString &m_newFileName)
{
    if (csCmp != "")
    {
        long timeB = atol(csCmp.GetBuffer(0));
        csCmp.ReleaseBuffer();
        time_t tTime = t_local.GetTime();
        long lTime = (long)tTime;
        if (timeB != lTime && abs(lTime - timeB) <= 180)
        {
            m_oldFileName = csCmp;

            time_t ttimeB = (time_t)timeB;
            CTime cTImeB(ttimeB);

            m_newFileName = cTImeB.Format("%Y%m%d-%H%M%S");
        }
    }
}


void CSLTMDlg::MoveFileAndRename(int pos)
{
    int m_IsNumberMatchTime = -1; //判断条件

    //获取正在处理的数据名（即时间）
    CTime t, t_local;
    t = g_tGetFileTime[pos];

    //读取包号时间列表
    for (int k = 0; k < g_BaohaoKey; k++)
    {
        CTime tm = g_BoahaoTimeList[k];

        CTimeSpan span = t - tm;
        LONG spanP = span.GetTotalSeconds();

        if (abs(spanP) < 300) //包号和正面图片时间在90s内
        {
            //CString csShow;
            //csShow.Format("时间差 %ld秒", spanP);
            //m_StaticLog = csShow;
            m_IsNumberMatchTime = k;
            break;
        }
    }


    //GetDlgItem(IDC_STATIC_LOG)->SetWindowText(csShow);

    CString m_tempPath;
    CString m_oldFilePath;
    CString m_oldFileName;
    CString m_baohaoPath;
    CString m_newFileName;

    //判断需转移的文件名

    m_oldFileName.Format("%ld-%d.tmp", g_tGetFileTime[pos], pos);
    m_oldFilePath = WRITE_FILE_PATH + g_devPosition + "\\" + m_oldFileName;
    //WRITE_FILE_PATH + g_devPosition + "\\" + csTime + pos + ".tmp";
    if (m_IsNumberMatchTime != -1)
    {
        if (g_BaohaoList[m_IsNumberMatchTime] < 10)
        {
            m_baohaoPath.Format(_T("0%d"), g_BaohaoList[m_IsNumberMatchTime]);
        }
        else
        {
            m_baohaoPath.Format(_T("%d"), g_BaohaoList[m_IsNumberMatchTime]);
        }
        CTime m_fileTime = g_BoahaoTimeList[m_IsNumberMatchTime];
        t_local = m_fileTime;
        m_newFileName = m_fileTime.Format("%Y%m%d-%H%M%S");
    }
    else
    {
        m_baohaoPath = "00";
        m_newFileName = t.Format("%Y%m%d-%H%M%S");
        t_local = t;
    }

    // 20230220 添加逻辑判断 相近时间打为一包

    switch (pos)
    {
    case 0:
        cmptime(t_local, m_oldFileNameB, m_oldFileNameA, m_newFileName);
        cmptime(t_local, m_oldFileNameC, m_oldFileNameA, m_newFileName);
        break;
    case 1:
        cmptime(t_local, m_oldFileNameA, m_oldFileNameB, m_newFileName);
        cmptime(t_local, m_oldFileNameC, m_oldFileNameB, m_newFileName);
        break;
    case 2:
        cmptime(t_local, m_oldFileNameB, m_oldFileNameC, m_newFileName);
        cmptime(t_local, m_oldFileNameA, m_oldFileNameC, m_newFileName);
        break;
    default:
        break;
    }


    //确认新文件名
    if (pos == 0)
    {
        m_newFileName = m_baohaoPath + "-" + m_newFileName + "-A.tmp";
    }
    else if (pos == 1)
    {
        m_newFileName = m_baohaoPath + "-" + m_newFileName + "-B.tmp";
    }
    else if (pos == 2)
    {
        m_newFileName = m_baohaoPath + "-" + m_newFileName + "-C.tmp";
    }

    m_tempPath = END_FILE_PATH + g_devPosition + "\\" + m_newFileName;


    CopyFile(m_oldFilePath, m_tempPath, FALSE);
    g_NowFileFath = m_newFileName;
    KillTimer(nIDEventDeleteFile);
    SetTimer(nIDEventDeleteFile, 180000, 0);

}

void CSLTMDlg::deleteUselessFile()
{
    int fileKey[3] = { 0,0,0 };

    CString normalName = g_NowFileFath.Left(19);
    CString m_FileTitle, m_FileTitle2, m_FileTitle3;

    CString pathlocal;
    pathlocal = END_FILE_PATH + g_devPosition + "\\";
    m_FileTitle = pathlocal + normalName + "A.tmp";
    m_FileTitle2 = pathlocal + normalName + "B.tmp";
    m_FileTitle3 = pathlocal + normalName + "C.tmp";

    CFile tmptmp;
    if (!tmptmp.Open(m_FileTitle, CFile::modeRead))
    {
        fileKey[0] = 0;
    }
    else
    {
        fileKey[0] = 1;
        tmptmp.Close();
    }

    if (!tmptmp.Open(m_FileTitle2, CFile::modeRead))
    {
        fileKey[1] = 0;
    }
    else
    {
        fileKey[1] = 1;
        tmptmp.Close();
    }

    if (!tmptmp.Open(m_FileTitle3, CFile::modeRead))
    {
        fileKey[2] = 0;
    }
    else
    {
        fileKey[2] = 1;
        tmptmp.Close();
    }

    /************************************************************************/
    /* 文件缺少2个画面 删除数据												*/
    /* 文件正常 保存数据到历史趋势                                          */
    /************************************************************************/
    int keySum = 0;
    keySum = fileKey[0] + fileKey[1] + fileKey[2];

    if (keySum < 2)
    {
        if (fileKey[0])
        {
            //DeleteFile(m_FileTitle);
        }
        if (fileKey[1])
        {
            //DeleteFile(m_FileTitle2);
        }
        if (fileKey[2])
        {
            //DeleteFile(m_FileTitle3);
        }
    }
    else
    {
        // 输出数据到数据后台
        CString m_baohao, m_time;
        m_baohao = normalName.Left(2);
        m_time = normalName.Mid(3, 15);
        m_time.Replace("-", " ");
        m_time.Insert(13, ":");
        m_time.Insert(11, ":");
        m_time.Insert(6, "-");
        m_time.Insert(4, "-");

        string s_baohao, s_time;
        s_baohao = m_baohao.GetBuffer(0);
        m_baohao.ReleaseBuffer();
        s_time = m_time.GetBuffer(0);
        m_time.ReleaseBuffer();
        string s_localpos = g_mapPositionNameForHTTP[g_devPosition];
        string jsonStr = writeJson(s_baohao, s_localpos, s_time);
        string url = "192.168.1.200:8000/homepage/report/?location=" + s_localpos;
        auto resp = requests::post(url.c_str(), jsonStr);
        if (resp != NULL)
        {
            string returnStr = "success";
        }
        resp.reset();
    }
}


void CSLTMDlg::CheackFileMove(__time64_t tName, int dev)
{
    g_tGetFileTime[dev] = tName;
    switch (dev)
    {
    case 0:
        SetTimer(nIDEventCheakFile, 20000, 0);
    case 1:
        SetTimer(nIDEventCheakFile2, 20000, 0);
    case 2:
        SetTimer(nIDEventCheakFile3, 20000, 0);
    }
    //	MoveFileAndRename(dev);
}

// Threshold-阈值 Centroid-质心
#define TempThreshold 50.0 //温度阈值
#define PointThreshold 5000 //高温点阈值

static inline bool ContoursSortFun(vector<cv::Point> contour1, vector<cv::Point> contour2)
{
    return (cv::contourArea(contour1) > cv::contourArea(contour2));
}
// 解析温度数据帧 自动截图逻辑
// 输入温度数组 设备位置 
// 输出最高温点 最高温坐标 质心坐标
void CSLTMDlg::HandleTempFrame(float* tempMatrix, Point &pCentroid, Point &pMaxTempPoint, float &fMaxTemp, int dev)
{
    Mat tempDataMat(PIC_HEIGHT, PIC_WIDTH, CV_32FC1, tempMatrix);
    double	minVal, maxVal;
    int		minIdx[2] = {}, maxIdx[2] = {};
    cv::minMaxIdx(tempDataMat, &minVal, &maxVal, minIdx, maxIdx);

    Point maxPoint(maxIdx[1], maxIdx[0]);

    Point centroidPoint(-1, -1); //无高温区块，质心初始化为(-1, -1)

    fMaxTemp = maxVal;
    pCentroid = centroidPoint;
    pMaxTempPoint = maxPoint;

    // 获取二值化图与统计高温点
    Mat imgBinarization = Mat(PIC_HEIGHT, PIC_WIDTH, CV_8UC1);

    float	fTempThreshold = TempThreshold; //温度阈值
    if (g_devPosition == "2" || g_devPosition == "9") // 老的设备 1号转炉1号铁包
    {
        fTempThreshold = 50.0;
    }
    int		iThreshold = g_mapPointThreshold[g_devPosition].iThreshold[dev]; // 个性化该位置该画面的温度点阈值

    int		iPointSum = 0; //高温点总计
    int		iPointXSum = 0, iTargetPointSum = 0, iPointYSum = 0;

    for (int i = 0; i < PIC_HEIGHT; i++)
    {
        for (int j = 0; j < PIC_WIDTH; j++)
        {
            if (tempDataMat.at<float>(i, j) > fTempThreshold)
            {
                iPointSum++;
                imgBinarization.at<uchar>(i, j) = 255;
                iTargetPointSum++;
                iPointXSum += j;
                iPointYSum += i;
            }
            else
            {
                imgBinarization.at<uchar>(i, j) = 0;
            }
        }
    }

    // 阈值判断
    if (iPointSum > iThreshold)
    {
        centroidPoint.x = (iPointXSum / iPointSum);
        centroidPoint.y = (iPointYSum / iPointSum);
        // 获取开操作之后的灰度图开始
        Mat imgAfterOpen = Mat(PIC_HEIGHT, PIC_WIDTH, CV_8UC1);

        //对图像进行开操作，排除干扰点
        Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
        morphologyEx(imgBinarization, imgAfterOpen, 2, kernel);

        // 轮廓合集contours，hierarchy用于Tree状结构，在此无其他作用
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;

        /* CV_RETR_EXTERNAL 只检测最外围轮廓                                     */
        /* CV_CHAIN_APPROX_SIMPLE 仅保存轮廓的拐点信息                           */
        /* 特别注意 该函数在某些版本OPENCV 编译时会导致内存泄漏                    */
        findContours(imgAfterOpen, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

        Mat imageContours = Mat::zeros(imgAfterOpen.size(), CV_8UC1);
        Mat Contours = Mat::zeros(imgAfterOpen.size(), CV_8UC1);

        sort(contours.begin(), contours.end(), ContoursSortFun); //轮廓按面积由大至小排序

        double dLengthWidthRatio = 0.0;
        Point pREPMaxtempPoint(0, 0);
        Rect rects1 = boundingRect(contours[0]);

        if (rects1.area() > 0.8*PointThreshold)
        {
            // 长宽比
            dLengthWidthRatio = (rects1.height*1.0) / rects1.width;

            // 排除铁包上部0.2区域求温度值（可能有裸露的铁包口部分）	在
            Point startPoint = rects1.tl();
            startPoint.y = startPoint.y + (int)(0.2 * rects1.height);
            Mat TempDataMatREP(tempDataMat, Rect(startPoint, rects1.br()));

            double REPminVal, REPmaxVal;
            int   REPminIdx[2] = {}, REPmaxIdx[2] = {};	// 修正后的minnimum Index, maximum Index
            cv::minMaxIdx(TempDataMatREP, &REPminVal, &REPmaxVal, REPminIdx, REPmaxIdx);

            pREPMaxtempPoint.x = startPoint.x + REPmaxIdx[1];
            pREPMaxtempPoint.y = startPoint.y + REPmaxIdx[0];

            maxVal = REPmaxVal;
            maxPoint = pREPMaxtempPoint; // 将高温点修正到包上

            centroidPoint.x = rects1.tl().x + (rects1.br().x - rects1.tl().x) / 2;
            centroidPoint.y = rects1.tl().y + (rects1.br().y - rects1.tl().y) / 2;

            fMaxTemp = maxVal;
            pCentroid = centroidPoint;
            pMaxTempPoint = maxPoint;
        }
        else
        {
            pCentroid = Point(-1, -1);
            return;
        }

        // 图像截取条件
        // 中心区域是否被覆盖
        Rect rTopRect = Rect(162, 67, 60, 20);
        Rect rMiddleRect;
        rMiddleRect = Rect(162, 134, 60, 20);
        if (g_devPosition == "6" && dev == 1)
        {
            rMiddleRect = Rect(50, 134, 60, 20);
        }

        Rect rBottomRect = Rect(162, 201, 60, 20);
        Rect rRightRect = Rect(320, 60, 20, 150);
        Rect rLeftRect = Rect(32, 60, 20, 150);
        Scalar MiddleValue = mean(imgBinarization(rMiddleRect));
        float fMiddleMeanValue = MiddleValue.val[0];
        Scalar TopValue = mean(imgBinarization(rTopRect));
        float fTopValue = TopValue.val[0];
        Scalar BottomValue = mean(imgBinarization(rBottomRect));
        float fBottomMeanValue = BottomValue.val[0];
        Scalar RightValue = mean(imgBinarization(rRightRect));
        float fRightMeanValue = RightValue.val[0];
        Scalar LeftValue = mean(imgBinarization(rLeftRect));
        float fLeftMeanValue = LeftValue.val[0];

        CTime tm; tm = CTime::GetCurrentTime();
        CTimeSpan span = tm - ctInitTime[dev];
        LONG tspan = span.GetTotalSeconds();

        bool* bJudgeCon = g_mapSaveJudge[g_devPosition].iJudge[dev];

        // 个性化图像存储条件 时差10分钟防止重复
        if (tspan > 600
            && (fMiddleMeanValue > 200 || bJudgeCon[0])
            && (fTopValue > 100 || bJudgeCon[1])
            && (fBottomMeanValue > 100 || bJudgeCon[2])
            && (fRightMeanValue < 20 || bJudgeCon[3])
            && (fLeftMeanValue < 20 || bJudgeCon[4]))
        {
            WriteTempDataFile(dev);
            ctInitTime[dev] = tm;
        }
        return;
    }
    else
    {
        pCentroid = centroidPoint;
        pMaxTempPoint = maxPoint;
        return;
    }
}


void CSLTMDlg::BmpData2Gui(unsigned char* pBits, int width, int height, int position)
{
    if (mapStaticPosition[position]->GetSafeHwnd())
    {
        g_iFPS[position] ++;
        CClientDC dc(mapStaticPosition[position]);
        RECT rect;
        SetStretchBltMode(dc.GetSafeHdc(), STRETCH_HALFTONE);
        mapStaticPosition[position]->GetClientRect(&rect);

        Mat matBmp(height, width, CV_8UC3, pBits);
        g_mPicData[position] = matBmp;
        if (iCentroid[position].iCentroidExist)
        {
            // cv::circle(matBmp, iCentroid[position].pCentroid, 3, Scalar(0, 255, 0), 2);
            cv::circle(matBmp, iCentroid[position].pMaxTemp, 2, Scalar(255, 255, 0), 2);

            string sMaxTemp;
#ifdef HKWS
            float maxtmepf = iCentroid[position].fMaxtemp;
#else
            float maxtmepf = (iCentroid[position].fMaxtemp > 200.0) ? iCentroid[position].fMaxtemp : (200 - iCentroid[position].fMaxtemp) * 0.25 + 200;
#endif
            sMaxTemp = to_string(maxtmepf);
            sMaxTemp = sMaxTemp.substr(0, sMaxTemp.find(".") + 3);

            cv::putText(matBmp, sMaxTemp, cv::Point(0, 254), cv::FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 0), 2);
            //cv::rectangle(matBmp, cv::Rect(iCentroid[position].pMaxTemp.x, iCentroid[position].pMaxTemp.y,3,3), Scalar(255, 255, 0), 2);
            //cv::putText(matBmp, sMaxTemp, iCentroid[position].pMaxTemp, cv::FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 0), 2);
        }

        CImage ciBmp;
        MatToCImage(matBmp, ciBmp);
        ciBmp.Draw(dc.GetSafeHdc(), rect);
    }
}


// 已经整合为一个函数 弃用下方三函数
void CSLTMDlg::BmpData2Gui0(unsigned char* pBits, int width, int height)
{
    if (this->GetSafeHwnd())
    {
        g_iFPS[0] ++;

        CClientDC dc(&m_staticImage1);
        RECT rect;
        SetStretchBltMode(dc.GetSafeHdc(), STRETCH_HALFTONE);
        m_staticImage1.GetClientRect(&rect);

        Mat matBmp(height, width, CV_8UC3, pBits);
        CImage ciBmp;
        MatToCImage(matBmp, ciBmp);
        ciBmp.Draw(dc.GetSafeHdc(), rect);
    }
}
void CSLTMDlg::BmpData2Gui1(unsigned char* pBits, int width, int height)
{
    if (this->GetSafeHwnd())
    {
        g_iFPS[1] ++;

        CClientDC dc(&m_staticImage2);

        SetStretchBltMode(dc.GetSafeHdc(), STRETCH_HALFTONE);

        RECT rect;
        m_staticImage2.GetClientRect(&rect);

        Mat matBmp(height, width, CV_8UC3, pBits);
        CImage ciBmp;
        MatToCImage(matBmp, ciBmp);
        ciBmp.Draw(dc.GetSafeHdc(), rect);
    }
}
void CSLTMDlg::BmpData2Gui2(unsigned char* pBits, int width, int height)
{
    if (this->GetSafeHwnd())
    {
        g_iFPS[2] ++;

        CClientDC dc(&m_staticImage3);
        RECT rect;
        SetStretchBltMode(dc.GetSafeHdc(), STRETCH_HALFTONE);
        m_staticImage3.GetClientRect(&rect);

        Mat matBmp(height, width, CV_8UC3, pBits);
        CImage ciBmp;
        MatToCImage(matBmp, ciBmp);
        ciBmp.Draw(dc.GetSafeHdc(), rect);
    }
}

void CSLTMDlg::DataTransfer(unsigned char* pBGR, int bgrLen, float* tempMatrix, int width, int height, int dev)
{
    if (width == PIC_WIDTH && height == PIC_HEIGHT)
    {
        memcpy(g_fTempData[dev], tempMatrix, width * height * sizeof(float));
        cv::Point pMax;
        float fmaxTemp;
        //GetCentroid(tempMatrix, iCentroid[dev].pCentroid, pMax, dev);
        g_temp_frame_fps[dev]++;
        HandleTempFrame(tempMatrix, iCentroid[dev].pCentroid, iCentroid[dev].pMaxTemp, iCentroid[dev].fMaxtemp, dev);
        if (iCentroid[dev].pCentroid != cv::Point(-1, -1))
        {
            iCentroid[dev].iCentroidExist = 1;
        }
        else
        {
            iCentroid[dev].iCentroidExist = 0;
        }
    }
}

char* UtfToGbk(const char* utf8)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len + 1];
    memset(wstr, 0, len + 1);
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
    len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len + 1];
    memset(str, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
    if (wstr) delete[] wstr;
    return str;
}

UINT SendJsonMessage(LPVOID pParam)
{
    string s_localpos = g_mapPositionNameForHTTP[g_devPosition];
    string jsonStr = writeJson("11", s_localpos, "2022-09-23 11:25:15");
    string url = "192.168.1.200:8000/homepage/report/?location=" + s_localpos;
    auto resp = requests::post(url.c_str(), jsonStr);

    if (resp != NULL)
    {
        string returnStr = "success";
        OutputDebugString(returnStr.c_str());
        string retStr = UtfToGbk(resp->body.c_str());
    }

    resp.reset();
    return 0;
}
CTime tOld = CTime(2021, 10, 10, 10, 10, 10);
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
        string sfps1 = to_string(g_showFPS[0]);
        string sfps2 = to_string(g_showFPS[1]);
        string sfps3 = to_string(g_showFPS[2]);
        string sfpssend = sfps1 + " " + sfps2 + " " + sfps3;
        return resp->String(sfpssend.c_str());
    });

    router.GET("/capture", [pDlg](HttpRequest* req, HttpResponse* resp) {

        CTime tNow = CTime::GetCurrentTime();
        CTimeSpan span = tNow - tOld;
        CString cstime;
        cstime.Format("%ld", span.GetTotalSeconds());
        OutputDebugString(cstime);

        int delay_time;
        // 1号转炉钢包 delay时间设置更长
        //g_devPosition == "2" ? delay_time = 60 : 
        delay_time = 20;
        if (span.GetTotalSeconds() < delay_time) // 防止误触发
        {
            resp->json["args"] = "";
            tOld = tNow;
            return 200;
        }
        else
        {
            tOld = tNow;
            resp->json["args"] = req->query_params;
            hv::Json a = req->query_params;
            string baohao = a["baohao"];
            pDlg->MakeDirWithTimeString(baohao);

            return 200;
        }

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


HBRUSH CSLTMDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    CFont m_font;
    // 先判断是否是要特殊显示的标语字体
    int iDlgCtlID = pWnd->GetDlgCtrlID();
    switch (iDlgCtlID)
    {
    case IDC_STATIC_TITLE:
        m_font.CreatePointFont(200, "MICROSOFT YAHEI");//代表15号字体，华文行楷
        pDC->SetTextColor(RGB(250, 250, 250));
        pDC->SelectObject(&m_font);
        pDC->SetBkMode(TRANSPARENT);
        return m_brushBack;
    case IDC_STATIC_TIMELIST:
        m_font.CreatePointFont(200, "MICROSOFT YAHEI");//代表15号字体，华文行楷
        pDC->SetTextColor(RGB(250, 250, 250));
        pDC->SelectObject(&m_font);
        pDC->SetBkMode(TRANSPARENT);
        return m_brushBack;
    case IDC_STATIC_LOG:
        m_font.CreatePointFont(200, "MICROSOFT YAHEI");//代表15号字体，华文行楷
        pDC->SetTextColor(RGB(250, 250, 250));
        pDC->SelectObject(&m_font);
        pDC->SetBkMode(TRANSPARENT);
        return m_brushBack;
    case IDC_STATIC_TEMPALERT:
        m_font.CreatePointFont(200, "MICROSOFT YAHEI");
        pDC->SetTextColor(RGB(255, 0, 0));
        pDC->SetBkColor(RGB(255, 140, 0));
        //pDC->SetBkMode(TRANSPARENT);
        pDC->SelectObject(&m_font);
        return m_brushBack;
    case IDC_STATIC_THROUGH:
        m_font.CreatePointFont(200, "MICROSOFT YAHEI");
        pDC->SetTextColor(RGB(255, 0, 0));
        pDC->SetBkColor(RGB(255, 140, 0));
        //pDC->SetBkMode(TRANSPARENT);
        pDC->SelectObject(&m_font);
        return m_brushBack;
    default:
        break;
    }


    if (nCtlColor == CTLCOLOR_STATIC)
    {
        m_font.CreatePointFont(90, "MICROSOFT YAHEI");
        pDC->SetTextColor(RGB(250, 250, 250));
        pDC->SelectObject(&m_font);
        pDC->SetBkMode(TRANSPARENT);
        //return a not NULL brush handle
        return m_brushBack;
    }

    // TODO:  如果默认的不是所需画笔，则返回另一个画笔
    return hbr;
}

int g_count_time = 0;
void CSLTMDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == nIDEventUpdateFPS)
    {
        CString csFPS, cs_temp_fps;
        csFPS.Format("%d %d %d", g_iFPS[0], g_iFPS[1], g_iFPS[2]);
        cs_temp_fps.Format(" %d %d %d", g_temp_frame_fps[0], g_temp_frame_fps[1], g_temp_frame_fps[2]);

#ifdef HKWS
#else
        for (int i = 0; i < 3; i++)
        {
            g_showFPS[i] = g_iFPS[i];
        }
        csFPS = csFPS + cs_temp_fps;

        // 10s帧数为0 重启
        if (g_iFPS[0] || g_iFPS[1] || g_iFPS[2]) // 但凡单画面温度帧为0
        {
            g_count_time++;
        }
        else
        {
            g_count_time = 0;
        }

        if (g_count_time == 20) // 20s不回复
        {
            OnBnClickedButtonLogout();
            OnBnClickedButtonLogin();
            g_count_time = 0;
        }
#endif
        GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(csFPS);
        memset(g_iFPS, 0, sizeof(g_iFPS));
        memset(g_temp_frame_fps, 0, sizeof(g_temp_frame_fps));

        if (!fakeimage.empty())
        {
            JpgData2Gui(fakeimage, 3);
        }
    }
    else if (nIDEvent == nIDEventUpdateAlert)
    {
        BOOL bMoveThrough = FALSE;

        if (iCentroid[0].iCentroidExist || iCentroid[1].iCentroidExist || iCentroid[2].iCentroidExist)
        {
            bMoveThrough = TRUE;
        }
        GetDlgItem(IDC_STATIC_THROUGH)->ShowWindow(bMoveThrough);

    }
    else if (nIDEvent == nIDEventCheakFile)
    {
        MoveFileAndRename(0);
        KillTimer(nIDEventCheakFile);
    }
    else if (nIDEvent == nIDEventCheakFile2)
    {
        MoveFileAndRename(1);
        KillTimer(nIDEventCheakFile2);
    }
    else if (nIDEvent == nIDEventCheakFile3)
    {
        MoveFileAndRename(2);
        KillTimer(nIDEventCheakFile3);
    }
    else if (nIDEvent == nIDEventGetTimeList)
    {
        GetTimeList();
    }
    else if (nIDEvent == nIDEventDeleteFile)
    {
        deleteUselessFile();
        KillTimer(nIDEventDeleteFile);
    }
    CDialogEx::OnTimer(nIDEvent);
}


void CSLTMDlg::OnBnClickedButtonTestdata()
{
    // TODO: 在此添加控件通知处理程序代码
    //AfxBeginThread(SendJsonMessage, this);
    //WriteTempDataFile(0);
    //WriteTempDataFile(1);
    //WriteTempDataFile(2);
    AfxBeginThread(SendJsonMessage, this);
}


BOOL CSLTMDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        ShowWindow(SW_MINIMIZE);
        return FALSE;
    }
    else if (pMsg->wParam == VK_ESCAPE)
    {
        if (MessageBox("确定要退出程序吗？ 如只希望最小化，请按回车键", "退出提示", MB_ICONINFORMATION | MB_YESNO) == IDNO)
        {
            this->OnClose();
            return TRUE;
        }
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}

std::string GetNumberCheck()
{
#ifdef LOCAL_TEST
    string url = "127.0.0.1:8000/assets/numbercheck/?location=" + g_mapPositionNameForHTTP[g_devPosition];
#else
    //string url = "192.168.1.200:8000/homepage/timelist/?location=" + g_mapPositionNameForHTTP[g_devPosition];
    string url = "192.168.1.200:8000/homepage/numbercheck/?location=" + g_mapPositionNameForHTTP[g_devPosition];
#endif

    auto resp = requests::get(url.c_str());
    if (resp != NULL)
    {
        int statuscode = resp->status_code;
        if (statuscode == 200)
        {
            string requestJson = resp->body.c_str();
            hv::Json root;
            root = hv::Json::parse(requestJson);

            string tempstr = root["timelist"][0]["number"];
            return tempstr;
        }

    }
}


UINT UploadData(LPVOID lpParam)
{
    CSLTMDlg* pDlg = (CSLTMDlg*)lpParam;

    int old_bao_number = pDlg->info.bao_number;
    int time_long = pDlg->info.time_long;
    // 延迟60秒
    Sleep(60000);
    //CTime tm = g_BoahaoTimeList[0];
    int bao_number = g_BaohaoList[0];
    CString m_oldFileName;
    CString m_baohaoPath, m_oldBaohaoName;
    CString m_timePath;
    CString m_newFileName;

    //判断需转移的文件名
    CString folder_path = CAPTURE_FILE_PATH;
    m_baohaoPath = CString(GetNumberCheck().c_str());
    //bao_number < 10 ? m_baohaoPath.Format(_T("0%d"), bao_number) : m_baohaoPath.Format(_T("%d"), bao_number);
    old_bao_number < 10 ? m_oldBaohaoName.Format(_T("0%d"), old_bao_number) : m_oldBaohaoName.Format(_T("%d"), old_bao_number);

    CTime m_fileTime = time_long;
    m_timePath = m_fileTime.Format("%Y%m%d-%H%M%S");

    char suffix[3] = { 'A','B','C' };

    for (int i = 0; i < 3; i++)
    {
        //old \\\\192.168.1.200\\Share\\capture\\local\\timelist\\number-pos.tmp
        m_oldFileName.Format("%s%s\\%ld\\%s-%d.tmp", folder_path, g_devPosition, time_long, m_oldBaohaoName, i);
        //new \\\\192.168.1.200\\Share\\data\\local\\number-timelist-pos.tmp
        m_newFileName.Format("%s%s\\%s-%s-%c.tmp", END_FILE_PATH, g_devPosition, m_baohaoPath, m_timePath, suffix[i]);

        CString nf = m_newFileName;
        CopyFile(m_oldFileName, m_newFileName, FALSE);
    }

    // 输出数据到数据后台
    CString m_baohao, m_time;
    m_baohao = m_baohaoPath;
    m_time = m_timePath;
    m_time.Replace("-", " ");
    m_time.Insert(13, ":");
    m_time.Insert(11, ":");
    m_time.Insert(6, "-");
    m_time.Insert(4, "-");

    string s_baohao, s_time;
    s_baohao = m_baohao.GetBuffer(0);
    m_baohao.ReleaseBuffer();
    s_time = m_time.GetBuffer(0);
    m_time.ReleaseBuffer();
    string s_localpos = g_mapPositionNameForHTTP[g_devPosition];
    string jsonStr = writeJson(s_baohao, s_localpos, s_time);
    string url = "192.168.1.200:8000/homepage/report/?location=" + s_localpos;

    if (s_baohao != "99")
    {
        auto resp = requests::post(url.c_str(), jsonStr);
        resp.reset();
    }

    return 0;
}

void MakeDir(CString folder_name)
{
    //检查文件夹是否存在
    BOOL exists = GetFileAttributes(folder_name) != INVALID_FILE_ATTRIBUTES;

    //如果不存在，创建文件夹
    if (!exists)
    {
        BOOL result = CreateDirectory(folder_name, NULL);
        if (result)
        {
            //cout << "文件夹创建成功" << endl;
        }
        else
        {
            //cout << "文件夹创建失败" << endl;
        }
    }
    else
    {
        //cout << "文件夹已存在" << endl;
    }
}


void CSLTMDlg::CaptureTemp(CString file_path, string baohao)
{

    CString folder_name;
    CTime tm; tm = CTime::GetCurrentTime();
    folder_name.Format("%ld", tm.GetTime());
    folder_name = file_path + folder_name;
    MakeDir(folder_name);

    CString baonumber = CString(baohao.c_str());
    for (int i = 0; i < 3; i++)
    {
        CString str;
        str.Format(_T("%d"), i);
        CString TempFileName = folder_name + "\\" + baonumber + "-" + str + ".tmp";
        CString PicFileName = folder_name + "\\" + baonumber + "-" + str + ".jpg";

        string s_path((LPSTR)(LPCTSTR)PicFileName);

        imwrite(s_path, g_mPicData[i]);
        SaveSingleTemp(g_fTempData[i], TempFileName);
    }
    // 设置子线程 一段时间后执行

    info.bao_number = atol(baohao.c_str());
    info.time_long = tm.GetTime();

    AfxBeginThread(UploadData, this);
}


void CSLTMDlg::MakeDirWithTimeString(string baohao)
{
    CString folder_path = CAPTURE_FILE_PATH;
    BOOL exists = GetFileAttributes(folder_path) != INVALID_FILE_ATTRIBUTES;
    if (!exists)
        MakeDir(folder_path);

    //CString folder_name = now.Format("%Y%m%d%H%M%S");

    folder_path = folder_path + g_devPosition + "\\";
    MakeDir(folder_path);

    CaptureTemp(folder_path, baohao);
}
