/*
 * CommonFun.cpp
 *
 *  Created on: Oct 15, 2014
 *      Author: qiaohj
 */

#include "CommonFun.h"

string CommonFun::readFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file)
        return string("");
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    string text;
    char* buffer = new char[size + 1];
    buffer[size] = 0;
    if (fread(buffer, 1, size, file) == (unsigned long) size)
        text = buffer;
    fclose(file);
    delete[] buffer;
    return text;
}

Json::Value CommonFun::readJson(const char* path){
	Json::Features features;
	Json::Reader reader(features);
	Json::Value root;
	string input = readFile(path);
	bool parsingSuccessful = reader.parse(input, root);
	if (parsingSuccessful){
	    return root;
	}else{
	    return root;
	}
}
string CommonFun::removeSuffix(const string& path, const string& extension) {
    if (extension.length() >= path.length())
        return string("");
    string suffix = path.substr(path.length() - extension.length());
    if (suffix != extension)
        return string("");
    return path.substr(0, path.length() - extension.length());
}
void CommonFun::writeFile(const string s, const char* path) {
    std::ofstream outfile(path);
    if (!outfile.is_open()) {
        std::cerr << "Couldn't open 'output.txt'" << std::endl;
    }
    outfile << s << std::endl;
    outfile.close();
}
void CommonFun::writeFile(const vector<string> s, const char* path){
    string joined = boost::algorithm::join(s, "\n");
    writeFile(joined, path);
}
