/*
Name: main.cpp
Date: 01/2018
Author: Austin Pursley
Course: ECEN 403, Senior Design Smart Mirror

Purpose: Demonstrate my image processing functions to find skin lesions.
*/

#include "stdafx.h"
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "dirent.h"
#include "my_in_out_directory.hpp"
#include "lesion_localization.hpp"
#define _CRT_SECURE_NO_DEPRECATE


int Lesion::img_id = 0; //each image associated with image id (rn for directory names)
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
		//read image
		std::string img_file = filenames[i]; //e.g. name.jpg
		std::string img_dir(input_dir.c_str() + img_file);
		cv::Mat matImage = cv::imread(img_dir, cv::IMREAD_COLOR);
		if (!matImage.data) {
			std::cout << "Unable to open the file: " << img_dir;
			return 1;
		}

		//locate skin lesions
		std::vector<std::vector<cv::Point> > lesion_contours;
		int les_type = 0; //0 only option here
		lesion_contours = lesion_localization(matImage, les_type);
		
		//new id for next new image
		Lesion::img_id++;

		//look in lesion_localize.cpp for output/debug
	}
	
	return 0;
}
