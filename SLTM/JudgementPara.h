#pragma once
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


class JudgementPara
{
public:
	float	fTempThreshold = 90.0; //温度阈值
	int		iPointThreshold = 10000; //高温点阈值
	double	dLengthWidthRatio = 1.0; //长宽比阈值

	// 设定侵入框位置
	//Rect rTopRect = Rect(162, 67, 60, 20);
	//Rect rMiddleRect = Rect(162, 134, 60, 20);
	//Rect rBottomRect = Rect(162, 201, 60, 20);

};

