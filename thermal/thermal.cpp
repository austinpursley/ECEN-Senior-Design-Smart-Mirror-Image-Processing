/*
	Name: thermal.c
	Date: 12/2017
	Author: Austin Pursley
	Course: ECEN 403, Senior Design Smart Mirror
	
	Uses computer vision to analyze thermal images of someone
	to get a skin temperature metric.
*/

#define _CRT_SECURE_NO_DEPRECATE
#include "stdafx.h"
#include "thermal.h"

int main() {
	//location of text file with thermal imaging data matrix
	std::string dir = "C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/thermal/data/";
	std::string file = dir + "input/snapshotdata1.txt";
	//text file for testing results
	FILE * pFile;
	pFile = fopen("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/thermal/data/result.txt", "w");

	//min max
	double max, min;
	cv::Point minLoc, maxLoc;
	//thresholding settings
	cv::Mat thresh_img;
	int thresholdType = cv::THRESH_BINARY;
	int otsu = thresholdType + cv::THRESH_OTSU;
	//int thresh_value = 8200; // found through calibration / testing
	double median;
	//morphology open settings
	int size_open = 5;
	int shape = cv::MORPH_ELLIPSE;
	int open = cv::MORPH_OPEN;
	int close = cv::MORPH_CLOSE;
	cv::Mat open_img;
	cv::Mat elem = cv::getStructuringElement(shape,
		cv::Size(2 * size_open + 1, 2 * size_open + 1),
		cv::Point(size_open, size_open));
	
	//Mat to hold thermal image pixel values
	int thermal_width = 80;  // FLiR Lepton thermal image size: 80x60
	int thermal_height = 60;
	cv::Mat thermal(thermal_width, thermal_height, CV_32SC1);
	//this function converts thermal text file data to an OpenCV Mat
	thermal = txt_to_mat(file, thermal_width, thermal_height);

	//convert to 32 bit float for compatability with openCV functions
	cv::Mat thermal_float;
	thermal.convertTo(thermal_float, CV_32FC1);
	
	//threshold value is the median pixel value of the thermal image
	cv::minMaxLoc(thermal_float, &min, &max, &minLoc, &maxLoc);
	median = medianMat(thermal_float, max);
	fprintf(pFile, "threshold is median: %f \n", median);
	
	//thresholded image is our mask.
	thresh_img = thermal_float > median;
	//remove small spots from mask
	cv::morphologyEx(thresh_img, open_img, open, elem);
	//close gaps in mask
	cv::morphologyEx(open_img, open_img, close, elem);
	
	//skin temperature is max value of thermal Mat with mask
	cv::minMaxLoc(thermal, &min, &max, &minLoc, &maxLoc, open_img);
	double skin_temp = max;

	//output for testing and presentation
	cv::Mat thermal_mask;
	cv::Mat thermal_show(60, 80, CV_8UC1);
	cv::normalize(thermal, thermal_show, 0, 255, cv::NORM_MINMAX, CV_8UC1);
	cv::imwrite(dir + "thermal0.jpg", thermal_show);
	thermal_show.copyTo(thermal_mask, open_img);
	cv::imwrite(dir + "thermal3_masked.jpg", thermal_mask);
	cv::imwrite(dir + "thermal1_thresh.jpg", thresh_img);
	cv::imwrite(dir + "thermal2_open.jpg", open_img);
	fprintf(pFile, "Simple Threshold \n");
	fprintf(pFile, "Largest thermal value: %f @(%d, %d) \n", 
		max, maxLoc.x, maxLoc.y);
	fprintf(pFile, "Smallest thermal value: %f @(%d, %d) \n\n", 
		min, minLoc.x, minLoc.y);
}