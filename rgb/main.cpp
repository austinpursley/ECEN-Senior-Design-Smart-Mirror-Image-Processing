// RGB_img_full_process.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "face_feature_extract/face_feature_extract.h"
#include "dirent.h"

int main()
{
	std::string dir = "C:/Users/Austin Pursley/Desktop/ECEN-Senior-Design-Smart-Mirror-Image-Processing/rgb/";
	std::string input_dir = dir + "input/";
	std::string output_dir = dir + "output/";

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
		cv::Mat img = cv::imread(img_dir, cv::IMREAD_COLOR);
		if (!img.data) {
			std::cout << "Unable to open the file: " << img_dir;
			return 1;
		}
		
		///STEP 1: detect the face, extract the eyes and mouth
		std::string face_cascade_name = "C:/Users/Austin Pursley/Documents/opencv/build/etc/haarcascades/haarcascade_frontalface_alt.xml";
		std::string eyes_cascade_name = "C:/Users/Austin Pursley/Documents/opencv/build/etc/haarcascades/haarcascade_eye.xml";
		std::string mouth_cascade_name = "C:/Users/Austin Pursley/Documents/opencv/build/etc/haarcascades/Mouth.xml";
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
		std::map<std::string, cv::Rect> features;
		if (!img.empty()) {
			get_face_features(img, face_cascade, eyes_cascade, mouth_cascade, features);
		}
		else {
			printf(" --(!) No captured frame -- Break!");
			exit(1);
		}

		cv::Mat face = img(features["face"]);

		double escale = 0.4;
		cv::Point center(features["eye1"].x + features["eye1"].width / 2, features["eye1"].y + features["eye1"].height / 2);
		cv::Size axes(features["eye1"].height * escale, features["eye1"].width * escale);
		ellipse(face, center, axes, 90, 0, 360, 0, -1);

		center = cv::Point(features["eye2"].x + features["eye2"].width / 2, features["eye2"].y + features["eye2"].height / 2);
		axes = cv::Size(features["eye2"].height * escale, features["eye2"].width * escale);
		ellipse(face, center, axes, 90, 0, 360, 0, -1);

		
		double mscale = 0.4;
		center = cv::Point(features["mouth"].x + features["mouth"].width / 2, features["mouth"].y + features["mouth"].height / 2);
		axes = cv::Size(features["mouth"].height * mscale, features["mouth"].width * mscale);
		ellipse(face, center, axes, 90, 0, 360, 0, -1);
		

		imshow("img", img);
		imshow("face", face);

		///NEXT/ISSUES: normalizing face, question of if should have "original" image and then have scaled image of face (face detection) that's used for all image processing.
		///probably have to add new output to face feature function (or break that function up?)

	}
    return 0;
}

