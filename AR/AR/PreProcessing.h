#pragma once
#include "Common.h"

class PreProcessing{

public:
	PreProcessing();
	~PreProcessing();
	cv::Mat PreProcess(cv::Mat&, cv::Mat&,bool thr=false);
};