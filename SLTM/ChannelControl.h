#ifndef CHANNEL_CONTROL_H
#define CHANNEL_CONTROL_H

///////////////////////////
#include "SCT_DataType.h"
#include "SCT_ChannelSDK.h"

#ifndef _WIN64
//#pragma comment(lib,"SCT_SDK_CHANNEL.lib")
#else
//#pragma comment(lib,"SCT_SDK_CHANNEL_X64.lib")
#endif

#include <string>

class ChannelControl
{
public:
	ChannelControl(void);
	~ChannelControl(void);

protected:
	HChannel _hChanne1;     //设备句柄
	OpenParams _openParams; //链接参数

	static bool s_bIniFlag; //初始化标志

	char _dstIp[32];

	MtImgInfo _mtImgInfo;
	FrameInfo _jpgFrame;
	FrameInfo _bgrFrame;
	FrameInfoFt _tempFrame;
	FrameInfo _rawFrame;

	PointInfo _pointInfo;
	LineInfo _lineInfo;
	RectInfo _rectInfo;
	JpgPlusData _jpgPlusData;

	MarkersInfo _markersInfo;

	//bool IsOnline(char *host,int port, int timeout);
	DSP_CONFIG_PARA _dspConfig;

	/////////////////////////////////////////
public:
	static char* GetSDKVersion();
	static void IniEnvironment(); //初始化SDK环境
	static void QuitEnvironment(); //离开SDK环境
	bool ConnectIp(char* ip, int wndIndex); //链接相机
	void Disconnect();//断开链接
	bool IsRunning();  //SDK线程调度是否处于运行中
	bool IsTcpConnected();//设备是否链接

	FrameInfo* GetBGRData();		//获取位图数据，不推荐使用
	FrameInfo* GetJPGData();		//获取JPG数据，推荐
	FrameInfoFt* GetTempData()	;//获取温度帧, 比较占用CPU，不建议以25fps帧率实时获取。
	FrameInfo* GetRawFrame();	//获取原始帧
	MtImgInfo* GetMtImgInfo();
	PointInfo* GetPointInfo(int index);
	LineInfo* GetLineInfo(int index);
	RectInfo* GetRectInfo(int index);
	MarkersInfo* GetAllMarkersInfo();

	void DoZero();//调零
	void DoStallUp();
	void FocusNear();//近焦
	void FocusFar();//远焦
	void FocusStop();	//停止聚焦
	//变倍
	void Zoom(VIRTUAL_KEYBOARD * virtual_keyboard); //变倍

	void ZoomIn();
	void ZoomOut();
	void ZoomStop();


	void AutoFocus();//自动聚焦
	void SetCurrentPalette(int index);//设置伪彩
	void SetDspImageMode(int index);//设置图像模式
	void SetRawFps(int nfps);//设置帧率

	void SetVideoFps(int nfps);//设置视频帧率

	void SetCameraStall(char sel );		 //设置相机档位
	void SetCameraLens(unsigned char sel); //设置镜头序号
	void SetEmissivity(float ftEmissivity);//取值 0.00~1.00
	void SetBKTemp(float ftBKTemp);	//摄氏度
	void SetDistance(unsigned int distance);	//单位：分米
	void SetReviseTemper(float ftReviseTemper);//摄氏度
	void SetTransmission(float ftTransmission);//取值 0.00~1.00
	void SetHumidity(unsigned char humidity); //湿度取值 0~100
	int   GetStallInfo(LENS_STALL_INFO* pLensStallInfo);		//获取档位信息

	void ShowMarkers(char param);
	void ClearMarkers();
	void DrawPoint(int index,int pt[2]);
	void DrawLine(int index,int startPt[2], int endPt[2]);
	void DrawRect(int index,int topLeft[2], int bottomRight[2]);

	void MotionDetection();
	void RegionalInvasion();
	void CrossLineAlarm();
	void ClearIntelligentAnalysisMarkers();
	void DisplayIntelligentAnalysis(bool bShow);
	void EnableIntelligentAnalysis(bool enable);
	void SaveIntelligentAnalysis();
	void GetIntelligentAnalysis();

	void FtpManual();			//仅仅支持定制版本
	void FtpAuto();				//仅仅支持定制版本
	void FtpClose();			//仅仅支持定制版本

	void SetDspHighTempAlarm(DSP_CONFIG_PARA_HT_ALARM* dspParams);
	void GetDspConfig_HighTempAlarm(DSP_CONFIG_PARA_HT_ALARM* pConfig);

	bool GetDspCrossInfo(DSP_CONFIG_PARA_CROSS_INFO* dspCross);
	void SetDspCrossInfo(DSP_CONFIG_PARA_CROSS_INFO* dspCross );

	bool GetDspImageSettings(DSP_CONFIG_PARA_IMAGE* pDspImageSettings);
	void SetDspImageSettings(DSP_CONFIG_PARA_IMAGE* pDspImageSettings);

	void RefreshDeviceInfo();
	void RefreshDspDeviceInfo();
	void SaveToFlash();		//测温参数保存到FLASH

	void RebootCamera();	

	void SetGrayAlarmMultiObjects(GrayAlarmMultiObjects* pGrayAlarmMultiObjects);

	void SendSerialData(char* data , int len);

	void ShowOsd(Osd_Config_State* showData);

	void GetOsd(structOsdInfo* osdInfo);

	void SetImageAlarm(AlarmInfo* pAlarmInfo);
	void SetObjAlarm(AlarmInfo* pAlarmInfo);

	void SetAgcData(int enAgcMode, float ftHistMaxTemper, float ftHistMinTemper);


	void SetBmpBrightness(int value);
	void 	SetBmpContrast(int value);

	void SetAbsFocusPos(unsigned short value);
	void SetAbsZoomPos(unsigned short value);
	void GetAbsFocusPos(FocusPosition* pFocusPosition);
	void GetAbsZoomPos(FocusPosition* pFocusPosition);

	void SetUserData(unsigned int value);
	void GetUserData(unsigned int* value);
	void SetObserveMode(char mode);

	void SetAutoZero(bool state);
	void SetAutoImage(int modetype);

	void SetAlarmShield(PTZ_ALARMSHIELD_t* data);

	void GetAlarmShield();

	void GetTempDataNum(TempDataNum* tempDataNum);

	JpgPlusData* GetJpgPlusData();   //获取JPG PLUS

	float GetLastErrorTime();

};

typedef ChannelControl SctDeviceControl ;

///////////////////////////
#endif
