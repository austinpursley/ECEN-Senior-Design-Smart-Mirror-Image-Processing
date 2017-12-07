#include "stdafx.h"
#include <string>
#include <iostream>
#include "../dominant_color.h"

double otsu_8u_with_mask(const cv::Mat1b src, const cv::Mat1b& mask)
{
	const int N = 256;
	int M = 0;
	int i, j, h[N] = { 0 };
	for (i = 0; i < src.rows; i++)
	{
		const uchar* psrc = src.ptr(i);
		const uchar* pmask = mask.ptr(i);
		for (j = 0; j < src.cols; j++)
		{
			if (pmask[j])
			{
				h[psrc[j]]++;
				++M;
			}
		}
	}

	double mu = 0, scale = 1. / (M);
	for (i = 0; i < N; i++)
		mu += i*(double)h[i];

	mu *= scale;
	double mu1 = 0, q1 = 0;
	double max_sigma = 0, max_val = 0;

	for (i = 0; i < N; i++)
	{
		double p_i, q2, mu2, sigma;

		p_i = h[i] * scale;
		mu1 *= q1;
		q1 += p_i;
		q2 = 1. - q1;

		if (std::min(q1, q2) < FLT_EPSILON || std::max(q1, q2) > 1. - FLT_EPSILON)
			continue;

		mu1 = (mu1 + i*p_i) / q1;
		mu2 = (mu - q1*mu1) / q2;
		sigma = q1*q2*(mu1 - mu2)*(mu1 - mu2);
		if (sigma > max_sigma)
		{
			max_sigma = sigma;
			max_val = i;
		}
	}
	return max_val;
}

double threshold_with_mask(cv::Mat1b& src, cv::Mat1b& dst, double thresh, double maxval, int type, const cv::Mat1b& mask = cv::Mat1b())
{
	if (mask.empty() || (mask.rows == src.rows && mask.cols == src.cols && cv::countNonZero(mask) == src.rows * src.cols))
	{
		// If empty mask, or all-white mask, use cv::threshold
		thresh = cv::threshold(src, dst, thresh, maxval, type);
	}
	else
	{
		// Use mask
		bool use_otsu = (type & cv::THRESH_OTSU) != 0;
		if (use_otsu)
		{
			// If OTSU, get thresh value on mask only
			thresh = otsu_8u_with_mask(src, mask);
			// Remove THRESH_OTSU from type
			type &= cv::THRESH_MASK;
		}

		// Apply cv::threshold on all image
		thresh = cv::threshold(src, dst, thresh, maxval, type);

		// Copy original image on inverted mask
		src.copyTo(dst, ~mask);
	}
	return thresh;
}

int main(int argc, char** argv) {


	std::string imageName("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/color_face/input/snapshot1_mancrop.jpg"); 
	std::string dir_out("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/color_face/output/");

	cv::Mat matImage = cv::imread(imageName);

	if (!matImage.data) {
		std::cout << "Unable to open the file: " << imageName;
		return 1;
	}

	
	// thresholding settings
	//int thresholdType = cv::THRESH_TOZERO;
	int thresholdType = cv::THRESH_BINARY;
	int otsu = thresholdType + cv::THRESH_OTSU;
	int otsu_max_value = 255;

	cv::Mat1b bgr[3];
	cv::Mat1b red_thresh;
	cv::Mat1b red_thresh2;
	
	split(matImage, bgr);
	cv::imwrite(dir_out + "0_red_.jpg", bgr[2]);
	cv::threshold(bgr[2], red_thresh, 0, otsu_max_value, thresholdType + cv::THRESH_OTSU);
	cv::imwrite(dir_out + "1_red_thresh.jpg", red_thresh);
	threshold_with_mask(bgr[2], red_thresh2, 0, otsu_max_value, thresholdType + cv::THRESH_OTSU, red_thresh);
	bitwise_and(red_thresh, red_thresh2, red_thresh2);
	cv::imwrite(dir_out + "2_red_thresh.jpg", red_thresh2);
	

	/*
	//cv::Mat red_image = matImage & cv::Scalar(0, 0, 255);
	//cv::Mat red_thresh;
	//cv::threshold(red_image, red_thresh, 0, otsu_max_value, thresholdType + cv::THRESH_OTSU);

	//two colors: color of skin and color of background (white)
	int count = 4;

	//This function returns a vector that contains two color vectors
	std::vector<cv::Vec3b> colors = find_dominant_colors(matImage, count);

	//select skin color, making sure it's not the white background color
	cv::Vec3b skin_color;
	for (int i = 0; i < count;i++) {
		if (colors[i][0] < 245 && colors[i][0] < 245 && colors[i][0] != 245) {
			skin_color = colors[i];
			//printf("skin_color[0]: %d\n", skin_color[0]);
		}
	}

	//create pallette image from the two extracted dominant colors
	cv::Mat dom = get_dominant_palette(colors);
	cv::imwrite("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/color_face/output/palette.png", dom);

	//create image that's just a square the dominant color of skin
	cv::Mat dom_face = get_color_rect(skin_color);
	cv::imwrite("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/color_face/output/facecolor.png", dom_face);

	//wait to close display windows
	cv::waitKey();
	*/
	return 0;
	
}
/*
void get_class_mean_var(cv::Mat img, cv::Mat classes, t_color_node *node) {
	const int width = img.cols;
	const int height = img.rows;
	const uchar classid = node->classid;
	//3x1 matrix and 3x3 matrix of 0s for mean and variance, repectively.
	cv::Mat mean = cv::Mat(3, 1, CV_64FC1, cv::Scalar(0));
	cv::Mat variance = cv::Mat(3, 3, CV_64FC1, cv::Scalar(0));

	double pixcount = 0; //track number of pixels in class
	for (int y = 0;y<height;y++) {
		//pointer to row
		cv::Vec3b* ptr = img.ptr<cv::Vec3b>(y);
		uchar* ptrClass = classes.ptr<uchar>(y);
		for (int x = 0;x<width;x++) {
			//skip pixel at (x,y) if it is not in the class associated with node
			if (ptrClass[x] != classid)
				continue;
			//The RGB vector of the pixel
			cv::Vec3b color = ptr[x];
			//scale RGB values to 0-1 range to avoid overflows
			cv::Mat scaled = cv::Mat(3, 1, CV_64FC1, cv::Scalar(0));
			scaled.at<double>(0) = color[0] / 255.0f;
			scaled.at<double>(1) = color[1] / 255.0f;
			scaled.at<double>(2) = color[2] / 255.0f;
			//See aishack tutorial for mean and variance forumals.
			//From that tutorial, these are Rn, mn, and Nn
			mean += scaled;
			variance = variance + (scaled * scaled.t());
			pixcount++;
		}
	}
	variance = variance - (mean * mean.t()) / pixcount;
	mean = mean / pixcount;

	// The node mean and variance
	node->mean = mean.clone();
	node->variance = variance.clone();

	return;
}
*/

/*This program does...*/
/*
int main(int argc, char** argv) {
	

	std::string imageName("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/color_face/input/skin1.jpg"); // by default

	//if (argc > 1)
	//{
	//	//I confingured this in Visual Studios debugging command arguments
	//	imageName = argv[1];
	//}

	cv::Mat matImage = cv::imread(imageName);

	if (!matImage.data) {
		std::cout << "Unable to open the file: " << imageName;
		return 1;
	}

	//two colors: color of skin and color of background (white)
	int count = 2;

	//This function returns a vector that contains two color vectors
	std::vector<cv::Vec3b> colors = find_dominant_colors(matImage, count);

	//select skin color, making sure it's not the white background color
	cv::Vec3b skin_color;
	for (int i = 0; i < count;i++) {
		if (colors[i][0] < 245 && colors[i][0] < 245 && colors[i][0] != 245) {
			skin_color = colors[i];
			//printf("skin_color[0]: %d\n", skin_color[0]);
		}
	}

	//display original image
	cv::namedWindow("Original Image", cv::WINDOW_AUTOSIZE);
	cv::imshow("Original Image", matImage);

	//create pallette image from the two extracted dominant colors
	cv::Mat dom = get_dominant_palette(colors);
	cv::imwrite("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/color_face/output/palette.png", dom);

	//create image that's just a square the dominant color of skin
	cv::Mat dom_face = get_color_rect(skin_color);
	cv::imwrite("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/color_face/output/facecolor.png", dom_face);
	//display dominate face color square
	cv::namedWindow("Face Color", cv::WINDOW_AUTOSIZE);
	cv::imshow("Face Color", dom_face);

	//wait to close display windows
	cv::waitKey();

	return 0;
}
\
*/