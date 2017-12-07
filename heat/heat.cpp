#define _CRT_SECURE_NO_DEPRECATE
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "stdafx.h"
#include "txt_to_mat.h"

int main() {
	// location of text file with thermal imaging data matrix
	std::string dir = "C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/heat/data/";
	std::string file = dir + "/input/snapshotdata1_hotspot.txt";
	// file stuff for testing
	FILE * pFile;
	pFile = fopen("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/heat/data/result.txt", "w");
	//min max
	double max, min;
	cv::Point minLoc, maxLoc;
	
	// thresholding settings
	cv::Mat thresh_img;
	int thresholdType = cv::THRESH_BINARY;
	int otsu = thresholdType + cv::THRESH_OTSU;
	int skin_threshold_low = 8200; // found through calibration / testing
	int otsu_max_value = 255;
	// morphology open settings
	int size_open = 4;
	int shape = cv::MORPH_ELLIPSE;
	int open = cv::MORPH_OPEN;
	int close = cv::MORPH_CLOSE;
	cv::Mat open_img;
	cv::Mat elem_open = cv::getStructuringElement(shape,
		cv::Size(2 * size_open + 1, 2 * size_open + 1),
		cv::Point(size_open, size_open));

	// want our thermal data in an openCV Mat format so that we can analyze it
	// dimensions of our FLiR Lepton thermal images are 80x60
	int thermal_width = 80;
	int thermal_height = 60;
	cv::Mat heat(thermal_width, thermal_height, CV_32SC1);
	// this function converts thermal text file data to a mat
	heat = txt_to_mat(file, thermal_width, thermal_height);

	cv::Mat heat_show(60, 80, CV_8UC1);
	cv::normalize(heat, heat_show, 0, 255, cv::NORM_MINMAX, CV_8UC1);
	cv::imwrite(dir + "heat0.jpg", heat_show);

	
	
	/*
	Need a mask that corresponds to just the face area.
	Threshold + Open is the plan. Here we try 2 types of thresholding.
	The ‘open’ operation removes smaller, hotter objects in the background.
	
	With the mask we can find skin temperature by finding highest thermal
	value within face region.
	*/

	/*
	MASK EXTRACTION #1: simple thresholding
	Because of the narrow range of human body temperature, I expect a hard coded, 
	calibrated threshold value will work well. 
	*/

	cv::Mat heat_con;
	heat.convertTo(heat_con, CV_32F);

	//ideally you would find meadian as a threshold value
	//cv::minMaxLoc(heat, &min, &max, &minLoc, &maxLoc);
	//skin_threshold_low = 0.9 * max;

	thresh_img = heat_con > skin_threshold_low;
	cv::morphologyEx(thresh_img, open_img, close, elem_open);
	cv::morphologyEx(open_img, open_img, open, elem_open);
	cv::minMaxLoc(heat, &min, &max, &minLoc, &maxLoc, open_img);
	
	cv::Mat heat_mask;
	heat_show.copyTo(heat_mask, open_img);
	cv::imwrite(dir + "heat1.3_masked.jpg", heat_mask);

	cv::imwrite(dir + "heat1.1_thresh.jpg", thresh_img);
	cv::imwrite(dir + "heat1.2_open.jpg", open_img);
	fprintf(pFile, "Simple Threshold \n");
	fprintf(pFile, "Largest thermal value: %f @(%d, %d) \n", max, maxLoc.x, maxLoc.y);
	fprintf(pFile, "Smallest thermal value: %f @(%d, %d) \n\n", min, minLoc.x, minLoc.y);

	/*
	MASK EXTRACTION #2: otsu tresholding
	This was the first thing I tried but don't think it will be robust in the case of anything
	really hot or really cold in the background, such as a lightbulb or cold window.
	*/
	/*
	cv::threshold(heat_show, thresh_img, 0, otsu_max_value, thresholdType + cv::THRESH_OTSU);
	cv::morphologyEx(thresh_img, open_img, open, elem_open);
	cv::minMaxLoc(heat, &min, &max, &minLoc, &maxLoc, open_img);

	heat.copyTo(heat_mask, open_img);
	cv::imwrite(dir + "heat2.3_masked.jpg", heat_mask);
	
	cv::imwrite(dir + "heat2.1_thresh.jpg", thresh_img);
	cv::imwrite(dir + "heat2.2_open.jpg", open_img);
	fprintf(pFile, "Otsu Threshold \n");
	fprintf(pFile,"Largest thermal value: %f @(%d, %d) \n", max, maxLoc.x, maxLoc.y);
	fprintf(pFile, "Sallest thermal value: %f @(%d, %d) \n", min, minLoc.x, minLoc.y);
	*/
}