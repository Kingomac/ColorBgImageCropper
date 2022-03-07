#include <iostream>
#include <fstream>
#include "Arguments.hpp"
#include "Crop.hpp"
using namespace cv;
using namespace std;
int main(int argc, char** argv)
{

	if (argc <= 1)
	{
		cout << "usage: autocroper" << endl;
		cout << "\t-i/input input_file: path to image to be cropped" << endl;
		cout << "\t-b/bg color: color to be ignored in format 255,0,0 or transparent" << endl;
		cout << "\toutput_file: at last the output file path (don't write argument name)" << endl;
	}

	ARGUMENTS args = parse_arguments(argc, argv);

	cout << "input_file: " << args.input_file << endl;
	cout << "bg_color: " << int(args.bg_color[0]) << "," << int(args.bg_color[1]) << "," << int(args.bg_color[2]) << endl;
	cout << "transparent: " << args.transparent << endl;
	cout << "output: " << args.output_file << endl;

	ifstream infile(args.input_file);
	if (!infile.good()) {
		cout << "Cannot read input file" << endl;
		return -1;
	}

	crop_transparent(args);
	

	return 0;
}