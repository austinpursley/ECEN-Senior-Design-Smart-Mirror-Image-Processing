/*
Name: skin_extract.cpp
Date: 06/2018
Author: Austin Pursley
Course: ECEN 403/404, Senior Design Smart Mirror

Purpose: Image processing to extract portion of face image that is skin.
*/

#include "stdafx.h"
#include "skin_extract.hpp"

// Cover up lips and eyes of a face with black ellipses.
void mask_features(cv::Mat face, std::map<std::string, cv::Rect> features, cv::Mat& masked_face) {
	face.copyTo(masked_face);
	double escale = 0.4;
	cv::Point center(features["eye1"].x + features["eye1"].width / 2, features["eye1"].y + features["eye1"].height / 2);
	cv::Size axes(features["eye1"].height * escale, features["eye1"].width * escale);
	ellipse(masked_face, center, axes, 90, 0, 360, 0, -1);
	center = cv::Point(features["eye2"].x + features["eye2"].width / 2, features["eye2"].y + features["eye2"].height / 2);
	axes = cv::Size(features["eye2"].height * escale, features["eye2"].width * escale);
	ellipse(masked_face, center, axes, 90, 0, 360, 0, -1);

	double mscale = 0.5;
	center = cv::Point(features["mouth"].x + features["mouth"].width / 2, features["mouth"].y + features["mouth"].height / 2);
	axes = cv::Size(features["mouth"].height * mscale, features["mouth"].width * mscale);
	ellipse(masked_face, center, axes, 90, 0, 360, 0, -1);

	return;
}

// Apply some morphological operations to binary skin mask.
void skin_morph(cv::Mat& mask) {
	int shape = cv::MORPH_ELLIPSE;
	cv::bitwise_not(mask, mask);
	for (int size = 3; size < 8; size++) {
		cv::Mat elem = cv::getStructuringElement(shape,
			cv::Size(2 * size + 1, 2 * size + 1),
			cv::Point(size, size));
		cv::morphologyEx(mask, mask, cv::MORPH_OPEN, elem);
		cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, elem);
	}
	cv::bitwise_not(mask, mask);
}

// Determine skin area based on known face color.
void extract_skin(cv::Mat img, std::map<std::string, cv::Rect> features, cv::Mat& skin_face) {
	cv::Mat face = img(features["face"]);
	cv::Mat face_masked;
	mask_features(face, features, face_masked);
	
	// "Zoomed in" crop of face
	double scalex = 0.175;
	double scaley = 0.15;
	int offset_x = face_masked.size().width*scalex;
	int offset_y = face_masked.size().height*scaley;
	cv::Rect roi;
	roi.x = offset_x;
	roi.y = offset_y;
	roi.width = face_masked.size().width - (offset_x * 2);
	roi.height = face_masked.size().height - (offset_y * 2.5);
	cv::Mat crop = face_masked(roi);

	// Get mean HSV color of face crop
	int thresholdType = cv::THRESH_BINARY;
	int otsu = thresholdType + cv::THRESH_OTSU;
	int otsu_max_value = 255;
	cv::Mat thresh, gray;
	cvtColor(crop, gray, cv::COLOR_BGR2GRAY);
	cv::threshold(gray, thresh, 1, 255, cv::THRESH_BINARY);
	cvtColor(crop, crop, cv::COLOR_BGR2HSV);
	cv::Scalar mean = cv::mean(crop, thresh);

	// Use mean HSV color to determine value range for skin pixels
	cv::Scalar lower = mean;
	cv::Scalar upper = mean;
	lower[0] = lower[0] - 10;
	lower[1] = lower[1] - 40;
	lower[2] = lower[2] - 40;
	upper[0] = upper[0] + 10;
	upper[1] = upper[1] + 50;
	upper[2] = upper[2] + 100;
	cv::Mat hsv_img, range;
	cvtColor(img, hsv_img, cv::COLOR_BGR2HSV, 3);
	cv::inRange(hsv_img, lower, upper, range);
	skin_morph(range); // Morphology
	cv::Mat skin_macro;
	img.copyTo(skin_macro, range);
	skin_face = skin_macro(features["face"]);

	//// Mask features (mouth and eyes)
	//cv::Mat feat_masked, gray_img, thresh_img;
	//mask_features(skin_face, features, feat_masked);
	//cvtColor(feat_masked, gray_img, cv::COLOR_BGR2GRAY);
	//cv::threshold(gray_img, thresh_img, 1, 255, cv::THRESH_BINARY);
	//thresh_img.convertTo(skin_mask, CV_8U);
}