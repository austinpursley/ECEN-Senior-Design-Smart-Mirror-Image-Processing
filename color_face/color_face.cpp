#include "stdafx.h"
#include <string>
#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "color_face.h"


int main(int argc, char** argv) {
	std::string img_name = "snapshot1_mcrop";
	std::string img_name_jpg = img_name + ".jpg";
	std::string imageName("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/color_face/input/" + img_name_jpg); 
	std::string dir_out("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/color_face/output/");

	cv::Mat matImage = cv::imread(imageName);
	cv::imwrite(dir_out + img_name + "_0.jpg", matImage);
	if (!matImage.data) {
		std::cout << "Unable to open the file: " << imageName;
		return 1;
	}

	// thresholding settings
	int thresholdType = cv::THRESH_BINARY;
	int otsu = thresholdType + cv::THRESH_OTSU;
	int otsu_max_value = 255;
	// guassian Blur setting
	cv::Size ksize;
	ksize.height = 5;
	ksize.width = ksize.height;
	cv::Mat blurr_img;
	cv::GaussianBlur(matImage, blurr_img, ksize, 0);

	//convert to lab color space, split channels
	cv::Mat lab_image;
	cvtColor(blurr_img, lab_image, cv::COLOR_BGR2Lab);
	cv::Mat1b lab[3];
	split(lab_image, lab);

	///red-green
	cv::imwrite(dir_out + "2.0_greenred.jpg", lab[1]);
	
	//otsu threshold 1
	cv::Mat gr_thresh;
	cv::threshold(lab[1], gr_thresh, 0, otsu_max_value, thresholdType + cv::THRESH_OTSU);
	cv::imwrite(dir_out + img_name +  "2.1_greenred_thresh.jpg", gr_thresh);
	
	//otsu threshold 2
	//cv::Mat1b gr_thresh2;
	//threshold_with_mask(lab[1], gr_thresh2, 0, otsu_max_value, thresholdType + cv::THRESH_OTSU, gr_thresh);
	//bitwise_and(gr_thresh, gr_thresh2, gr_thresh2);
	//cv::imwrite(dir_out + "2.2_greenred_thresh.jpg", gr_thresh2);
	
	///blue-yellow
	cv::imwrite(dir_out + "1.0_blueyellow.jpg", lab[2]);
	//otsu threshold
	cv::Mat1b by_thresh;
	cv::threshold(lab[2], by_thresh, 0, otsu_max_value, thresholdType + cv::THRESH_OTSU);
	cv::imwrite(dir_out + img_name + "1.1_blueyellow_thresh.jpg", by_thresh);
	
	//otsu threshold 2
	//cv::Mat1b by_thresh2;
	//threshold_with_mask(lab[2], by_thresh2, 0, otsu_max_value, thresholdType + cv::THRESH_OTSU, by_thresh);
	//bitwise_and(by_thresh, by_thresh2, by_thresh2);
	//cv::imwrite(dir_out + "1.2_blueyellow_thresh.jpg", by_thresh2);
	
	cv::Mat mask;
	bitwise_and(gr_thresh, by_thresh, mask);

	//mask to show where we are extracting mean color
	cv::Mat masked;
	matImage.copyTo(masked, mask);
	cv::imwrite(dir_out + img_name + "masked.jpg", masked);

	//extract mean
	cv::Scalar mean = cv::mean(matImage, gr_thresh);

	//dispaly results
	printf("blue: %f \n", mean[0]);
	printf("green: %f \n", mean[1]);
	printf("red: %f \n", mean[2]);
	//mean color square
	int tile_size = 64;
	cv::Rect rect(0, 0, tile_size, tile_size);
	cv::Mat ret = cv::Mat(tile_size, tile_size, CV_8UC3, cv::Scalar(0));
	cv::rectangle(ret, rect, mean, CV_FILLED);
	cv::imwrite(dir_out + img_name + "_mean_color.jpg", ret);
	
	
	return 0;
	
}