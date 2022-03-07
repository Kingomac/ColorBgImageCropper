#include "Arguments.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <concurrent_vector.h>
#include <limits>

using namespace cv;
using namespace std;

unsigned int get_min(Concurrency::concurrent_vector<unsigned int> *a) {
	unsigned int min = UINT_MAX;
	for (auto i = a->begin(); i != a->end(); i++) {
		if (*i < min) {
			min = *i;
		}
	}
	return min;
}

unsigned int get_max(Concurrency::concurrent_vector<unsigned int>* a) {
	unsigned int max = 0;
	for (auto i = a->begin(); i != a->end(); i++) {
		if (*i > max) {
			max = *i;
		}
	}
	return max;
}

void crop_transparent(ARGUMENTS args) {
	auto t1 = chrono::high_resolution_clock::now();
	Mat image;
	image = imread(args.input_file, IMREAD_UNCHANGED); // Read the file
	Mat newimg = Mat::zeros(image.rows, image.cols, image.type());
	if (image.empty()) // Check for invalid input
	{
		cout << "Could not open the image" << endl;
		exit(-1);
	}

	const Size image_size = image.size();

	int cropTop = 0;
	int cropBottom = image_size.height;
	int cropLeft = 0;
	int cropRight = image_size.width;

	//cv::parallel_for_(Range(0, 4), [&](const Range& sideRange) {
	Concurrency::concurrent_vector<unsigned int> a;
	cv::parallel_for_(Range(0, image_size.width), [&](const Range& rangeHeight) {

		for (int j = rangeHeight.start; j < rangeHeight.end; j++) {
			cv::parallel_for_(Range(0, image_size.height), [&](const Range& range) {
				bool finished = false;
				for (int i = range.start; i < range.end && !finished; i++) {
					if (image.at<Vec4b>(i, j)[3] > 0) {
						a.push_back(i);
						finished = true;
					}
				}
				});
		}
		});
	cropTop = get_min(&a);
	a.clear();

	cv::parallel_for_(Range(0, image_size.width), [&](const Range& rangeHeight) {

		for (int j = rangeHeight.start; j < rangeHeight.end; j++) {
			cv::parallel_for_(Range(image_size.height-1, 0), [&](const Range& range) {
				bool finished = false;
				for (int i = range.start; i >= range.end && !finished; i--) {
					if (image.at<Vec4b>(i, j)[3] > 0) {
						a.push_back(i);
						finished = true;
					}
				}
				});
		}
		});
	cropBottom = get_max(&a)+1;
	a.clear();

	cv::parallel_for_(Range(0, image_size.height), [&](const Range& rangeHeight) {

		for (int j = rangeHeight.start; j < rangeHeight.end; j++) {
			cv::parallel_for_(Range(0, image_size.width), [&](const Range& range) {
				bool finished = false;
				for (int i = range.start; i < range.end && !finished; i++) {
					if (image.at<Vec4b>(i, j)[3] > 0) {
						a.push_back(j);
						finished = true;
					}
				}
				});
		}
		});
	cropLeft = get_min(&a);
	a.clear();

	cv::parallel_for_(Range(image_size.width-1, 0), [&](const Range& rangeHeight) {

		for (int j = rangeHeight.start; j >= rangeHeight.end; j--) {
			cv::parallel_for_(Range(0, image_size.height), [&](const Range& range) {
				bool finished = false;
				for (int i = range.start; i < range.end && !finished; i++) {
					if (image.at<Vec4b>(i, j)[3] > 0) {
						a.push_back(j);
						finished = true;
					}
				}
				});
		}
		});
	cout << "vsize: " << a.size() << endl;
	cropRight = get_max(&a);
	a.clear();

	Mat cropped = image(Range(cropTop, cropBottom), Range(cropLeft, cropRight));

	auto t2 = chrono::high_resolution_clock::now();

	auto ms = chrono::duration_cast<chrono::milliseconds>(t2 - t1);

	cout << "Execution time: " << ms.count() << "ms" << endl;

	cout << "cropTop: " << cropTop << endl;
	cout << "cropBottom: " << cropBottom << endl;
	cout << "cropRight: " << cropRight << endl;
	cout << "cropLeft: " << cropLeft << endl;
	namedWindow("Original", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Original", image); // Show our image inside it.

	namedWindow("Cropped", WINDOW_AUTOSIZE);
	imshow("Cropped", cropped);

	imwrite("output.png", cropped);
	waitKey(0); // Wait for a keystroke in the window�
	destroyAllWindows();
	cropped.release();
}

void crop_transparent2(ARGUMENTS args) {
	auto t1 = chrono::high_resolution_clock::now();
	Mat image;
	image = imread(args.input_file, IMREAD_UNCHANGED); // Read the file
	Mat newimg = Mat::zeros(image.rows, image.cols, image.type());
	if (image.empty()) // Check for invalid input
	{
		cout << "Could not open the image" << endl;
		exit(-1);
	}

	const Size image_size = image.size();

	int cropTop = 0;
	int cropBottom = image_size.height;
	int cropLeft = 0;
	int cropRight = image_size.width;

	bool finish = false;

	/*for (int row = 0; row < image_size.width && !finish; row++) {
		for (int col = 0; col < image_size.height && !finish; col++) {
			if (image.at<Vec4b>(row, col)[3] > 0) {
				cropTop = row;
				finish = true;
			}
		}
	}*/

	for (int col = 0; col < image_size.height && !finish; col++) {
		for (int row = 0; row < image_size.width && !finish; row++) {
			if (image.at<Vec4b>(row, col)[3] > 0) {
				cropTop = row;
				finish = true;
			}
		}
	}

	finish = false;
	for (int j = image_size.height-1; j >= 0 && !finish; j--) {
		bool infinish = false;
		for (int i = 0; i < image_size.width && !infinish; i++) {
			if (image.at<Vec4b>(i, j)[3] > 0) {
				//cropBottom = i;
				finish = true;
			}
		}
	}

	finish = false;
	for (int i = 0; i < image_size.width && !finish; i++) {
		bool infinish = false;
		for (int j = 0; j < image_size.height && !infinish; j++) {
			if (image.at<Vec4b>(i, j)[3] > 0) {
				cropLeft = i;
				finish = true;
			}
		}
	}

	finish = false;
	for (int i = image_size.width-1; i >= 0 && !finish; i--) {
		bool infinish = false;
		for (int j = 0; j < image_size.height && !infinish; j++) {
			if (image.at<Vec4b>(i, j)[3] > 0) {
				cropRight = i;
				finish = true;
			}
		}
	}

	Mat cropped = image(Range(cropTop, cropBottom), Range(cropLeft, cropRight));

	auto t2 = chrono::high_resolution_clock::now();

	auto ms = chrono::duration_cast<chrono::milliseconds>(t2 - t1);

	cout << "Execution time: " << ms.count() << "ms" << endl;

	cout << "cropTop: " << cropTop << endl;
	cout << "cropBottom: " << cropBottom << endl;
	cout << "cropRight: " << cropRight << endl;
	cout << "cropLeft: " << cropLeft << endl;
	namedWindow("Original", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Original", image); // Show our image inside it.

	namedWindow("Cropped", WINDOW_AUTOSIZE);
	imshow("Cropped", cropped);

	imwrite("output.png", cropped);
	waitKey(0); // Wait for a keystroke in the window�
	destroyAllWindows();
	cropped.release();
}