#ifndef CAMERA_CONTROL_H
#define CAMERA_CONTROL_H

#include "SCT_DataType.h"
#include "SCT_ChannelSDK.h"
class CameraControl
{


public:


private:
	HChannel _hChanne1;     //�豸���
	OpenParams _openParams; //���Ӳ���

	static bool s_bIniFlag; //��ʼ����־

};

#endif