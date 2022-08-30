#pragma once
#define PIC_WIDTH 384
#define PIC_HEIGHT 288
#define PIC_SIZE PIC_WIDTH*PIC_HEIGHT
#include "string"
#include "array"
using namespace std;
class Frame
{
public:
	void InitFreamData() {
		memset(this->fFrameData, 0, sizeof(this->fFrameData));
	}

	float* GetFreamData()
	{
		return this->fFrameData;
	}
	

private:
	float fFrameData[PIC_SIZE];
	array<float, PIC_SIZE>vecData {};
};

