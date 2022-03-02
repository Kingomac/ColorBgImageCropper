#include "Arguments.hpp";
#include <string>
#include <iostream>
#include <opencv2/core/matx.hpp>
#include "util.hpp";
#include <list>

using namespace std;

ARGUMENTS parse_arguments(int argc, char** args)
{
	ARGUMENTS parseds;
	for (int i = 1; i < argc; i++)
	{
		string a = args[i];
		if (!a.starts_with("-"))
		{
			parseds.output_file = a;
			return parseds;
		}
		string b;
		if (a.starts_with("--"))
			b = a.substr(2);
		else
			b = a.substr(1);

		i++;
		if (i >= argc)
		{
			cout << "Wrong arguments. Pass no arguments to read help" << endl;
			::exit(-1);
		}
		if (b.compare("i") == 0 || b.compare("input") == 0)
		{
			parseds.input_file = args[i];
		}
		else if (b.compare("b") == 0 || b.compare("bg") == 0)
		{
			if (strcmp(args[i], "-1") == 0) {
				parseds.transparent = true;
				parseds.bg_color = cv::Vec3b(255, 255, 255);
			}
			else {
				list rgb = strsplit(args[i], ",");
				if (rgb.size() < 3 || rgb.size() > 4) {
					cout << "Incorrect color format" << endl;
					::exit(-1);
				}
				cv::Vec3b vec;
				int i = 0;
				for (string component : rgb) {
					int val = stoi(component);
					vec[i] = val;
					i++;
				}
				parseds.bg_color = vec;
			}
		}
		else
		{
			cout << "What is \"" << args[i] << "\"?" << endl;
			::exit(-1);
		}
	}
	cout << "Output as last argument required" << endl;
	::exit(-1);
}
