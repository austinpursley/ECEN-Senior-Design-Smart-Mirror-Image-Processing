#define _CRT_SECURE_NO_DEPRECATE
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "stdafx.h"
#include <iostream>

#include "lesion_detection.h"
//#include "lesion_entropy_detect.h"
#include "lesion_id.h"
#include "dirent.h"


static std::string output_dir("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/lesions/output/");
static std::string input_dir("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/lesions/input/");

int main(int argc, char** argv) {
	
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

	//for each image in input directory
	for (int i = 2; i < filenames.size(); i++) {
		
		std::string img_file = filenames[i]; //e.g. name.jpg
		//we pass image name in functions below for the testing
		size_t lastindex = img_file.find_last_of(".");
		std::string img_name = img_file.substr(0, lastindex); //e.g. name
		
		//read image
		std::string img_dir(input_dir.c_str() + img_file);
		cv::Mat matImage = cv::imread(img_dir, cv::IMREAD_COLOR);
		if (!matImage.data) {
			std::cout << "Unable to open the file: " << img_dir;
			return 1;
		}

		//red lesions
		std::vector<std::vector<cv::Point> > red_lesion_contours;

		
		//detect lesions
		std::vector<std::vector<cv::Point> > lesion_contours;
		lesion_contours = lesion_detection(matImage, img_name);
		/*
		//entropy detect lesions
		std::vector<std::vector<cv::Point> > e_lesion_contours;
		e_lesion_contours = lesion_detect_entropy(matImage, img_name);

		
		//lesion features: area/size and mean color
		std::vector<double> les_areas;
		std::vector<cv::Scalar> les_colors;
		les_areas = lesion_areas(lesion_contours); //not in use right now....
		les_colors = lesion_colors(matImage, lesion_contours, img_name);

		//lesion classification: dark and red
		std::vector<int> lesion_ids;
		lesion_ids = lesion_id(les_colors, img_name);
		*/
	}
	
	return 0;
}
