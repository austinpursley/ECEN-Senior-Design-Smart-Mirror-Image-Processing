#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "dirent.h"
#include "face_feature_extract.hpp"
/*
Name: main.cpp
Date: 06/2018
Author: Austin Pursley
Course: ECEN 403/404, Senior Design Smart Mirror

Purpose: Demo of skin extraction image processing for senior design.
*/

#include "skin_extract.hpp"

int main()
{
	std::string input_dir("C:/Users/Austin Pursley/Projects/Senior Design/ECEN-Senior-Design-Smart-Mirror-Image-Processing/rgb/skin_extract/input/");
	std::string output_dir("C:/Users/Austin Pursley/Projects/Senior Design/ECEN-Senior-Design-Smart-Mirror-Image-Processing/rgb/skin_extract/output/");
	std::string dir("C:/Users/Austin Pursley/Projects/Senior Design/ECEN-Senior-Design-Smart-Mirror-Image-Processing/rgb/skin_extract/");
	// Get list of images in input direcotry (edit in my_in_out_directory.cpp)
	DIR *dpdf;
	struct dirent *epdf;
	std::vector<std::string> filenames;
	dpdf = opendir(input_dir.c_str());
	if (dpdf != NULL) {
		while (epdf = readdir(dpdf)) {
			filenames.push_back(std::string(epdf->d_name));
		}
	}
	// For each image in input directory...
	for (int i = 2; i < filenames.size(); i++) {
		// Read image
		std::string img_file = filenames[i]; //e.g. name.jpg
		size_t lastindex = img_file.find_last_of(".");
		std::string img_name = img_file.substr(0, lastindex); //e.g name (no .jpg extension)
		std::string img_dir(input_dir.c_str() + img_file);
		cv::Mat img = cv::imread(img_dir);
		std::cout << "START image processing for: " << img_file.c_str() << "\n";
		if (!img.data) {
			std::cout << "Unable to open the file: " << img_dir << '\n';
			return 1;
		}
		// File to write outputs
		FILE * oFile;
		std::string text_file = output_dir + "/2_skin_extract/" + img_name + "_rgb_output.txt";
		oFile = fopen(text_file.c_str(), "w");

		/// STEP 1: detect the face, extract the eyes and mouth
		std::map<std::string, cv::Rect> face_featrs;
		if (!img.empty()) {
			if (img.cols / img.rows != 1 || img.rows / img.cols != 1) {
				cv::Rect roi(img.cols*0.125, 0, img.cols*0.75, img.rows);
				img = img(roi);
			}
			std::string face_cascade_name = dir + "/haarcascades/haarcascade_frontalface_alt.xml";
			std::string eyes_cascade_name = dir + "/haarcascades/haarcascade_eye.xml";
			std::string mouth_cascade_name = dir + "/haarcascades/Mouth.xml";
			cv::CascadeClassifier face_cascade;
			cv::CascadeClassifier eyes_cascade;
			cv::CascadeClassifier mouth_cascade;
			if (!face_cascade.load(face_cascade_name)) {
				printf("--(!)Error loading face cascade\n");
				return (-1);
			}
			if (!eyes_cascade.load(eyes_cascade_name)) {
				printf("--(!)Error loading eyes cascade\n");
				return (-1);
			}
			if (!mouth_cascade.load(mouth_cascade_name)) {
				printf("--(!)Error loading mouth cascade\n");
				return (-1);
			}
			// Apply the classifiers to the image to extract features
			get_face_features(img, face_cascade, eyes_cascade, mouth_cascade, face_featrs);
		}
		else {
			printf(" --(!) No image -- Break!");
			exit(1);
		}
		// Check if no face was found
		if (face_featrs["face"] == cv::Rect(0, 0, 0, 0)) {
			printf("NO face detected\n");
			fprintf(oFile, "Face detected: no \n\n");
			std::cout << "END image processing for: " << img_file.c_str() << "\n\n";
			continue;
		}
		fprintf(oFile, "Face detected: yes \n");
		cv::Mat face = img(face_featrs["face"]);
		cv::Mat face_masked;
		mask_features(face, face_featrs, face_masked);
		// Output results for feature extractions
		cv::imwrite(output_dir + "/1_feat_extract/" + img_name + "_0_img.jpg", img);
		cv::imwrite(output_dir + "/1_feat_extract/" + img_name + "_1_face.jpg", face);
		cv::imwrite(output_dir + "/1_feat_extract/" + img_name + "_2_face_features_masked.jpg", face_masked);

		/// STEP 2: SKIN EXTRACTION
		cv::Mat skin_face, skin;
		extract_skin(img, face_featrs, skin_face);
		cv::imwrite(output_dir + "/2_skin_extract/" + img_name + "_4_face.jpg", skin_face);
		// Mask features (mouth and eyes)
		cv::Mat feat_masked, gray_img, thresh_img, skin_mask;
		mask_features(skin_face, face_featrs, feat_masked);
		cvtColor(feat_masked, gray_img, cv::COLOR_BGR2GRAY);
		cv::threshold(gray_img, thresh_img, 1, 255, cv::THRESH_BINARY);
		thresh_img.convertTo(skin_mask, CV_8U);

		face.copyTo(skin, skin_mask);
		resize(skin, skin, cv::Size(500, 500), 0, 0, cv::INTER_LINEAR);
		cv::Scalar skin_color = cv::mean(face, skin_mask);
		int skin_blue = skin_color[0];
		int skin_green = skin_color[1];
		int skin_red = skin_color[2];
		// Output results for skin extractions
		fprintf(oFile, "Skin Color: #%d %d %d \n", skin_red, skin_green, skin_blue);
		cv::imwrite(output_dir + "/2_skin_extract/" + img_name + "_5_skin_mask.jpg", skin_mask);
		cv::imwrite(output_dir + "/2_skin_extract/" + img_name + "_6_skin.jpg", skin);
	}
	return 0;
}