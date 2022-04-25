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
	HChannel _hChanne1;     //�豸���
	OpenParams _openParams; //���Ӳ���

	static bool s_bIniFlag; //��ʼ����־

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
	static void IniEnvironment(); //��ʼ��SDK����
	static void QuitEnvironment(); //�뿪SDK����
	bool ConnectIp(char* ip, int wndIndex); //�������
	void Disconnect();//�Ͽ�����
	bool IsRunning();  //SDK�̵߳����Ƿ���������
	bool IsTcpConnected();//�豸�Ƿ�����

	FrameInfo* GetBGRData();		//��ȡλͼ���ݣ����Ƽ�ʹ��
	FrameInfo* GetJPGData();		//��ȡJPG���ݣ��Ƽ�
	FrameInfoFt* GetTempData()	;//��ȡ�¶�֡, �Ƚ�ռ��CPU����������25fps֡��ʵʱ��ȡ��
	FrameInfo* GetRawFrame();	//��ȡԭʼ֡
	MtImgInfo* GetMtImgInfo();
	PointInfo* GetPointInfo(int index);
	LineInfo* GetLineInfo(int index);
	RectInfo* GetRectInfo(int index);
	MarkersInfo* GetAllMarkersInfo();

	void DoZero();//����
	void DoStallUp();
	void FocusNear();//����
	void FocusFar();//Զ��
	void FocusStop();	//ֹͣ�۽�
	//�䱶
	void Zoom(VIRTUAL_KEYBOARD * virtual_keyboard); //�䱶

	void ZoomIn();
	void ZoomOut();
	void ZoomStop();


	void AutoFocus();//�Զ��۽�
	void SetCurrentPalette(int index);//����α��
	void SetDspImageMode(int index);//����ͼ��ģʽ
	void SetRawFps(int nfps);//����֡��

	void SetVideoFps(int nfps);//������Ƶ֡��

	void SetCameraStall(char sel );		 //���������λ
	void SetCameraLens(unsigned char sel); //���þ�ͷ���
	void SetEmissivity(float ftEmissivity);//ȡֵ 0.00~1.00
	void SetBKTemp(float ftBKTemp);	//���϶�
	void SetDistance(unsigned int distance);	//��λ������
	void SetReviseTemper(float ftReviseTemper);//���϶�
	void SetTransmission(float ftTransmission);//ȡֵ 0.00~1.00
	void SetHumidity(unsigned char humidity); //ʪ��ȡֵ 0~100
	int   GetStallInfo(LENS_STALL_INFO* pLensStallInfo);		//��ȡ��λ��Ϣ

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

	void FtpManual();			//����֧�ֶ��ư汾
	void FtpAuto();				//����֧�ֶ��ư汾
	void FtpClose();			//����֧�ֶ��ư汾

	void SetDspHighTempAlarm(DSP_CONFIG_PARA_HT_ALARM* dspParams);
	void GetDspConfig_HighTempAlarm(DSP_CONFIG_PARA_HT_ALARM* pConfig);

	bool GetDspCrossInfo(DSP_CONFIG_PARA_CROSS_INFO* dspCross);
	void SetDspCrossInfo(DSP_CONFIG_PARA_CROSS_INFO* dspCross );

	bool GetDspImageSettings(DSP_CONFIG_PARA_IMAGE* pDspImageSettings);
	void SetDspImageSettings(DSP_CONFIG_PARA_IMAGE* pDspImageSettings);

	void RefreshDeviceInfo();
	void RefreshDspDeviceInfo();
	void SaveToFlash();		//���²������浽FLASH

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

	JpgPlusData* GetJpgPlusData();   //��ȡJPG PLUS

	float GetLastErrorTime();

};

typedef ChannelControl SctDeviceControl ;

///////////////////////////
#endif
