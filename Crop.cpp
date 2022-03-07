#include "Arguments.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <concurrent_vector.h>

using namespace cv;
using namespace std;

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

	bool notFinished = true;

	int crop[4];

	//cv::parallel_for_(Range(0, 4), [&](const Range& sideRange) {
	Concurrency::concurrent_vector<int> a;
	cv::parallel_for_(Range(0, image_size.width), [&](const Range& rangeHeight) {
		bool finished = false;

		for (int j = rangeHeight.start; j < rangeHeight.end; j += 2) {
			cv::parallel_for_(Range(0, image_size.height), [&](const Range& range) {
				for (int i = range.start; i < range.end && !finished; i++) {
					if (image.at<Vec4b>(i, j)[3] > 0) {
						a.push_back(i);
						finished = true;
					}
				}
				});
		}
		});
	cout << "vec_size: " << a.size() << endl;
	int min = image_size.height;
	for (auto i = a.begin(); i != a.end(); i++) {
		if (*i < min) {
			min = *i;
		}
	}
	cropTop = min;
	a.clear();



	/*cv::parallel_for_(Range(image_size.height-1, 0), [&](const Range& rangeHeight) {
		bool finished = false;
		for (int i = rangeHeight.start; i >= rangeHeight.end; i--) {

			cv::parallel_for_(Range(0, image_size.width), [&](const Range& range) {
				for (int j = range.start; j < range.end && !finished; j++) {
					if (image.at<Vec4b>(i, j)[3] > 0) {
						cropBottom = i;
						finished = true;
					}
				}
				});
		}
		});

	cv::parallel_for_(Range(0, image_size.width), [&](const Range& rangeHeight) {
		bool finished = false;
		for (int j = rangeHeight.start; j >= rangeHeight.end; j++) {

			cv::parallel_for_(Range(0, image_size.height), [&](const Range& range) {
				for (int i = range.start; i < range.end && !finished; i++) {
					if (image.at<Vec4b>(i, j)[3] > 0) {
						cropLeft = i;
						finished = true;
					}
				}
				});
		}
		});

	cv::parallel_for_(Range(image_size.width-1, 0), [&](const Range& rangeHeight) {
		bool finished = false;
		for (int j = rangeHeight.start; j >= rangeHeight.end; j--) {

			cv::parallel_for_(Range(0, image_size.height), [&](const Range& range) {
				for (int i = range.start; i < range.end && !finished; i++) {
					if (image.at<Vec4b>(i, j)[3] > 0) {
						cropRight = i;
						finished = true;
					}
				}
				});
		}
		});*/
		//});


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