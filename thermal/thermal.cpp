/*
Name: thermal.cpp
Date: 01/2018
@author Austin Pursley
Course: ECEN 403, Senior Design Smart Mirror

Purpose: demonstrate my thermal imaging processing functions.
*/

#define _CRT_SECURE_NO_DEPRECATE
#include "stdafx.h"
#include "thermal.h"

/*
Convert text file with matrix of int values to an OpenCV Mat

Specific purpose is to convert thermal imaging data given by Flir
Lepton so that it can be analyzed by OpenCV.
*/
cv::Mat txt_to_mat(std::string txt_file, int width, int height) {
	//The data in txt will be integer values, hence CV_32SC1
	cv::Mat mat(height, width, CV_32S);
	std::ifstream ifs(txt_file);
	std::string row;
	int tempint;
	int i = 0;
	int j = 0;
	while (std::getline(ifs, row)) {
		j = 0;
		std::istringstream iss(row);
		std::vector<int> tempv;
		while (iss >> tempint) {
			mat.at<int>(i, j) = tempint;
			j++;
		}
		i++;
	}
	return mat;
}

/*
Finds the median value of an OpenCV mat (mat_in)
Specific purpose here is find a threshold value for thermal image analysis.
SOURCE: stackoverflow.com/a/30093862

to-do: adjust to remove nVals, make only work for 8bit images.
*/
double medianMat(cv::Mat mat_in, int nVals) {
	// COMPUTE HISTOGRAM OF SINGLE CHANNEL MATRIX
	float range[] = { 0, nVals };
	const float* histRange = { range };
	bool uniform = true; bool accumulate = false;
	cv::Mat hist;
	calcHist(&mat_in, 1, 0, cv::Mat(), hist, 1, &nVals, &histRange, uniform,
		accumulate);

	// COMPUTE CUMULATIVE DISTRIBUTION FUNCTION (CDF)
	cv::Mat cdf;
	hist.copyTo(cdf);
	for (int i = 1; i <= nVals - 1; i++) {
		cdf.at<float>(i) += cdf.at<float>(i - 1);
	}
	cdf /= mat_in.total();

	// COMPUTE MEDIAN
	double medianVal;
	for (int i = 0; i <= nVals - 1; i++) {
		if (cdf.at<float>(i) >= 0.5) { medianVal = i;  break; }
	}
	return medianVal;
}

/*
Takes a thermal image of a person and finds the median
pixel value of the a region of interest that corresponds to the person.

The purpose is to extract an approximate skin temperature metric.

todo: error check for non-gray/1channel images.
*/
double temp_from_thermal_img(const cv::Mat &thermal) {	
	//int thresh_value = 8200; // found through calibration / testing
	//morphology open settings
	int size_open = 5;
	int shape = cv::MORPH_ELLIPSE;
	
	cv::Mat elem = cv::getStructuringElement(shape,
		cv::Size(2 * size_open + 1, 2 * size_open + 1),
		cv::Point(size_open, size_open));

	//convert to 32 bit float for compatability with openCV functions
	cv::Mat thermal_float;
	thermal.convertTo(thermal_float, CV_32FC1);
	//threshold value is the median pixel value of the thermal image
	double max, min;
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(thermal_float, &min, &max, &minLoc, &maxLoc);
	double median = medianMat(thermal_float, max);
	//thresholded image is our mask.
	cv::Mat thresh_img = thermal_float > median;
	//remove small spots from mask (open) and close gaps (close)
	cv::Mat open_img, mask;
	cv::morphologyEx(thresh_img, open_img, cv::MORPH_OPEN, elem);
	cv::morphologyEx(open_img, mask, cv::MORPH_CLOSE, elem);
	//skin temperature is max value of thermal Mat with mask
	cv::minMaxLoc(thermal, &min, &max, &minLoc, &maxLoc, mask);
	double skin_temp = max;

	///OUTPUT / DEBUG
	/*
	std::string dir = "C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/thermal/data/";
	FILE * pFile;
	pFile = fopen("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/thermal/data/result.txt", "w");
	cv::Mat thermal_mask;
	cv::Mat thermal_show(60, 80, CV_8UC1);
	cv::normalize(thermal, thermal_show, 0, 255, cv::NORM_MINMAX, CV_8UC1);
	cv::imwrite(dir + "thermal0.jpg", thermal_show);
	thermal_show.copyTo(thermal_mask, open_img);
	cv::imwrite(dir + "thermal3_masked.jpg", thermal_mask);
	cv::imwrite(dir + "thermal1_thresh.jpg", thresh_img);
	cv::imwrite(dir + "thermal2_open.jpg", open_img);
	fprintf(pFile, "Median as Threshold Value \n");
	fprintf(pFile, "median: %f \n", median);
	fprintf(pFile, "Largest thermal value: %f @(%d, %d) \n",
		max, maxLoc.x, maxLoc.y);
	fprintf(pFile, "Smallest thermal value: %f @(%d, %d) \n\n",
		min, minLoc.x, minLoc.y);
	*/
	return skin_temp;
}