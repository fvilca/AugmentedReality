#include "PreProcessing.h"

PreProcessing::PreProcessing() {
}

PreProcessing::~PreProcessing() {
}

cv::Mat PreProcessing::PreProcess(cv::Mat& in, cv::Mat& out,bool thr) {
	if(!in.type() == CV_8UC1)
		cvtColor(in, out, CV_BGR2GRAY);
	
	if (thr == true)
	{	
		//adaptiveThreshold(out, out, 255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,25,10);
		GaussianBlur(out, out, Size(7, 7), 0, 0);
		adaptiveThreshold(out, out, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 75, 10);
	}
	else
	{
		GaussianBlur(out, out, Size(5, 5), 0, 0);
		// solo con Circulos
		//adaptiveThreshold(out, out, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 25, 10);		
	}
	imshow("preprocessssss",out);

	return out;
}