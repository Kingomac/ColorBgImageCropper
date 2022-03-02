#include <string>
#include <list>

using namespace std;

list<string> strsplit(string str, string to_split) {
	size_t i = str.find(to_split);
	list<string> toret = {};
	while (i != string::npos) {
		toret.push_back(str.substr(0, i));
		str = str.substr(size_t(i + 1));
		i = str.find(to_split);
	}
	toret.push_back(str);
	return toret;
}