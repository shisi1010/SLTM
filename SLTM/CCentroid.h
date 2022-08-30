#pragma once
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class CCentroid
{

public:
	int iCentroidExist = 0;
	cv::Point pCentroid = { 0, 0 };
};