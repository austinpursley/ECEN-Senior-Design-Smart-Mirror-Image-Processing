/*
Name: main.cpp
Date: 05/2018
Author: Austin Pursley
Course: ECEN 403/404, Senior Design Smart Mirror

Purpose: The visible light facial image processing for my senior design project.
*/
#define _CRT_SECURE_NO_WARNINGS
#include "dirent.h"
#include "stdafx.h"
#include "skin_extract/face_feature_extract.hpp"
#include "skin_extract/skin_extract.hpp"
#include "lesions/lesion_localization.hpp"

// Each image associated with image id (rn for directory names)
int Lesion::img_id = 0; 
int main()
{
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
		std::string text_file = output_dir + img_name + "_rgb_output.txt";
		oFile = fopen(text_file.c_str(), "w");

		/// STEP 1: detect the face, extract the eyes and mouth
		std::map<std::string, cv::Rect> face_featrs;
		if (!img.empty()) {
			if (img.cols / img.rows != 1 || img.rows / img.cols != 1) {
				cv::Rect roi(img.cols*0.125, 0, img.cols*0.75, img.rows);
				img = img(roi);
			}
			std::string face_cascade_name = dir + "/skin_extract/haarcascades/haarcascade_frontalface_alt.xml";
			std::string eyes_cascade_name = dir + "/skin_extract/haarcascades/haarcascade_eye.xml";
			std::string mouth_cascade_name = dir + "/skin_extract/haarcascades/Mouth.xml";
			cv::CascadeClassifier face_cascade;
			cv::CascadeClassifier eyes_cascade;
			cv::CascadeClassifier mouth_cascade;
			// Load the face cascade
			if (!face_cascade.load(face_cascade_name)) {
				printf("--(!)Error loading face cascade\n");
				return (-1);
			}
			// Load the eyes cascade
			if (!eyes_cascade.load(eyes_cascade_name)) {
				printf("--(!)Error loading eyes cascade\n");
				return (-1);
			}
			// Load the mouth cascade
			if (!mouth_cascade.load(mouth_cascade_name)) {
				printf("--(!)Error loading mouth cascade\n");
				return (-1);
			}
			// Apply the classifier to the frame
			get_face_features(img, face_cascade, eyes_cascade, mouth_cascade, face_featrs);
		}
		else {
			printf(" --(!) No image -- Break!");
			exit(1);
		}
		// Check if no face was found
		if (face_featrs["face"] == cv::Rect(0, 0, 0, 0)) {
			printf("NO face detected! \n");
			fprintf(oFile, "Face detected: no \n\n");
			std::cout << "END image processing for: " << img_file.c_str() << "\n\n";
			continue;
		}
		fprintf(oFile, "Face detected: yes \n");
		cv::Mat face = img(face_featrs["face"]);
		cv::Mat face_masked;
		mask_features(face, face_featrs, face_masked);
		cv::imwrite(output_dir + "/1_face_feature_extraction/" + img_name + "_0_img.jpg", img);
		cv::imwrite(output_dir + "/1_face_feature_extraction/" + img_name + "_1_face.jpg", face);
		cv::imwrite(output_dir + "/1_face_feature_extraction/" + img_name + "_2_face_features_masked.jpg", face_masked);

		/// STEP 2: SKIN EXTRACTION
		cv::Mat skin_mask, skin;
		extract_skin(img, face_featrs, skin_mask);
		face.copyTo(skin, skin_mask);
		resize(skin, skin, cv::Size(500, 500), 0, 0, cv::INTER_LINEAR);
		cv::Scalar skin_color = cv::mean(face, skin_mask);
		int skin_blue = skin_color[0];
		int skin_green = skin_color[1];
		int skin_red = skin_color[2];
		fprintf(oFile,"Skin Color: #%d %d %d \n", skin_red, skin_green, skin_blue);
		cv::imwrite(output_dir + "/2_skin_extraction/" + img_name + "_3_skin_mask.jpg", skin_mask);
		cv::imwrite(output_dir + "/2_skin_extraction/" + img_name + "_4_skin.jpg", skin);

		/// STEP 3: LESION LOCALIZATION
		std::vector<Lesion> lesions;
		int les_type = 0;
		lesion_localization(skin, lesions, les_type);
		int les_count = lesions.size();
		fprintf(oFile,"Lesion count: %d \n", les_count);

		/// STEP 4: LESION CLASSIFICATION
		int drk_les_cnt = 0;
		int red_les_cnt = 0;
		for (int i = 0; i < lesions.size(); i++) {
			int les_class = lesions[i].get_lesion_class();
			std::vector<std::vector<cv::Point>> contour{ lesions[i].get_contour() };
			if (les_class == 0) {
				drk_les_cnt++;
				cv::drawContours(skin, contour, 0, cv::Scalar(0,0,0), 1);
			}
			if (les_class == 1) {
				red_les_cnt++;
				cv::drawContours(skin, contour, 0, cv::Scalar(0,0,255), 1);
			}
		}
		cv::imwrite(output_dir + "/4_lesion_classify/" + img_name + "_classify.jpg", skin);
		fprintf(oFile,"Dark lesion count: %d \n", drk_les_cnt);
		fprintf(oFile,"Red lesion count: %d \n\n", red_les_cnt);
		std::cout << "END image processing for: " << img_file.c_str() << "\n\n";
		Lesion::img_id++;
	}
	return 0;
}