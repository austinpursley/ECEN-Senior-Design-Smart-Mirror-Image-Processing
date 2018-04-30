#include "stdafx.h"
#include <string>
#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "dirent.h"
#include "skin_extract.h"
//#include "color_face.h"
#define _CRT_SECURE_NO_DEPRECATE


int main(int argc, char** argv) {
	std::string input_dir("C:/Users/Austin Pursley/Desktop/ECEN-Senior-Design-Smart-Mirror-Image-Processing/rgb/skin_extract/input/");
	std::string dir_out("C:/Users/Austin Pursley/Desktop/ECEN-Senior-Design-Smart-Mirror-Image-Processing/rgb/skin_extract/output/");
	//get list of images in input direcotry
	DIR *dpdf;
	struct dirent *epdf;
	std::vector<std::string> filenames;
	dpdf = opendir(input_dir.c_str());
	if (dpdf != NULL) {
		while (epdf = readdir(dpdf)) {
			filenames.push_back(std::string(epdf->d_name));
		}
	}

	double maxL = 0;
	double maxA = 0;
	double maxB = 0;

	double minL = 255;
	double minA = 255;
	double minB = 255;

	for (int i = 2; i < filenames.size(); i++) {
		//read image
		std::string img_file = filenames[i]; //e.g. name.jpg
		size_t lastindex = img_file.find_last_of(".");
		std::string img_name = img_file.substr(0, lastindex); //e.g name (no .jpg extension)

		std::string img_dir(input_dir.c_str() + img_file);
		cv::Mat matImage = cv::imread(img_dir, cv::IMREAD_COLOR);
		if (!matImage.data) {
			std::cout << "Unable to open the file: " << img_dir;
			return 1;
		}
		
		//convert to lab color space
		cv::Mat lab_image;
		//cvtColor(matImage, lab_image, cv::COLOR_BGR2Lab);
		cvtColor(matImage, lab_image, cv::COLOR_RGB2HSV);
		

		// crop
		double scale = 0.25;
		int offset_x = matImage.size().width*scale;
		int offset_y = matImage.size().height*scale;
		cv::Rect roi;
		roi.x = offset_x;
		roi.y = offset_y;
		roi.width = matImage.size().width - (offset_x * 2);
		roi.height = matImage.size().height - (offset_y * 2);
		cv::Mat crop = matImage(roi);
		
		// thresholding and mask
		int thresholdType = cv::THRESH_BINARY;
		int otsu = thresholdType + cv::THRESH_OTSU;
		int otsu_max_value = 255;
		cv::Mat thresh, gray;
		cvtColor(crop, gray, cv::COLOR_BGR2GRAY);
		cv::threshold(gray, thresh, 0, otsu_max_value, thresholdType + cv::THRESH_OTSU);

		//extract mean
		cvtColor(crop, crop, cv::COLOR_RGB2HSV);
		cv::Scalar mean = cv::mean(crop, thresh);
		//cv::Scalar lower = 0.93*mean;
		//cv::Scalar upper = 1.03*mean;
		
		cv::Scalar lower = mean;
		cv::Scalar upper = mean;

		lower[0] = lower[0] - 12;
		upper[0] = upper[0] + 12;

		lower[1] = lower[1] - 100;
		upper[1] = upper[1]  + 100;

		lower[2] = lower[2] - 100;
		upper[2] = upper[2] + 100;

		//in range
		cv::Mat range;
		cv::inRange(lab_image, lower, upper, range);
		
		///round 2
		/*
		mean = cv::mean(lab_image, range);
		lower = mean;
		upper = mean;
		lower[0] = lower[0] * 0.6;
		upper[0] = upper[0] * 1.65;
		lower[1] = lower[1] * 0.92;
		upper[1] = upper[1] * 1.08;
		lower[2] = lower[2] * 0.92;
		upper[2] = upper[2] * 1.08;
		//in range
		cv::inRange(lab_image, lower, upper, range);
		*/


		cvtColor(crop, crop, cv::COLOR_HSV2RGB);
		//cvtColor(crop, crop, cv::COLOR_RGB2BGR);
		cv::Mat crop_mask;
		crop.copyTo(crop_mask, thresh);
		//cv::imwrite(dir_out + img_name + "_0.jpg", matImage);
		//cv::imwrite(dir_out + img_name + "_2_thresh.jpg", thresh);
		cv::imwrite(dir_out + img_name + "_2_crop.jpg", crop_mask);
		cv::imwrite(dir_out + img_name + "_3_range.jpg", range);


		/*
		

		// guassian Blur
		cv::Size ksize;
		ksize.height = 1;
		ksize.width = ksize.height;
		cv::Mat blurr_img;
		cv::GaussianBlur(matImage, blurr_img, ksize, 0);

		//convert to lab color space
		cv::Mat lab_image;
		cvtColor(blurr_img, lab_image, cv::COLOR_BGR2Lab);

		///red-green
		//otsu threshold 1

		//otsu threshold 2
		//cv::Mat1b gr_thresh2;
		//threshold_with_mask(lab[1], gr_thresh2, 0, otsu_max_value, thresholdType + cv::THRESH_OTSU, gr_thresh);
		//bitwise_and(gr_thresh, gr_thresh2, gr_thresh2);
		//cv::imwrite(dir_out + "2.2_greenred_thresh.jpg", gr_thresh2);

		///blue-yellow
		//otsu threshold
		
		//otsu threshold 2
		//cv::Mat1b by_thresh2;
		//threshold_with_mask(lab[2], by_thresh2, 0, otsu_max_value, thresholdType + cv::THRESH_OTSU, by_thresh);
		//bitwise_and(by_thresh, by_thresh2, by_thresh2);
		//cv::imwrite(dir_out + "1.2_blueyellow_thresh.jpg", by_thresh2);
		
		cv::Mat test;
		int min_L = 10; //15
		int min_a = 110; //134
		int min_b = 160; //137
		int max_L = 255; //235
		int max_a = 170; //170
		int max_b = 180; //160
		cv::inRange(lab_image, cv::Scalar(min_L, min_a, min_b), cv::Scalar(max_L, max_a, max_b), test);
		cv::imwrite(dir_out + img_name + "_inrange_test.jpg", test);
		cv::Mat mask_nonskin;

		//bitwise_and(matImage, matImage, mask_nonskin, test);
		bitwise_and(matImage, matImage, mask_nonskin);
		cvtColor(mask_nonskin, lab_image, CV_BGR2Lab);
		
		cv::Mat skin_detect_mask;
		matImage.copyTo(skin_detect_mask, test);
		//cv::imwrite(dir_out + img_name + "_test_masked.jpg", skin_detect_mask);

		//split channels
		cv::Mat1b lab[3];
		split(lab_image, lab);

		cv::Mat gr_thresh;
		cv::threshold(lab[1], gr_thresh, 0, otsu_max_value, thresholdType + cv::THRESH_OTSU);
		cv::imwrite(dir_out + "_1.0_greenred.jpg", lab[1]);
		cv::imwrite(dir_out + img_name + "_2.1_greenred_thresh.jpg", gr_thresh);
		
		cv::Mat1b by_thresh;
		cv::threshold(lab[2], by_thresh, 0, otsu_max_value, thresholdType + cv::THRESH_OTSU);
		cv::imwrite(dir_out + img_name + "_2.0_blueyellow.jpg", lab[2]);
		cv::imwrite(dir_out + img_name + "_2.2_blueyellow_thresh.jpg", by_thresh);
		
		
		cv::Mat mask;
		bitwise_and(gr_thresh, by_thresh, mask);
		//mask to show where we are extracting mean color
		cv::Mat masked;
		matImage.copyTo(masked, mask);
		cv::imwrite(dir_out + img_name + "_masked.jpg", masked);

		//extract mean
		cv::Scalar mean = cv::mean(lab_image, mask);
		
		//printf("blue: %f \n", mean[0]);
		//printf("green: %f \n", mean[1]);
		//printf("red: %f \n\n", mean[2]);
		/// TESTING FOR MAX/MIN VALUES
		cv::Mat1b m_lab[3];
		split(lab_image, m_lab);

		double maxL_c;
		double maxA_c;
		double maxB_c;

		double minL_c;
		double minA_c;
		double minB_c;

		cv::Point loc;

		cv::minMaxLoc(m_lab[0], &minL_c, &maxL_c, &loc, &loc, mask); //find minimum and maximum intensities
		cv::minMaxLoc(m_lab[1], &minA_c, &maxA_c, &loc, &loc, mask);
		cv::minMaxLoc(m_lab[2], &minB_c, &maxB_c, &loc, &loc, mask);
		//max
		if (maxL_c > maxL) {
			maxL = maxL_c;
		}
		if (maxA_c > maxA) {
			maxA = maxA_c;
		}
		if (maxB_c  > maxB) {
			maxB = maxB_c;
		}
		//min
		if (minL_c < minL) {
			minL = minL_c;
		}
		if (minA_c < minA) {
			minA = minA_c;
		}
		if (minB_c < minB) {
			minB = minB_c;
		}
		///---------------------------------------
		
		//mean color square
		mean = cv::mean(matImage, mask);
		int tile_size = 64;
		cv::Rect rect(0, 0, tile_size, tile_size);
		cv::Mat ret = cv::Mat(tile_size, tile_size, CV_8UC3, cv::Scalar(0));
		cv::rectangle(ret, rect, mean, CV_FILLED);
		cv::imwrite(dir_out + img_name + "_mean_color.jpg", ret);
	
	*/
	}

	/*
	printf("maxL: %f \n", maxL);
	printf("maxA: %f \n", maxA);
	printf("maxB: %f \n\n", maxB);

	printf("minL: %f \n", minL);
	printf("minA: %f \n", minA);
	printf("minB: %f \n\n", minB);
	*/

	return 0;
}