#ifndef CAMERA_CONTROL_H
#define CAMERA_CONTROL_H

#include "SCT_DataType.h"
#include "SCT_ChannelSDK.h"
class CameraControl
{


public:


private:
	HChannel _hChanne1;     //设备句柄
	OpenParams _openParams; //链接参数

	static bool s_bIniFlag; //初始化标志

};

#endif