#include "stdafx.h"
#include <string>
#include <iostream>
#include "../dominant_color.h"

/*This program does...*/
int main(int argc, char** argv) {


	std::string imageName("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/data/eyes/eye_b0.jpg"); // by default

	//if (argc > 1)
	//{
	//	//I confingured this in Visual Studios debugging command arguments
	//	imageName = argv[1];
	//}

	cv::Mat matImage = cv::imread(imageName);
	//cv::Mat gray_image = cv::imread(imageName, 0);

	if (!matImage.data) {
		std::cout << "Unable to open the file: " << imageName;
		return 1;

	}

	//display original image
	cv::namedWindow("Original Image", cv::WINDOW_AUTOSIZE);
	cv::imshow("Original Image", matImage);

	//Apply a median blur filter to reduce noise while keeping edges
	int ksize = 3;
	cv::medianBlur(matImage, matImage, ksize);
	//display median blurred image
	cv::namedWindow("Median Blurred", cv::WINDOW_AUTOSIZE);
	cv::imshow("Median Blurred", matImage);

	//adaptive threshold test
	cv::Mat threshold_img = matImage.clone();
	//convert to grayscale
	cv::cvtColor(threshold_img, threshold_img, cv::COLOR_RGB2GRAY);
	cv::Mat adpt_threshold_img = threshold_img.clone();
	
	//threshold
	double thresh = 70;
	double maxValue = 255;
	cv::threshold(threshold_img, threshold_img, thresh, maxValue, cv::THRESH_BINARY);
	//display threshold
	cv::namedWindow("Threshold", cv::WINDOW_AUTOSIZE);
	cv::imshow("Threshold", threshold_img);

	//set adaptive threshold paramteres
	maxValue = 255;
	int adaptiveMethod = cv::ADAPTIVE_THRESH_MEAN_C;
	int thresholdType = cv::THRESH_BINARY;
	int blockSize = 77;
	double C = 2;
	cv::adaptiveThreshold(adpt_threshold_img, adpt_threshold_img, maxValue, adaptiveMethod, thresholdType, blockSize, C);
	//display median blurred image
	cv::namedWindow("Adaptive Threshold", cv::WINDOW_AUTOSIZE);
	cv::imshow("Adaptive Threshold", adpt_threshold_img);




































	/*
	//reduce image to 10 dominate colors to help us pick out white part of eye
	int count = 3;

	//This function returns a vector that contains dominantc colors (see header for how it works)
	std::vector<cv::Vec3b> colors = find_dominant_colors(matImage, count);

	//create pallette image from the two extracted dominant colors
	cv::Mat palette = get_dominant_palette(colors);
	cv::imwrite("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/color_eye/output/palette.png", palette);

	//create image that's just a square the dominant color of white part of eye
	//cv::Mat dom_color = get_color_rect(skin_color);
	//cv::imwrite("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/color_eye/output/eyecolor.png", dom_color);
	//display dominate face color square
	//cv::namedWindow("Face Color", cv::WINDOW_AUTOSIZE);
	//cv::imshow("Face Color", dom_face);
	
	
	//rgb to hsv color
	cv::Mat dom_hsv;
	//cv::cvtColor(dom_color, dom_hsv, CV_BGR2HSV);

	//get the eye color
	cv::Vec3b eye_color;

	//for (int i = 0; i < count;i++) {
	//	if (colors[i][0] > 200 && colors[i][0] > 200 && colors[i][0] > 200) {
	//		printf("colors[i]: %d\n", colors[i][0]);
	//		skin_color = colors[i];
	//		printf("skin_color[0]: %d\n", skin_color[0]);
	//	}
	//}
	*/
	//wait to close display windows
	cv::waitKey();
	
	return 0;
}
