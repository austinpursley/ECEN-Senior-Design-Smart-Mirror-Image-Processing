#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "stdafx.h"
#include <iostream>


int main(int argc, char** argv) {
	std::string image_name("les_skin1.jpg");
	std::string image_file("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/data/lesions/" + image_name);
	std::string image_out("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/data/lesions/output/");
	
	cv::Mat matImage = cv::imread(image_file, cv::IMREAD_COLOR);

	if (!matImage.data) {
		std::cout << "Unable to open the file: " << image_file;
		return 1;

	}

	//display original image
	//cv::namedWindow("Original Image", cv::WINDOW_AUTOSIZE);
	//cv::imshow("Original Image", matImage);

	//Guassian Blur
	cv::Size ksize;
	ksize.height = 11;
	ksize.width = ksize.height;
	//Thresholding
	int thresh = 0;
	int maxValue = 255;
	int thresholdType = cv::THRESH_BINARY_INV;
	int adaptMethod = cv::ADAPTIVE_THRESH_GAUSSIAN_C;
	int blocksize = 19;
	//Erosion
	int erosion_type = cv::MORPH_ELLIPSE;
	int erosion_size = 2;
	cv::Mat element = cv::getStructuringElement(erosion_type,
		cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		cv::Point(erosion_size, erosion_size));
	/*
	 * Green and Red Image
	 */
	//remove blue channel
	cv::Mat gr_image = matImage & cv::Scalar(0, 255, 255);
	//cv::namedWindow("remove blue", cv::WINDOW_AUTOSIZE);
	//cv::imshow("remove blue", gr_image);
	cv::cvtColor(gr_image, gr_image, CV_BGR2GRAY);
	cv::GaussianBlur(gr_image, gr_image, ksize, 0);
	cv::adaptiveThreshold(gr_image, gr_image, maxValue, adaptMethod, thresholdType, blocksize, 2);
	cv::imwrite(image_out + "greenred_thresh.jpg", gr_image);
	cv::erode(gr_image, gr_image, element);
	cv::dilate(gr_image, gr_image, element);
	cv::bitwise_not(gr_image, gr_image);
	cv::Mat masked, color;
	cv::cvtColor(gr_image, color, CV_GRAY2BGR);
	cv::bitwise_and(color, matImage, masked);
	//write image
	cv::imwrite(image_out + "greenred_erode_dilate.jpg", gr_image);
	cv::imwrite(image_out + "masked.jpg", masked);
	
	/*
	 * Gray image
	/*
	cv::Mat gray;
	cvtColor(matImage, gray, CV_BGR2GRAY);
	cv::GaussianBlur(gray, gray, ksize, 0);
	cv::threshold(gray, gray, thresh, maxValue, thresholdType + cv::THRESH_OTSU);
	cv::adaptiveThreshold(gray, gray, maxValue, adaptMethod, thresholdType, blocksize, 2);
	cv::imwrite(image_out + "gray.jpg", gray);
	*/


	/*
	cv::namedWindow("Threshold", cv::WINDOW_AUTOSIZE);
	cv::imshow("Threshold", gray);
	cv::imshow("blue", cv::WINDOW_AUTOSIZE);
	cv::imshow("blue", rgbChannels[0]);
	cv::imshow("green", cv::WINDOW_AUTOSIZE);
	cv::imshow("green", rgbChannels[1]);
	cv::imshow("red", cv::WINDOW_AUTOSIZE);
	cv::imshow("red", rgbChannels[2]);
	*/

	/*
	cv::Mat src_gray, edge, draw, blur;

	//convert to gray
	cvtColor(matImage, src_gray, CV_BGR2GRAY);

	//apply blur
	GaussianBlur(src_gray, blur, cv::Size(5, 5), 2, 2);
	cv::namedWindow("blurred", CV_WINDOW_AUTOSIZE);
	cv::imshow("blurred", blur);


	/// Show your results
	cv::namedWindow("Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE);
	cv::imshow("Hough Circle Transform Demo", matImage);
	*/

	cv::waitKey();
	return 0;
}

















/*
//split image
std::vector<cv::Mat> rgbChannels(3);
cv::split(matImage, rgbChannels);

//rgb channels
for (unsigned int i = 0; i < 3;i++) {
cv::GaussianBlur(rgbChannels[i], rgbChannels[i], ksize, 0);
}

//rgb channels
for (unsigned int i = 0; i < 3;i++) {
cv::adaptiveThreshold(rgbChannels[i], rgbChannels[i], maxValue, adaptMethod, thresholdType, 11, 2);
//cv::threshold(rgbChannels[i], rgbChannels[i], thresh, maxValue, thresholdType + cv::THRESH_OTSU);
cv::imwrite(image_out + "rgbchannel" + std::to_string(i) + ".jpg", rgbChannels[i]);
}
*/