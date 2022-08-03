
// SLTMDlg.h: 头文件
//

#pragma once
#include "afxwin.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <time.h>


#include "ChannelControl.h"
using namespace std;
using namespace cv;

#define PIC_WIDTH 384
#define PIC_HEIGHT 288

#ifdef HKWS
#include "HCNetSDK.h"
#include "xmlmanage.h"
#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib,"HCCore.lib")
#pragma comment(lib,"HCNetSDK.lib")
#pragma comment(lib,"PlayCtrl.lib")
#pragma comment(lib,"GdiPlus.lib")

class CameraLink
{
public:
	CameraLink();

	LONG UserID = -1;
	NET_DVR_USER_LOGIN_INFO struLoginInfo = { 0 };
	NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = { 0 };
	int DevicePort = 8000;
	char *UserName = "admin";
	char *Password = "dxwx12345";
	BOOL getHotPic = FALSE;
	LONG getUserID();
	void setIP(char *inIP);
	string Connect();
	string Disconnect();
protected:
	char* IP;
};
#endif
// CSLTMDlg 对话框
class CSLTMDlg : public CDialogEx
{
// 构造
public:
	CSLTMDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SLTM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnBnClickedButtonQuit();
	afx_msg void OnClose();

	void DataTransfer(unsigned char* pBGR, int bgrLen, float* tempMatrix, int width, int height, int dev);
	void StartHTTPServer();

	void SetInfraredData(char *str);
	bool GetCentroid(float* tempMatrix, Point &pCentroid, Point &pMaxTempPoint);
	void BmpData2Gui0(unsigned char* pBits, int width, int height);
	void BmpData2Gui1(unsigned char* pBits, int width, int height);
	void BmpData2Gui2(unsigned char* pBits, int width, int height);
	afx_msg void OnBnClickedButtonLogin();

	BOOL ReadPara();
	CString g_csDeviceIP[3];
	
	bool b_getHotPic;
	int m_DevicePort;
	LONG lChannel;
	LONG lRealTimeInfoHandle;
	CString m_UserName;
	CString m_Password;
	CString m_StaticLog;
	CString m_StaticTemp;
	int GetP2PParam();

	int m_maxFrameRate;
	BOOL m_reflectiveEnable;
	float m_reflectiveTemp;
	float m_emissivity;
	int m_distance;
	int m_refreshInterval;
	int m_DataLength;
	BOOL m_jpegPicEnabled;
	BOOL m_visJpegPicEnabled;

	afx_msg void OnBnClickedButtonLogout();
	void JpgData2Gui(char* pJpg, int nJpgLen, int Key);
	afx_msg void OnBnClickedButtonGetHotPic();
	CStatic m_staticImage1;
	CStatic m_staticImage2;
	CStatic m_staticImage3;
	int llRealHandle;
	int llRealHandle2;
	int llRealHandle3;
#ifdef HKWS
	CameraLink clDevice;
	CameraLink clDevice2;
	CameraLink clDevice3;
#endif

	ChannelControl _ChannelControl0;
	ChannelControl _ChannelControl1;
	ChannelControl _ChannelControl2;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonTestdata();
};

