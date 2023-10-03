#include "pch.h"
#include "ChannelControl.h"
#include "SLTMDlg.h"

bool ChannelControl::s_bIniFlag = false;

#define MAX_WIDTH (640)
#define MAX_HEIGHT (480)

ChannelControl::ChannelControl(void)
{
	if (!s_bIniFlag)
	{
		IniEnvironment();
		s_bIniFlag = true;
	}

	_hChanne1 = SCT_ChannelCreate(); 

	memset(_dstIp,0,sizeof(_dstIp));

	memset(&_jpgFrame, 0,  sizeof(FrameInfo));
	memset(&_tempFrame , 0, sizeof(FrameInfo));
	memset(&_bgrFrame , 0,   sizeof(FrameInfo));
	memset(&_markersInfo , 0,  sizeof(MarkersInfo));
	memset(&_rawFrame, 0,  sizeof(FrameInfo));

	_jpgFrame._pData = new char[1024*1024];
	_rawFrame._pData = new char[1024*1024];
	_tempFrame._pData = new float[MAX_WIDTH*MAX_HEIGHT];
	_bgrFrame._pData = new char[MAX_WIDTH*MAX_HEIGHT*3];

	_jpgPlusData.pData = new char[1024*1024*1.5];
}


ChannelControl::~ChannelControl(void)
{
	if(_hChanne1)
	{
		SCT_ChannelClose(_hChanne1);

		SCT_ChannelDestory(_hChanne1);
		_hChanne1 = 0;
	}

	if (_jpgFrame._pData)
	{
		delete _jpgFrame._pData;
		_jpgFrame._pData = 0;
	}

	if (_tempFrame._pData)
	{
		delete _tempFrame._pData;
		_tempFrame._pData = 0;
	}

	if (_bgrFrame._pData)
	{
		delete _bgrFrame._pData;
		_bgrFrame._pData = 0;
	}

	if (_rawFrame._pData)
	{
		delete _rawFrame._pData;
		_rawFrame._pData = 0;
	}
	if (_jpgPlusData.pData)
	{
		delete _jpgPlusData.pData;
		_jpgPlusData.pData = 0;
	}
}

char* ChannelControl::GetSDKVersion()
{
	return SCT_ChannelVersion();
}

void ChannelControl::IniEnvironment()
{
	SCT_ChannelInit();
}

void ChannelControl::QuitEnvironment()
{
	SCT_ChannelQuit();
}

void __stdcall CallbackGetEvent(HChannel hChannel,EventResult* pEventResult,GUI_MD_AREA_BCK* pGuiMdAreaBck)
{
	OutputDebugStringA("CallbackGetEvent\n");
	if (pEventResult)
	{
		if(pEventResult->IA_LINE_0)
		{
			OutputDebugStringA("Event Alarm ");
			OutputDebugStringA("line 0\n");

		}else if(pEventResult->IA_LINE_1)
		{
			OutputDebugStringA("Event Alarm ");
			OutputDebugStringA("line 1\n");
		}else if(pEventResult->IA_LINE_2)
		{
			OutputDebugStringA("Event Alarm ");
			OutputDebugStringA("line 2\n");

		}else if(pEventResult->IA_LINE_3)
		{
			OutputDebugStringA("Event Alarm ");
			OutputDebugStringA("line 3\n");
		}else if(pEventResult->IA_LINE_4)
		{
			OutputDebugStringA("Event Alarm ");
			OutputDebugStringA("line 4\n");
		}else if(pEventResult->IA_MD_0)
		{
			OutputDebugStringA("Event Alarm ");
			OutputDebugStringA("Motion Detection 0\n");
		}else if(pEventResult->IA_MD_1)
		{
			OutputDebugStringA("Event Alarm ");
			OutputDebugStringA("Motion Detection 1\n");
		}else if(pEventResult->IA_MD_2)
		{
			OutputDebugStringA("Event Alarm ");
			OutputDebugStringA("Motion Detection 2\n");
		}else if(pEventResult->IA_MD_3)
		{
			OutputDebugStringA("Event Alarm ");
			OutputDebugStringA("Motion Detection 3\n");
		}else if(pEventResult->IA_MD_4)
		{
			OutputDebugStringA("Event Alarm ");
			OutputDebugStringA("Motion Detection 4\n");
		}else if(pEventResult->IA_MD_INVADE_0)
		{
			OutputDebugStringA("Event Alarm ");
			OutputDebugStringA("Regional invasion 0\n");
		}else if(pEventResult->IA_MD_INVADE_1)
		{
			OutputDebugStringA("Event Alarm ");
			OutputDebugStringA("Regional invasion 1\n");
		}else if(pEventResult->IA_MD_INVADE_2)
		{
			OutputDebugStringA("Event Alarm ");
			OutputDebugStringA("Regional invasion 2\n");

		}else if(pEventResult->IA_MD_INVADE_3)
		{
			OutputDebugStringA("Event Alarm ");
			OutputDebugStringA("Regional invasion 3\n");

		}else if(pEventResult->IA_MD_INVADE_4)
		{
			OutputDebugStringA("Event Alarm ");
			OutputDebugStringA("Regional invasion 4\n");
		}
	}

	if (pGuiMdAreaBck)
	{
		for (int i = 0; i <MAX_EVENT_AREA ; ++i )
		{
			if (pGuiMdAreaBck->stEventMoveD.szEventArea[i].enable)
			{
				char data[256] = {0};
				sprintf(data, "Get Config:  MotionDetection= %d\n" , i);
				OutputDebugStringA(data);
			}
		}

		for (int i = 0; i <MAX_EVENT_AREA ; ++i )
		{
			if (pGuiMdAreaBck->stEventIntrudeD.szEventArea[i].enable)
			{
				char data[256] = {0};
				sprintf(data, "Get Config:  RegionalInvasion= %d\n" , i);
				OutputDebugStringA(data);
			}
		}

		for (int i = 0; i <MAX_EVENT_AREA ; ++i )
		{
			if (pGuiMdAreaBck->stEventIntrudeD.szEventArea[i].enable)
			{
				char data[256] = {0};
				sprintf(data, "Get Config:  CrossLineAlarm= %d\n" , i);
				OutputDebugStringA(data);
			}
		}
		
	}


}


 void __stdcall CallbackGetGrayAlarm(HChannel hChannel,
															DSP_GRAY_HIGHTEMPER_PROPERTY* pProperty, 
															DSP_TO_NET_GRAY_ALARM_INFO* pAlarmInfo,  
															int* alarmValue)
{
	/*CSctVCDemoDlg* pDlg = (CSctVCDemoDlg*)(AfxGetApp()->m_pMainWnd);

	if (pDlg && pDlg->GetSafeHwnd())
	{
	if (pProperty)
	{
	CString strText;
	strText.Format(_T("是否报警 %d\r\n高温点坐标(%d,%d)") , 
	pProperty->u32AlarmFlag ,
	pProperty->u16HighTempX,
	pProperty->u16HighTempY);

	pDlg->SetDspGrayHighTemperProperty(strText);
	}

	if (pAlarmInfo)
	{
	CString strText;
	for (int  i = 0; i < pAlarmInfo->u32AlarmRectValidNum && i <4; ++i)
	{
	CString strItem;
	strItem.Format(_T("报警坐标:  矩形 左上角(%d,%d)，右下角 (%d,%d)\n"), 
	pAlarmInfo->stTopLeftOutImg[i].u16X,
	pAlarmInfo->stTopLeftOutImg[i].u16Y,
	pAlarmInfo->stDownRightOutImg[i].u16X,
	pAlarmInfo->stDownRightOutImg[i].u16Y);
	strText  += strItem;
	}

	pDlg->SetGrayAlarmInfo(strText);
	}

	if (alarmValue)
	{
	CString strText;
	strText.Format(_T("发生报警: %d"),  *alarmValue);

	pDlg->SetGrayAlarmValue(strText);
	}

	}*/
}


 void  __stdcall  CallbackGetJpg(HChannel hChannel,char* pJpg, int nJpgLen)
 {
	 /* CSctVCDemoDlg* pDlg = (CSctVCDemoDlg*)(AfxGetApp()->m_pMainWnd);

	 if (pDlg && pJpg && nJpgLen > 0)
	 {
	 pDlg->JpgData2Gui(pJpg , nJpgLen);
	 }*/
 }

 void __stdcall CallbackGetDeviceSerialData(HChannel hChannel,char* pSerialData, int len, int serialType)
 {
	  /*CSctVCDemoDlg* pDlg = (CSctVCDemoDlg*)(AfxGetApp()->m_pMainWnd);

	  if (pDlg)
	  {
		  pDlg->SetSerialData(pSerialData ,len);

	  }*/
 }


 void __stdcall CallbackGetMtData(HChannel hChannel,MT_POINT* pMtPoint,int pointCount,MT_LINE* pMtLine,int lineCount,MT_RECT* pMtRect,int rectCount,MtImgInfo* mtImgInfo)
 {
	 //CSctVCDemoDlg* pDlg = (CSctVCDemoDlg*)(AfxGetApp()->m_pMainWnd);


	 ////SYSTEMTIME st;
	 ////CString strTime;
	 ////GetLocalTime(&st);
	 ////strTime.Format(L"%2d:%2d:%2d %3d  =%.1f\n ",st.wHour,st.wMinute,st.wSecond , st.wMilliseconds , mtImgInfo->ftMaxTemper);
	 ////OutputDebugString(strTime);


	 //if (pDlg)
	 //{
		// if (mtImgInfo)
		// {
		//	 AlarmInfo* pAlarmInfo = &(mtImgInfo->alarmInfo);

		//	pDlg->UpdateGlobalMt(pAlarmInfo);
		// }

		// if (pMtPoint)
		// {
		//	 for (int i = 0 ; i < pointCount; ++i)
		//	 {
		//		 AlarmInfo alarmInfo;
		//		 alarmInfo.enAlarmType  = pMtPoint[i].stAlarmType.enAlarmType;
		//		 
		//		 AlarmTypeStatusBit* pAlarmTypeStatusBit =    ( AlarmTypeStatusBit*)(&(  pMtPoint[i].stAlarmType.u32AlarmlFlag));
		//		 alarmInfo.alarmStatus = pAlarmTypeStatusBit->statusbit ;
		//		 
		//		alarmInfo.ftHighTemper =  pMtPoint[i].stAlarmType.ftHighTemper;
		//		alarmInfo.ftLowTemper =  pMtPoint[i].stAlarmType.ftLowTemper;

		//		pDlg->UpdatePointMt(i , &alarmInfo);
		//	 }
		// }

		// if (pMtRect)
		// {
		//	 for (int  i = 0; i < rectCount; ++i)
		//	 {
		//		 AlarmInfo alarmInfo;
		//		 alarmInfo.enAlarmType  = pMtRect[i].stAlarmType.enAlarmType;

		//		 AlarmTypeStatusBit* pAlarmTypeStatusBit =    ( AlarmTypeStatusBit*)(&(  pMtRect[i].stAlarmType.u32AlarmlFlag));
		//		 alarmInfo.alarmStatus = pAlarmTypeStatusBit->statusbit ;

		//		 alarmInfo.ftHighTemper =  pMtRect[i].stAlarmType.ftHighTemper;
		//		 alarmInfo.ftLowTemper =  pMtRect[i].stAlarmType.ftLowTemper;

		//		 pDlg->UpdateRectMt(i , &alarmInfo);


		//	 }
		// }
	 //}

 }



 int SaveBitmapToFile(int  width ,  int height, const WCHAR *   lpFileName,char *lpBuf) {
	 //DWORD dwWritten;
	 BITMAPFILEHEADER   bmfHdr;
	 BITMAPINFOHEADER   bi;

	 bi.biSize = sizeof(BITMAPINFOHEADER);
	 bi.biWidth= width;
	 bi.biHeight = -height;
	 bi.biPlanes = 1;
	 bi.biBitCount      = 24;
	 bi.biCompression   = 0;	//BI_RGB
	 bi.biSizeImage     = 0;
	 bi.biXPelsPerMeter = 0;
	 bi.biYPelsPerMeter = 0;
	 bi.biClrUsed       = 0;
	 bi.biClrImportant  = 0;

	 FILE* fd = NULL;
	 fd = _wfopen(lpFileName, L"wb+");

	 if (fd == 0)
	 {
		 return FALSE;
	 }
	 bmfHdr.bfType = 0x4D42; // "BM"
	 bmfHdr.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)+  width*height*3;
	 bmfHdr.bfReserved1 = 0;
	 bmfHdr.bfReserved2 = 0;
	 bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

	 fwrite((char*)&bmfHdr, sizeof(BITMAPFILEHEADER),1,fd);
	 fwrite((char *)&bi,sizeof(BITMAPINFOHEADER), 1,fd);
	 fwrite((char *)lpBuf,width*height*3,1, fd);

	 fclose(fd);
	 return true;
 }

 void __stdcall CallbackGetRGB0(HChannel hChannel,char* pRGB, int totalLen,int width, int height)
 {
	 if (pRGB)
	 {
		 CSLTMDlg* pDlg = (CSLTMDlg*)(AfxGetApp()->m_pMainWnd);
		 
		 if (pDlg)
		 {
			 pDlg->BmpData2Gui((unsigned char*)pRGB, width, height, 0);
			 //pDlg->BmpData2Gui0((unsigned char*)pRGB,width,height);
		 }
	 }
 }


 void __stdcall CallbackGetRGB1(HChannel hChannel,char* pRGB, int totalLen,int width, int height)
 {
	 if (pRGB)
	 {
		 CSLTMDlg* pDlg = (CSLTMDlg*)(AfxGetApp()->m_pMainWnd);
		 CPoint ptMaxTemp(0,0);
		 if (pDlg)
		 {
			 pDlg->BmpData2Gui((unsigned char*)pRGB, width, height, 1);
			 //pDlg->BmpData2Gui1((unsigned char*)pRGB,width,height);
		 }
	 }
 }


 void __stdcall CallbackGetRGB2(HChannel hChannel,char* pRGB, int totalLen,int width, int height)
 {
	 if (pRGB)
	 {
		 CSLTMDlg* pDlg = (CSLTMDlg*)(AfxGetApp()->m_pMainWnd);
		 CPoint ptMaxTemp(0,0);
		 if (pDlg)
		 {
			 pDlg->BmpData2Gui((unsigned char*)pRGB, width, height, 2);
			 //pDlg->BmpData2Gui2((unsigned char*)pRGB,width,height);
		 }
	 }
 }
 
 void __stdcall CallbackGetRectAlarm(HChannel,MT_RECT_ALARM * mtRectAlarm,int* alarmValue)
 {
	 /*CSLTMDlg* pDlg = (CSLTMDlg*)(AfxGetApp()->m_pMainWnd);
	 if (alarmValue)
	 {
	 CString strText;
	 strText.Format(_T("发生报警: %d"),  *alarmValue);

	 pDlg->SetGrayAlarmValue(strText);
	 }
	 if (mtRectAlarm)
	 {
	 CString csRectAlarmInfo;
	 csRectAlarmInfo.Format(_T("灰度最高点:(%d,%d)\r\n")
	 _T("最高灰度值:%d\r\n")
	 _T("报警框中心坐标):(%d,%d)\r\n")
	 _T("中心灰度值:%d\r\n"),
	 mtRectAlarm->stMaxPt.u16X,
	 mtRectAlarm->stMaxPt.u16Y,
	 mtRectAlarm->s16MaxGray,
	 mtRectAlarm->stRectCenterPoint.u16X,
	 mtRectAlarm->stRectCenterPoint.u16Y,
	 mtRectAlarm->s16RectCenterGray);
	 pDlg->fnGetRectAlarmStr(csRectAlarmInfo);
	 }*/
	// 	FocusPosition focusPosition;
	//focusPosition.bValid = 0;
	//focusPosition.nTimeoutMS = 3000;	//超时1秒
	//focusPosition.value = 0;

	//pDlg->_sctDeviceControl.GetAbsFocusPos(&focusPosition);

	//if (focusPosition.bValid)
	//{
	//	CString strValue;
	//	strValue.Format(_T("%d"), focusPosition.value);

	//	pDlg->m_editAbsFocusPos.SetWindowText(strValue);
	//}
	
 }
 //设备句柄，   位图数据指针，  位图数据长度，   全帧温度数组，   全帧灰度数组，   探测器宽，  探测器高
void __stdcall  CallbackGetInfraredData(HChannel hChannel ,  unsigned char* pBGR, int bgrLen,  float* tempMatrix ,  short* grayMatrix ,  int width, int height)
{
	  
	// OutputDebugStringA("GetInfraredData\n");
	  
#if 0
	SaveBitmapToFile(width, height, L"d:\\output.bmp", (char *)pBGR);

	float maxTemp = *tempMatrix;
	float minTemp = *tempMatrix;
	short maxGray = *grayMatrix;
	short minGray = *grayMatrix;
	CPoint ptMaxTemp(0, 0);
	float* pCurTempMatrix = tempMatrix;
	short* pCurGrayMatrix = grayMatrix;

	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			if (*pCurTempMatrix > maxTemp)
			{
				maxTemp = *pCurTempMatrix;
				ptMaxTemp = CPoint(i, j);
			}

			if (*pCurTempMatrix < minTemp)
			{
				minTemp = *pCurTempMatrix;
			}
			if (*pCurGrayMatrix > maxGray)
			{
				maxGray = *pCurGrayMatrix;
			}
			if (*pCurGrayMatrix < minGray)
			{
				minGray = *pCurGrayMatrix;
			}
			++pCurTempMatrix;
			++pCurGrayMatrix;
		}
	}
#endif
	CSLTMDlg* pDlg = (CSLTMDlg*)(AfxGetApp()->m_pMainWnd);

	if (pDlg)
	{
#if 0

		static CString strOutput;
		strOutput.Format(_T("InfraredData maxTemp:%.1f,minTemp:%.1f,maxGray:%d,minGray:%d,width:%d,height:%d,MaxTemp Point(%d,%d)\n"),
			maxTemp, minTemp, maxGray, minGray, width, height,
			ptMaxTemp.x, ptMaxTemp.y);

		OutputDebugString(strOutput);

#endif
		pDlg->DataTransfer(pBGR, bgrLen, tempMatrix, width, height, 0);
		// pDlg->SetInfraredData(strOutput);
		// pDlg->GetDlgItem(IDC_STATIC_LOG)->SetWindowText(strOutput);
		// pDlg->BmpData2Gui0( pBGR, width, height);
	}
}

//设备句柄，   位图数据指针，  位图数据长度，   全帧温度数组，   全帧灰度数组，   探测器宽，  探测器高
void __stdcall  CallbackGetInfraredData1(HChannel hChannel, unsigned char* pBGR, int bgrLen, float* tempMatrix, short* grayMatrix, int width, int height)
{

	// OutputDebugStringA("GetInfraredData\n");

#if 0
	SaveBitmapToFile(width, height, L"d:\\output.bmp", (char *)pBGR);
#endif

	float maxTemp = *tempMatrix;
	float minTemp = *tempMatrix;
	short maxGray = *grayMatrix;
	short minGray = *grayMatrix;
	CPoint ptMaxTemp(0, 0);
	float* pCurTempMatrix = tempMatrix;
	short* pCurGrayMatrix = grayMatrix;

	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			if (*pCurTempMatrix > maxTemp)
			{
				maxTemp = *pCurTempMatrix;
				ptMaxTemp = CPoint(i, j);
			}

			if (*pCurTempMatrix < minTemp)
			{
				minTemp = *pCurTempMatrix;

			}

			if (*pCurGrayMatrix > maxGray)
			{
				maxGray = *pCurGrayMatrix;

			}

			if (*pCurGrayMatrix < minGray)
			{
				minGray = *pCurGrayMatrix;

			}

			++pCurTempMatrix;
			++pCurGrayMatrix;
		}
	}

	CSLTMDlg* pDlg = (CSLTMDlg*)(AfxGetApp()->m_pMainWnd);

	if (pDlg)
	{
#if 0

		static CString strOutput;
		strOutput.Format(_T("InfraredData maxTemp:%.1f,minTemp:%.1f,maxGray:%d,minGray:%d,width:%d,height:%d,MaxTemp Point(%d,%d)\n"),
			maxTemp, minTemp, maxGray, minGray, width, height,
			ptMaxTemp.x, ptMaxTemp.y);

		OutputDebugString(strOutput);

#endif
		pDlg->DataTransfer(pBGR, bgrLen, tempMatrix, width, height, 1);
		// pDlg->SetInfraredData(strOutput);
		// pDlg->GetDlgItem(IDC_STATIC_LOG)->SetWindowText(strOutput);
		// pDlg->BmpData2Gui0( pBGR, width, height);
	}
}


//设备句柄，   位图数据指针，  位图数据长度，   全帧温度数组，   全帧灰度数组，   探测器宽，  探测器高
void __stdcall  CallbackGetInfraredData2(HChannel hChannel, unsigned char* pBGR, int bgrLen, float* tempMatrix, short* grayMatrix, int width, int height)
{

	// OutputDebugStringA("GetInfraredData\n");

#if 0
	SaveBitmapToFile(width, height, L"d:\\output.bmp", (char *)pBGR);
#endif

	float maxTemp = *tempMatrix;
	float minTemp = *tempMatrix;
	short maxGray = *grayMatrix;
	short minGray = *grayMatrix;
	CPoint ptMaxTemp(0, 0);
	float* pCurTempMatrix = tempMatrix;
	short* pCurGrayMatrix = grayMatrix;

	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			if (*pCurTempMatrix > maxTemp)
			{
				maxTemp = *pCurTempMatrix;
				ptMaxTemp = CPoint(i, j);
			}

			if (*pCurTempMatrix < minTemp)
			{
				minTemp = *pCurTempMatrix;

			}

			if (*pCurGrayMatrix > maxGray)
			{
				maxGray = *pCurGrayMatrix;

			}

			if (*pCurGrayMatrix < minGray)
			{
				minGray = *pCurGrayMatrix;

			}

			++pCurTempMatrix;
			++pCurGrayMatrix;
		}
	}

	CSLTMDlg* pDlg = (CSLTMDlg*)(AfxGetApp()->m_pMainWnd);

	if (pDlg)
	{
#if 0

		static CString strOutput;
		strOutput.Format(_T("InfraredData maxTemp:%.1f,minTemp:%.1f,maxGray:%d,minGray:%d,width:%d,height:%d,MaxTemp Point(%d,%d)\n"),
			maxTemp, minTemp, maxGray, minGray, width, height,
			ptMaxTemp.x, ptMaxTemp.y);

		OutputDebugString(strOutput);

#endif
		pDlg->DataTransfer(pBGR, bgrLen, tempMatrix, width, height, 2);
		// pDlg->SetInfraredData(strOutput);
		// pDlg->GetDlgItem(IDC_STATIC_LOG)->SetWindowText(strOutput);
		// pDlg->BmpData2Gui0( pBGR, width, height);
	}
}



bool ChannelControl::ConnectIp(char* ip, int wndIndex)
{

	//return 0;

	//300毫秒之内，判断设备是否在线，
	
#if 0
	if(!IsOnline(ip, 4321, 300))
	{
		char data[256] = {0};
		sprintf(data,"Camera ip:%s Decive Is Not Online\n", ip);
		OutputDebugStringA(data);
		return false;
	}

#endif
	

	memset(_dstIp,0,sizeof(_dstIp));
	strcpy(_dstIp,ip);

	if(_hChanne1 == 0)
	{
		OutputDebugStringA("SCT ChannelControl  ConnectIp failure :  0 == _hChanne1\n");
		return false;
	}

	int runState = 0;
	SCT_ChannelQuery(_hChanne1,QT_RunState,&runState);
	if(runState == 1)
	{
		OutputDebugStringA("SCT ChannelControl  ConnectIp failure :  Is Running\n");
		return false;
	}

	OpenParams openParams;
	strcpy((char*)openParams._ipV4, ip);
	openParams._iniFps = 0;
	openParams._destPort = 4321;
	openParams._encodeJpg = 0;  //1: Enable Trans Jpg Stream;   0:Disable Trans Jpg Stream
	openParams._jpgSize = JPEG_SIZE_720x576;
	openParams._bAutoReconnect = 1;
	openParams._heartBreakTimeOut = 10;

	openParams._rtsp2Jpg = 1;
	memset(openParams._userName,0,sizeof(openParams._userName));
	memset(openParams._passWord,0,sizeof(openParams._passWord));
	strcpy(openParams._userName,"admin");
	strcpy(openParams._passWord,"admin123");
	openParams._rtspPort = 554;

	openParams._rgbStream = 0;

	//////////////////////////////
	openParams._bCapabilitySet = 1;
	openParams._jpgStream._bOpen = 1;
	openParams._jpgStream._bOverTcp = 1;			//1:TCP  0:UDP
	openParams._jpgStream._nIniFPS = 10;	//这个作废了		，内部改为RTSP获取

	//////////////////////////////
	openParams._tempStream._bOpen = 1;
	openParams._tempStream._bOverTcp = 1;			//1:TCP  0:UDP
	openParams._tempStream._nIniFPS = 10;			// 这个是实时测温数据，点、线、矩形和参数，测温必须打开获取

	//////////////////////////////
	openParams._rawStream._bOpen = 1;
	openParams._rawStream._bOverTcp = 1;			//1:TCP  0:UDP
	openParams._rawStream._nIniFPS = 1;//这个是原始帧帧率，对应的 CallbackGetInfraredData
	//超时时间
	openParams._ConnectTimeOut = 3000;
	//SCT_ChannelSetOpt(  _hChanne1 ,   OT_SetFunGetEvent ,   CallbackGetEvent);
	//SCT_ChannelSetOpt(  _hChanne1 ,   OT_SetFunGetGrayAlarm ,   CallbackGetGrayAlarm);
	//SCT_ChannelSetOpt(  _hChanne1 ,   OT_SetFunGetJpg,   CallbackGetJpg );
	//SCT_ChannelSetOpt(  _hChanne1,    OT_SetFunDeviceSerialData, CallbackGetDeviceSerialData);
	//SCT_ChannelSetOpt(  _hChanne1 ,   OT_SetFunGetMtData,   CallbackGetMtData );
	//SCT_ChannelSetOpt(  _hChanne1 ,  OT_SetFunInfraredData,  CallbackGetInfraredData);
	//SCT_ChannelSetOpt(  _hChanne1,   OT_SetRectAlarm,CallbackGetRectAlarm);
	//SCT_ChannelSetOpt(  _hChanne1,   OT_SetFunGetRGB,CallbackGetRGB);
	
	if (0 == wndIndex)
	{
		SCT_ChannelSetOpt(_hChanne1, OT_SetFunGetOpencvRGB, CallbackGetRGB0);
		SCT_ChannelSetOpt(_hChanne1, OT_SetFunInfraredData, CallbackGetInfraredData);
	}else if (1 == wndIndex) {
		SCT_ChannelSetOpt(_hChanne1, OT_SetFunGetOpencvRGB, CallbackGetRGB1);
		SCT_ChannelSetOpt(_hChanne1, OT_SetFunInfraredData, CallbackGetInfraredData1);
	}else if (2 == wndIndex) {
		SCT_ChannelSetOpt(_hChanne1, OT_SetFunGetOpencvRGB, CallbackGetRGB2);
		SCT_ChannelSetOpt(_hChanne1, OT_SetFunInfraredData, CallbackGetInfraredData2);
	}
	

	
	
	int i = sizeof(JPEG_SIZE);
	int ret = SCT_ChannelOpen(_hChanne1,&openParams);

	memcpy(&_openParams, &openParams , sizeof(OpenParams));

	return ret;
}

void ChannelControl::Disconnect()
{
	if(_hChanne1)
	{
		SCT_ChannelClose(_hChanne1);
	}
}

bool ChannelControl::IsRunning()
{
	int runState = 0;
	SCT_ChannelQuery(_hChanne1,QT_RunState,&runState);
	return runState;
}


bool ChannelControl::IsTcpConnected()
{
	if(_hChanne1)
	{
		int nConnectState = 0;
		SCT_ChannelQuery(_hChanne1,QT_ConnectState,&nConnectState);

		return nConnectState == 1;
	}

	return false;
}
float ChannelControl::GetLastErrorTime()
{
	if(_hChanne1)
	{
		float time = 0;
		SCT_ChannelQuery(_hChanne1,QT_LastHeartBreakTime,&time);
		return time;
	}
	return 0;
}
FrameInfo* ChannelControl::GetBGRData()
{
	if (IsRunning())
	{
		if (SCT_ChannelRead(_hChanne1,RTR_GetBGRFrame,&_bgrFrame) == 1)
		{
			return &_bgrFrame;
		}
	}
	return 0;
}

FrameInfo* ChannelControl::GetJPGData()
{
	if (IsRunning())
	{
		if (SCT_ChannelRead(_hChanne1,RTR_GetJpgFrame,&_jpgFrame) == 1)
		{
			return &_jpgFrame;
		}
	}
	return 0;
}

FrameInfoFt* ChannelControl::GetTempData()
{
	if (IsRunning())
	{
		if (SCT_ChannelRead(_hChanne1,RTR_GetTempFrame,&_tempFrame) == 1)
		{
			return &_tempFrame;
		}
	}
	return 0;
}

FrameInfo* ChannelControl::GetRawFrame()
{
	if (IsRunning())
	{
		if (SCT_ChannelRead(_hChanne1,RTR_GetRawFrame ,&_rawFrame) == 1)
		{
			return &_rawFrame;
		}
	}
	return 0;
}

MtImgInfo* ChannelControl::GetMtImgInfo()
{
	if (IsRunning())
	{
		if (SCT_ChannelRead(_hChanne1,RTR_GetMtImgInfo,&_mtImgInfo) == 1)
		{
			return &_mtImgInfo;
		}
	}
	return 0;
}

PointInfo* ChannelControl::GetPointInfo(int index)
{
	if (IsRunning())
	{
		_pointInfo.index = index;
		if (SCT_ChannelRead(_hChanne1,RTR_GetPointInfo,&_pointInfo) == 1)
		{
			return &_pointInfo;
		}
	}
	return 0;
}

LineInfo* ChannelControl::GetLineInfo(int index)
{
	if (IsRunning())
	{
		_lineInfo.index = index;
		if (SCT_ChannelRead(_hChanne1,RTR_GetLineInfo,&_lineInfo) == 1)
		{
			return &_lineInfo;
		}
	}
	return 0;
}

RectInfo* ChannelControl::GetRectInfo(int index)
{
	if (IsRunning())
	{
		_rectInfo.index = index;
		if (SCT_ChannelRead(_hChanne1,RTR_GetRectInfo,&_rectInfo) == 1)
		{
			return &_rectInfo;
		}
	}
	return 0;
}

MarkersInfo* ChannelControl::GetAllMarkersInfo()
{
	MarkersInfo* ret = 0;

	if (IsRunning())
	{
		if (SCT_ChannelRead(_hChanne1 ,RTR_GetAllMarkersInfo, &_markersInfo ) == 1)
		{
			ret = &_markersInfo;
		}
	}

	return ret;
}

void ChannelControl::DoZero()
{
	if( IsTcpConnected() )
	{
		SCT_ChannelWrite(_hChanne1, RTW_DoZero,0);
	}
}

void ChannelControl::DoStallUp()
{
	if( IsTcpConnected() )
	{
		SCT_ChannelWrite(_hChanne1, RTW_DSP_SetStallShutterUp, 0);
	}
}


void ChannelControl::FocusNear()
{
	if( IsTcpConnected() )
	{
		SCT_ChannelWrite(_hChanne1,RTW_SetFocusNear,0 );
	}
}

void ChannelControl::FocusFar()
{
	if( IsTcpConnected() )
	{
		SCT_ChannelWrite(_hChanne1,RTW_SetFocusFar,0 );
	}
}

void ChannelControl::FocusStop()
{
	if( IsTcpConnected() )
	{
		SCT_ChannelWrite(_hChanne1,RTW_SetFocusStop,0 );
	}
}

void ChannelControl::Zoom(VIRTUAL_KEYBOARD * virtual_keyboard)
{
	SCT_ChannelWrite(_hChanne1,RTW_SendVirtualKey,virtual_keyboard);
}

void ChannelControl::ZoomIn()
{
	if( IsTcpConnected() )
	{
		SCT_ChannelWrite(_hChanne1,RTW_ZoomIn,0 );
	}
}

void ChannelControl::ZoomOut()
{
	if( IsTcpConnected() )
	{
		SCT_ChannelWrite(_hChanne1,RTW_ZoomOut,0 );
	}
}

void ChannelControl::ZoomStop()
{
	if( IsTcpConnected() )
	{
		SCT_ChannelWrite(_hChanne1,RTW_ZoomStop,0 );
	}
}

void ChannelControl::AutoFocus()
{
	if( IsTcpConnected() )
	{
		SCT_ChannelWrite(_hChanne1,RTW_AutoFocus,0 );
	}
}

void ChannelControl::SetCurrentPalette(int index)
{
	if( IsTcpConnected() &&  _hChanne1)
	{
		unsigned char palette = index;
		SCT_ChannelWrite(_hChanne1, RTW_SetColorRule, &palette);
	}


}
void ChannelControl::SetDspImageMode(int index)
{
	if( IsTcpConnected() )
	{
		SCT_ChannelWrite(_hChanne1, RTW_DSP_SetImgMode,&index);
	}
}


void ChannelControl::SetEmissivity(float ftEmissivity)
{
	if( IsTcpConnected() )
	{
		SCT_ChannelWrite(_hChanne1,RTW_SetEmissivity, &ftEmissivity);
	}
}

void ChannelControl::SetBKTemp(float ftBKTemp)
{
	if( IsTcpConnected() )
	{
		SCT_ChannelWrite(_hChanne1 , RTW_SetBKTemp,  &ftBKTemp);
	}

}

void ChannelControl::SetDistance(unsigned int distance)
{
	if( IsTcpConnected() )
	{
		SCT_ChannelWrite(_hChanne1 , RTW_SetDistance,  &distance);
	}
}

void ChannelControl::SetReviseTemper(float ftReviseTemper)
{
	if( IsTcpConnected() )
	{
		SCT_ChannelWrite(_hChanne1,  RTW_SetReviseTemper,&ftReviseTemper );
	}

}

void ChannelControl::SetTransmission(float ftTransmission)
{
	if( IsTcpConnected() )
	{
		SCT_ChannelWrite(_hChanne1,  RTW_SetTransmission , &ftTransmission );
	}

}

void ChannelControl::SetHumidity(unsigned char humidity)
{
	if( IsTcpConnected() )
	{
		SCT_ChannelWrite(_hChanne1 , RTW_SetHumidity,  &humidity);
	}
}

int ChannelControl::GetStallInfo(LENS_STALL_INFO* pLensStallInfo)
{
	int ret = 0;
	if( IsTcpConnected() )
	{
		ret = SCT_ChannelRead(_hChanne1 ,  RTR_GetLensStallInfo,   pLensStallInfo );
	}
	return ret;
}

void ChannelControl::ShowMarkers(char param)
{
	if( IsTcpConnected() )
	{
		SCT_ChannelWrite(_hChanne1, RTW_EnableDrawingMarkersInDsp, &param);
	}
}

void ChannelControl::ClearMarkers()
{
	if( IsTcpConnected() )
	{
		SCT_ChannelWrite(_hChanne1 , RTW_ClearAllMtObj,  0);
	}
}

void ChannelControl::DrawPoint(int index, int pt[2])
{
	if( IsTcpConnected() )
	{
		PointPosition ptPosition;
		ptPosition.index = index;
		ptPosition.zsPoint.u16X = pt[0];
		ptPosition.zsPoint.u16Y = pt[1];
		ptPosition.enable = 1;
		SCT_ChannelWrite(_hChanne1, RTW_SetPointPosition,&ptPosition);
	}
}

void ChannelControl::DrawLine(int index, int startPt[2], int endPt[2])
{
	if( IsTcpConnected() )
	{
		LinePosition liPosition;
		liPosition.index = index;
		liPosition.stStartPt.u16X = startPt[0];
		liPosition.stStartPt.u16Y = startPt[1];

		liPosition.stEndPt.u16X = endPt[0];
		liPosition.stEndPt.u16Y = endPt[1];

		liPosition.enable = 1;
		SCT_ChannelWrite(_hChanne1, RTW_SetLineposition,&liPosition);
	}
}

void ChannelControl::DrawRect(int index, int topLeft[2], int bottomRight[2])
{
	if( IsTcpConnected() )
	{
		RectPosition bxPosition;
		bxPosition.index = index;
		bxPosition.stTopLeft.u16X = topLeft[0];
		bxPosition.stTopLeft.u16Y = topLeft[1];

		bxPosition.stBottomRight.u16X = bottomRight[0];
		bxPosition.stBottomRight.u16Y = bottomRight[1];

		bxPosition.enable = 1;
		SCT_ChannelWrite(_hChanne1, RTW_SetRectposition,&bxPosition);
	}
}

void ChannelControl::MotionDetection()
{
	EventConfig szEventConfig;

	memset(&szEventConfig,0,sizeof(EventConfig));

	//  0跨线报警 1移动侦测 2区域入侵
	szEventConfig.eventType = 1;

	szEventConfig.show = 1;				   //  显示 0隐藏 1显示
	szEventConfig.nAreaCount = 1;	   //几个区域

	szEventConfig.enable = ((szEventConfig.nAreaCount > 0) ? 1:0);         //  开关 0关 1开

	for(int i = 0; i <szEventConfig.nAreaCount; ++i )
	{

		//////////////////////////////
		szEventConfig.szEventArea[i].direction = 0;				// 0: 1: ； A-B A->B B->A
		szEventConfig.szEventArea[i].enable = 1;					// 开关 0关 1开
		szEventConfig.szEventArea[i].sensitivity = 50;			 // 灵敏度 0~100
		szEventConfig.szEventArea[i].reserve = 0;				//
		szEventConfig.szEventArea[i].reserve2 = 0;
		szEventConfig.szEventArea[i].ptCount =  4;              //4点包围

		szEventConfig.szEventArea[i].pt[0].s16X = 50;
		szEventConfig.szEventArea[i].pt[0].s16Y = 50;

		szEventConfig.szEventArea[i].pt[1].s16X =  100;
		szEventConfig.szEventArea[i].pt[1].s16Y =  50;

		szEventConfig.szEventArea[i].pt[2].s16X = 100;
		szEventConfig.szEventArea[i].pt[2].s16Y = 200;

		szEventConfig.szEventArea[i].pt[3].s16X = 50;
		szEventConfig.szEventArea[i].pt[3].s16Y = 200;
	}
	
	SCT_ChannelWrite(_hChanne1, RTW_SetIntelligentAnalysis, &szEventConfig);


}

void ChannelControl::RegionalInvasion()
{
	EventConfig szEventConfig;

	memset(&szEventConfig,0,sizeof(EventConfig));

	//  0跨线报警 1移动侦测 2区域入侵
	szEventConfig.eventType = 2;

	szEventConfig.show = 1;				   //  显示 0隐藏 1显示
	szEventConfig.nAreaCount = 1;	   //几个区域

	szEventConfig.enable = ((szEventConfig.nAreaCount > 0) ? 1:0);         //  开关 0关 1开

	for(int i = 0; i <szEventConfig.nAreaCount; ++i )
	{

		//////////////////////////////
		szEventConfig.szEventArea[i].direction = 0;				// 0: 1: ； A-B A->B B->A
		szEventConfig.szEventArea[i].enable = 1;					// 开关 0关 1开
		szEventConfig.szEventArea[i].sensitivity = 50;			 // 灵敏度 0~100
		szEventConfig.szEventArea[i].reserve = 0;				//
		szEventConfig.szEventArea[i].reserve2 = 0;
		szEventConfig.szEventArea[i].ptCount =  4;              //4点包围

		szEventConfig.szEventArea[i].pt[0].s16X = 50;
		szEventConfig.szEventArea[i].pt[0].s16Y = 50;

		szEventConfig.szEventArea[i].pt[1].s16X =  300;
		szEventConfig.szEventArea[i].pt[1].s16Y =  50;

		szEventConfig.szEventArea[i].pt[2].s16X = 300;
		szEventConfig.szEventArea[i].pt[2].s16Y = 200;

		szEventConfig.szEventArea[i].pt[3].s16X = 50;
		szEventConfig.szEventArea[i].pt[3].s16Y = 200;
	}

	SCT_ChannelWrite(_hChanne1, RTW_SetIntelligentAnalysis, &szEventConfig);


}

void ChannelControl::CrossLineAlarm()
{
	EventConfig szEventConfig;

	memset(&szEventConfig,0,sizeof(EventConfig));

	//  0跨线报警 1移动侦测 2区域入侵
	szEventConfig.eventType = 0;

	szEventConfig.show = 1;				   //  显示 0隐藏 1显示
	szEventConfig.nAreaCount = 1;	   //几个区域

	szEventConfig.enable = ((szEventConfig.nAreaCount > 0) ? 1:0);         //  开关 0关 1开

	for(int i = 0; i <szEventConfig.nAreaCount; ++i )
	{

		//////////////////////////////
		szEventConfig.szEventArea[i].direction = 0;				// 0: 1: ； A-B A->B B->A
		szEventConfig.szEventArea[i].enable = 1;					// 开关 0关 1开
		szEventConfig.szEventArea[i].sensitivity = 50;			 // 灵敏度 0~100
		szEventConfig.szEventArea[i].reserve = 0;				//
		szEventConfig.szEventArea[i].reserve2 = 0;
		szEventConfig.szEventArea[i].ptCount =  4;              //4点包围

		szEventConfig.szEventArea[i].pt[0].s16X = 50;
		szEventConfig.szEventArea[i].pt[0].s16Y = 50;

		szEventConfig.szEventArea[i].pt[1].s16X =  300;
		szEventConfig.szEventArea[i].pt[1].s16Y =  200;
	}

	SCT_ChannelWrite(_hChanne1, RTW_SetIntelligentAnalysis, &szEventConfig);
}

void ChannelControl::ClearIntelligentAnalysisMarkers()
{
	EventConfig szEventConfig;

	memset(&szEventConfig,0,sizeof(EventConfig));

	//  0跨线报警 1移动侦测 2区域入侵
	szEventConfig.eventType = 0;

	szEventConfig.show = 1;				   //  显示 0隐藏 1显示
	szEventConfig.nAreaCount = 0;	   //几个区域
	szEventConfig.enable = 0;         //  开关 0关 1开

	szEventConfig.eventType = 0;
	SCT_ChannelWrite(_hChanne1, RTW_SetIntelligentAnalysis, &szEventConfig);

	szEventConfig.eventType = 1;
	SCT_ChannelWrite(_hChanne1, RTW_SetIntelligentAnalysis, &szEventConfig);

	szEventConfig.eventType = 2;
	SCT_ChannelWrite(_hChanne1, RTW_SetIntelligentAnalysis, &szEventConfig);
}

void ChannelControl::DisplayIntelligentAnalysis(bool bShow)
{
	int data = bShow ? 1:0;

	SCT_ChannelWrite(_hChanne1, RTW_ShowIntelligentAnalysis, &data);
}

void ChannelControl::EnableIntelligentAnalysis(bool enable)
{
	int data = enable ? 1:0;
	SCT_ChannelWrite(_hChanne1, RTW_EnableIntelligentAnalysis, &data);
}

void ChannelControl::SaveIntelligentAnalysis()
{
	SCT_ChannelWrite(_hChanne1, RTW_SaveIntelligentAnalysis, 0);
}

void ChannelControl::GetIntelligentAnalysis()
{
	SCT_ChannelWrite(_hChanne1, RTW_TransIntelligentAnalysis, 0);
}

void ChannelControl::FtpManual()
{
	FtpConfig ftpConfig;
	memset(&ftpConfig, 0, sizeof(FtpConfig));
	ftpConfig._bOpen = 1;						//1 : 开启FTP    ,  0: 关闭FTP
	ftpConfig._downLoadAuto = 0;		//1 : 自动下载  ,   0:手动下载
	ftpConfig._downLoadJpg = 1;			//1 : 下载JPG ,      0: 停止下载
	ftpConfig._downLoadMp4 = 1;		//1 : 下载MP4 ,      0: 停止下载
	sprintf(ftpConfig._localDir,"%s","D:\\FTP");		//PC端下载目录
	ftpConfig._maxSpeed = 5*1024;		//带宽限速 5M/S

	SCT_ChannelPerform(_hChanne1, PT_RunFtpDownLoad, &ftpConfig);
}

void ChannelControl::FtpAuto()
{
	FtpConfig ftpConfig;
	memset(&ftpConfig, 0, sizeof(FtpConfig));
	ftpConfig._bOpen = 1;						//1 : 开启FTP    ,  0: 关闭FTP
	ftpConfig._downLoadAuto = 1;		//1 : 自动下载  0:手动下载
	ftpConfig._downLoadJpg = 1;			//1 : 下载JPG ,      0: 停止下载
	ftpConfig._downLoadMp4 = 1;		//1 : 下载MP4 ,      0: 停止下载
	sprintf(ftpConfig._localDir,"%s","D:\\FTP");		//PC端路径
	ftpConfig._maxSpeed = 5*1024;		//带宽限速 5M/S

	SCT_ChannelPerform(_hChanne1, PT_RunFtpDownLoad, &ftpConfig);
}

void ChannelControl::FtpClose()
{
	FtpConfig ftpConfig;
	memset(&ftpConfig, 0, sizeof(FtpConfig));
	ftpConfig._bOpen = 0;						//1 : 开启FTP    ,  0: 关闭FTP

	SCT_ChannelPerform(_hChanne1, PT_RunFtpDownLoad, &ftpConfig);
}

void ChannelControl::SetDspHighTempAlarm(DSP_CONFIG_PARA_HT_ALARM* dspParams)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelWrite(_hChanne1,  RTW_SetDspHighTempAlarm,  dspParams);
	}
}

void ChannelControl::GetDspConfig_HighTempAlarm(DSP_CONFIG_PARA_HT_ALARM* pConfig)
{
	if( IsTcpConnected() && _hChanne1)
	{
		if (pConfig && 
			1 ==SCT_ChannelRead(_hChanne1, RTR_GetDspSettings, &_dspConfig ) )
		{
			memcpy(pConfig,  &( _dspConfig.stHtAlarmPara), sizeof(_dspConfig.stHtAlarmPara));
		}
	}
}

bool ChannelControl::GetDspCrossInfo(DSP_CONFIG_PARA_CROSS_INFO* dspCross)
{
	bool ret = false;
	if( IsTcpConnected() && _hChanne1)
	{
		if (dspCross && 
			1 == SCT_ChannelRead(_hChanne1, RTR_GetDspSettings, &_dspConfig))
		{
			memcpy(dspCross,  &( _dspConfig.stCrossPara), sizeof(_dspConfig.stCrossPara));
			ret = true;
		}
	}

	return ret;
}

void ChannelControl::SetDspCrossInfo(DSP_CONFIG_PARA_CROSS_INFO* dspCross)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelWrite(_hChanne1,  RTW_SetDspCrossCursor ,   dspCross );
	}
}

void ChannelControl::RefreshDeviceInfo()
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelWrite(_hChanne1 , RTW_RefreshDeviceInfo,   0);
	}
}

void ChannelControl::RefreshDspDeviceInfo()
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelWrite(_hChanne1 , RTW_RefreshDspSettings,   0);
	}
}

void ChannelControl::SaveToFlash()
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelWrite(_hChanne1 , RTW_SaveAllMtParaToFlash,   0);
	}
}

void ChannelControl::RebootCamera()
{
	if( IsTcpConnected() && _hChanne1)
	{
		char time[MAX_PATH]={0};
		SYSTEMTIME CurTime;
		GetLocalTime(&CurTime);
		CString csTime;
		csTime.Format("%d-%d-%d %d:%d:%d",CurTime.wYear,CurTime.wMonth,CurTime.wDay,CurTime.wHour,CurTime.wMinute,CurTime.wSecond);
		int len = csTime.GetLength();
		for (int i = 0;i < len;++i)
		{
			time[i] = csTime[i];
		}
		              
		SCT_ChannelWrite(_hChanne1, RTW_SetDevicetime , &time);
	}
}

void ChannelControl::SetGrayAlarmMultiObjects(GrayAlarmMultiObjects* pGrayAlarmMultiObjects)
{
	if( IsTcpConnected() && _hChanne1 && pGrayAlarmMultiObjects)
	{
		SCT_ChannelWrite(_hChanne1, RTW_SetGrayAlarmMultiObjects , pGrayAlarmMultiObjects);
	}
}

void ChannelControl::SendSerialData(char* data , int len)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SerialPortData serialPortData;
		serialPortData.data = data;
		serialPortData.dataLen = len;

		SCT_ChannelWrite(_hChanne1,RTW_SendDataRS485 , &serialPortData);
	}
}

void ChannelControl::ShowOsd(Osd_Config_State* showData)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelWrite(_hChanne1 ,RTW_ShowOsd , showData);
	}
}

void ChannelControl::GetOsd(structOsdInfo* osdInfo)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelRead(_hChanne1 ,RTR_GetOsdInfo , osdInfo);
	}
}

void ChannelControl::SetImageAlarm(AlarmInfo* pAlarmInfo)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelWrite(_hChanne1 , RTW_SetImgAlarmInfo , pAlarmInfo );
	}
}

void ChannelControl::SetObjAlarm(AlarmInfo* pAlarmInfo)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelWrite(_hChanne1 , RTW_SetObjAlarmInfo, pAlarmInfo);
	}
}

void ChannelControl::SetAgcData(int enAgcMode, float ftHistMaxTemper, float ftHistMinTemper)
{
	if( IsTcpConnected() && _hChanne1)
	{
		AgcData agcData = {enAgcMode, ftHistMaxTemper ,ftHistMinTemper };
		SCT_ChannelWrite(_hChanne1 , RTW_SetAgc, &agcData);
	}
}

void ChannelControl::SetBmpBrightness(int value)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelWrite(_hChanne1 , RTW_SetBmpBrightness, &value);
	}
}

void ChannelControl::SetBmpContrast(int value)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelWrite(_hChanne1 , RTW_SetBmpContrast, &value);
	}
}

void ChannelControl::SetAbsFocusPos(unsigned short value)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelWrite(_hChanne1 , RTW_SetAbsFocusLensPos, &value);
	}
}

void ChannelControl::SetAbsZoomPos(unsigned short value)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelWrite(_hChanne1 , RTW_SetAbsZoomLensPos, &value);
	}
}

void  ChannelControl::GetAbsFocusPos(FocusPosition* pFocusPosition)
{

	if( IsTcpConnected() && _hChanne1 && pFocusPosition)
	{
		unsigned short pos = 0;

		SCT_ChannelRead(_hChanne1 , RTR_GetAbsFocusLensPos, pFocusPosition);

		//变倍指令
		//SCT_ChannelRead(_hChanne1 , RTR_GetAbsZoomLensPos, pFocusPosition);

	}

}

void ChannelControl::GetAbsZoomPos(FocusPosition* pFocusPosition)
{
	if( IsTcpConnected() && _hChanne1 && pFocusPosition)
	{
		unsigned short pos = 0;

		//SCT_ChannelRead(_hChanne1 , RTR_GetAbsFocusLensPos, pFocusPosition);

		//变倍指令
		SCT_ChannelRead(_hChanne1 , RTR_GetAbsZoomLensPos, pFocusPosition);

	}

}

void ChannelControl::SetUserData(unsigned int value)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelSetOpt(_hChanne1 , OT_SetUserData, &value);
	}
}

void ChannelControl::GetUserData(unsigned int* value)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelGetOpt(_hChanne1 , OT_SetUserData, value);
	}
}

void ChannelControl::SetObserveMode(char mode)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelWrite(_hChanne1 , RTW_SetObserveMode, &mode);
	}
}

void ChannelControl::SetAutoZero(bool state)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelWrite(_hChanne1 , RTW_SetShutterState, &state);
	}
}

void ChannelControl::SetAutoImage(int modetype)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelWrite(_hChanne1 , RTW_SetAutoImageMode, &modetype);
	}
}

void ChannelControl::SetAlarmShield(PTZ_ALARMSHIELD_t* data)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelWrite(_hChanne1 , RTW_SetAlarmShield,data);
	}
}

void ChannelControl::GetAlarmShield()
{
	if(_hChanne1)
	{
		int ptzid = 2;
		SCT_ChannelWrite(_hChanne1,RTW_GetAlarmShield, &ptzid);
	}
	Sleep(200);
	char DataLocal[52] = {0};
	SCT_ChannelRead(_hChanne1,RTR_GetAlarmShieldLocal,DataLocal);


	OutputDebugStringA(DataLocal);
}

void ChannelControl::GetTempDataNum(TempDataNum* tempDataNum)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelRead(_hChanne1, RTR_GetTempDataNum, tempDataNum);
	}
}

JpgPlusData* ChannelControl::GetJpgPlusData()
{
	if (IsRunning())
	{
		if (SCT_ChannelRead(_hChanne1,RTR_GetJpgPlus,&_jpgPlusData) == 1)
		{
			CString csLen;
			csLen.Format(_T("len1:%d"),_jpgPlusData.len);
			OutputDebugString(csLen);
			return &_jpgPlusData;
		}
	}
	return 0;
}



bool ChannelControl::GetDspImageSettings(DSP_CONFIG_PARA_IMAGE* pDspImageSettings)
{
	bool ret = false;

	if( IsTcpConnected() && _hChanne1)
	{
		if (pDspImageSettings && 
			1 ==SCT_ChannelRead(_hChanne1, RTR_GetDspSettings, &_dspConfig ) )
		{
			memcpy(pDspImageSettings,  &( _dspConfig.stImagePara), sizeof(_dspConfig.stImagePara));
			ret = true;

		}
		else{
				
			ret = false;
		}
	}

	return ret;
}

void ChannelControl::SetDspImageSettings(DSP_CONFIG_PARA_IMAGE* pDspImageSettings)
{
	bool ret = false;

	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelWrite(_hChanne1,  RTW_SetDspImageSettings,  pDspImageSettings );
	}
}

void ChannelControl::SetRawFps(int nfps)
{
	if( IsTcpConnected() && _hChanne1)
	{
		unsigned char fps = nfps;
		SCT_ChannelWrite(_hChanne1, RTW_SetRawFrameRate,	&fps);
	}

}

void ChannelControl::SetVideoFps(int nfps)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelWrite(_hChanne1, RTW_SetVideoFrameRate,	&nfps);
	}
}

void ChannelControl::SetCameraStall(char sel)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelWrite(_hChanne1, RTW_SetMtStall,	&sel);
	}
}

void ChannelControl::SetCameraLens(unsigned char sel)
{
	if( IsTcpConnected() && _hChanne1)
	{
		SCT_ChannelWrite(_hChanne1, RTW_SetMtLens,	&sel);
	}
}

//
//bool ChannelControl::IsOnline(char *host,int port, int timeout)
//{
//
//	return true;
//
//
//	TIMEVAL Timeout;
//	Timeout.tv_sec = 0;
//	Timeout.tv_usec = timeout*1000;
//	struct sockaddr_in address;  /* the libc network address data structure */   
//	
//	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//	
//	address.sin_addr.s_addr = inet_addr(host); /* assign the address */
//	address.sin_port = htons(port);            /* translate int2port num */	
//	address.sin_family = AF_INET;
//	
//	//set the socket in non-blocking
//	unsigned long iMode = 1;
//	int iResult = ioctlsocket(sock, FIONBIO, &iMode);
//	if (iResult != NO_ERROR)
//	{	
//		printf("ioctlsocket failed with error: %ld\n", iResult);
//	}
//
//	if(connect(sock,(struct sockaddr *)&address,sizeof(address))==false)
//	{	
//		closesocket(sock);
//		return false;
//	}	
//	
//	// restart the socket mode
//	iMode = 0;
//	iResult = ioctlsocket(sock, FIONBIO, &iMode);
//	if (iResult != NO_ERROR)
//	{	
//		printf("ioctlsocket failed with error: %ld\n", iResult);
//	}
//	
//	fd_set Write, Err;
//	FD_ZERO(&Write);
//	FD_ZERO(&Err);
//	FD_SET(sock, &Write);
//	FD_SET(sock, &Err);
//	
//	// check if the socket is ready
//	select(0,NULL,&Write,&Err,&Timeout);			
//	if(FD_ISSET(sock, &Write)) 
//	{	
//		closesocket(sock);
//		return true;
//	}
//	
//	closesocket(sock);
//	return false;
//}


