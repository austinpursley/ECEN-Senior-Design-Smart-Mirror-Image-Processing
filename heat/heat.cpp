// heat.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_DEPRECATE
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "stdafx.h"
#include "txt_to_mat.h"

int main() {
	
	//location of text file that contains
	std::string dir = "C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/heat/data/";
	std::string file = dir + "snapshotdata1.txt";

	cv::Mat heat(60, 80, CV_32SC1);
	//file stuff for testing
	FILE * pFile;
	pFile = fopen("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/heat/data/result.txt", "w");

	//thresholding
	cv::Mat thresh_img;
	int thresholdType = cv::THRESH_BINARY;
	int maxValue = 255;
	cv::Mat heat_show(60, 80, CV_8UC1);
	//morphology
	cv::Mat close_img, open_img;
	int size_close = 4;
	int size_open = 4;
	int shape = cv::MORPH_ELLIPSE;
	int open = cv::MORPH_OPEN;
	int close = cv::MORPH_CLOSE;
	cv::Mat elem_close = cv::getStructuringElement(shape,
		cv::Size(2 * size_close + 1, 2 * size_close + 1),
		cv::Point(size_close, size_close));
	cv::Mat elem_open = cv::getStructuringElement(shape,
		cv::Size(2 * size_open + 1, 2 * size_open + 1),
		cv::Point(size_open, size_open));

	//our function to convert text file matrix to a openCV Mat
	heat = txt_to_mat(file, 60, 80);
	//normalize it for processing
	cv::normalize(heat, heat_show, 0, 255, cv::NORM_MINMAX, CV_8UC1);
	cv::imwrite(dir + "heat0.jpg", heat_show);
	//ostu threshodling
	cv::threshold(heat_show, thresh_img, 0, maxValue, thresholdType + cv::THRESH_OTSU);
	cv::imwrite(dir + "heat1_thresh.jpg", thresh_img);
	//open removes smaller blobs, want remove small hot spots
	cv::morphologyEx(thresh_img, open_img, open, elem_open);
	cv::imwrite(dir + "heat2_open.jpg", open_img);

	double max, min;
	cv::Point minLoc, maxLoc;
	//cv::minMaxLoc(heat, &min, &max, &minLoc, &maxLoc, open_img);
	cv::minMaxLoc(heat, &min, &max, &minLoc, &maxLoc);

	fprintf(pFile,"largest heat pixel: %f @(%d, %d) \n", max, maxLoc.x, maxLoc.y);
	fprintf(pFile, "largest heat pixel: %f @(%d, %d) \n", min, minLoc.x, minLoc.y);

	cv::waitKey();

}