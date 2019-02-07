
#include "stdafx.h"
#include "opencv.hpp"
#pragma comment(lib, "opencv_world320.lib")

void read_barcode(cv::Mat &);
int main(){
	cv::Mat image;
	image = cv::imread("C:\\Users\\Ali\\Desktop\\upca.png");
	read_barcode(image);
	cv::imwrite("C:\\Users\\Ali\\Desktop\\bar.jpg", image);
	cv::imshow("aa", image);
	cv::waitKey(0);
	return 0;
}

#define SPACE 0
#define BAR 255

void ignore_begin(const cv::Mat_<uchar>& image, cv::Point& cur){
	while (image(cur) == SPACE)
		cur.x++;
}

void skip_middle(const cv::Mat_<uchar>& image, cv::Point& cur){
	int pattern[5] = { SPACE, BAR, SPACE, BAR, SPACE };
	for (int i = 0; i < 5; i++){
		while (image(cur) == pattern[i])
			cur.x++;
	}
}

enum direction {
	LEFT = 0,
	RIGHT
};

int get_digit(int code, int direction){
	if (direction == LEFT)
		switch (code){
		case 13:
			return 0;
		case 25:
			return 1;
		case 19:
			return 2;
		case 61:
			return 3;
		case 35:
			return 4;
		case 49:
			return 5;
		case 47:
			return 6;
		case 59:
			return 7;
		case 55:
			return 8;
		case 11:
			return 9;
	}
	else
		switch (code){
		case 114:
			return 0;
		case 102:
			return 1;
		case 108:
			return 2;
		case 66:
			return 3;
		case 92:
			return 4;
		case 78:
			return 5;
		case 80:
			return 6;
		case 68:
			return 7;
		case 72:
			return 8;
		case 116:
			return 9;
	}
	return 0;

}
int read_digit(const cv::Mat_<uchar>& image, cv::Point& cur, int usize, int direction){
	int bit[7] = { 0 };
	int code = 0;
	for (int i = 0; i < 7; i++){
		for (int j = 0; j < usize; j++){
			if (image(cur) == BAR)
				bit[i]++;
			cur.x++;
		}
		bit[i] = bit[i] > usize / 2 ? 1 : 0;

		int x = cur.x; int c = 0;

		if (image(cur) == bit[i] * 255){ // آيا در جايي که هستي هنوز همرنگ مقدار بدست آمده هستي؟ اگر هستي برو جلو که به رنگ مخالف برسي (در هنگامي که خط بزرگتر است)
			while (image(cur) == bit[i] * 255){
				cur.x++;
				c++;
			}
			if (c >= usize / 2)
				cur.x = x;
		}
		else { // خط کوچکتر است آنرا تراز ميکنيم!
			while (image(cur) != bit[i] * 255){
				cur.x--;
				c++;
			}

			c--;
			cur.x++;

			if (c >= usize / 2)
				cur.x = x;
		}

	}
	for (int i = 0; i < 7; i++){
		//std::cout << bit[i]; std::cout << " ";
		code *= 2;
		code += bit[i];
	}
	//std::cout << "code = " << code << " & mount = " << get_digit(code, direction)  << std::endl;
	return get_digit(code, direction);

}

int calc_unit_size(const cv::Mat_<uchar>& image, cv::Point& cur){
	int counter = 0;
	int pattern[3] = { BAR, SPACE, BAR };
	for (int i = 0; i < 3; i++){
		while (image(cur) == pattern[i]){
			cur.x++;
			counter++;
		}
	}
	return counter / 3;
}


void read_barcode(cv::Mat& image){
	cv::bitwise_not(image, image);
	cv::threshold(image, image, 128, 255, cv::THRESH_BINARY);
	cv::Size size = image.size();
	cv::Point cur = cv::Point(0, size.height / 2);

	ignore_begin(image, cur);

	int unit_size = calc_unit_size(image, cur);
	std::cout << "unit size = " << unit_size << std::endl;

	unsigned long int serial = 0;

	for (int i = 0; i < 6; i++)
		std::cout << read_digit(image, cur, unit_size, LEFT) % 10;


	skip_middle(image, cur);

	for (int i = 0; i < 6; i++)
		std::cout << read_digit(image, cur, unit_size, RIGHT) % 10;
}