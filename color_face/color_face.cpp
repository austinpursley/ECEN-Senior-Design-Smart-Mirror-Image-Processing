#include "stdafx.h"
#include <string>
#include <iostream>
#include "../dominant_color.h"

/*This program does...*/
int main(int argc, char** argv) {
	

	std::string imageName("C:/Users/Austin Pursley/Desktop/ECEN-403-Smart-Mirror-Image-Analysis/color_face/input/skin0_scaled.jpg"); // by default

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
			printf("colors[i]: %d\n", colors[i][0]);
			skin_color = colors[i];
			printf("skin_color[0]: %d\n", skin_color[0]);
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