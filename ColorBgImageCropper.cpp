#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;
int main(int argc, char** argv)
{
	if (argc != 2)
	{
		cout << " Usage: " << argv[0] << " ImageToLoadAndDisplay" << endl;
		return -1;
	}
	Mat image;
	image = imread(argv[1], IMREAD_UNCHANGED); // Read the file
	Mat newimg = Mat::zeros(image.rows, image.cols, image.type());
	if (image.empty()) // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	const Size image_size = image.size();

	int cropTop = 0;
	int cropBottom = image_size.height;
	int cropLeft = 0;
	int cropRight = image_size.width;

	bool notFinished = true;

	for (int i = 0; i < image_size.height && notFinished; i++) {
		for (int j = 0; j < image_size.width && notFinished; j++) {
			if (image.at<Vec4b>(i, j)[3] > 0) {
				cropTop = i;
				notFinished = false;
			}
		}
	}

	notFinished = true;
	for (int i = image_size.height - 1; i >= 0 && notFinished; i--) {
		for (int j = 0; j < image_size.width && notFinished; j++) {
			if (image.at<Vec4b>(i, j)[3] > 0) {
				cropBottom = i;
				notFinished = false;
			}
		}
	}

	notFinished = true;
	for (int j = 0; j < image_size.width && notFinished; j++) {
		for (int i = 0; i < image_size.height && notFinished; i++) {
			if (image.at<Vec4b>(i, j)[3] > 0) {
				cropLeft = j;
				notFinished = false;
			}
		}
	}

	notFinished = true;
	for (int j = image_size.width - 1; j >= 0 && notFinished; j--) {
		for (int i = 0; i < image_size.height && notFinished; i++) {
			if (image.at<Vec4b>(i, j)[3] > 0) {
				cropRight = j;
				notFinished = false;
			}
		}
	}

	Mat cropped = image(Range(cropTop, cropBottom), Range(cropLeft, cropRight));

	cout << "cropTop: " << cropTop << endl;
	cout << "cropBottom: " << cropBottom << endl;
	cout << "cropRight: " << cropRight << endl;
	cout << "cropLeft: " << cropLeft << endl;
	namedWindow("Original", WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Original", image); // Show our image inside it.

	namedWindow("Cropped", WINDOW_AUTOSIZE);
	imshow("Cropped", cropped);

	imwrite("output.png", cropped);
	waitKey(0); // Wait for a keystroke in the window
	return 0;
}