#pragma once
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


class JudgementPara
{
public:
	float	fTempThreshold = 90.0; //�¶���ֵ
	int		iPointThreshold = 10000; //���µ���ֵ
	double	dLengthWidthRatio = 1.0; //�������ֵ

	// �趨�����λ��
	//Rect rTopRect = Rect(162, 67, 60, 20);
	//Rect rMiddleRect = Rect(162, 134, 60, 20);
	//Rect rBottomRect = Rect(162, 201, 60, 20);

};

