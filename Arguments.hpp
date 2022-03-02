#pragma once
#include <string>
#include <opencv2/core/matx.hpp>

struct ARGUMENTS
{
	std::string input_file;
	std::string output_file;
	cv::Vec3b bg_color;
	bool transparent;
};

ARGUMENTS parse_arguments(int argc, char** args);