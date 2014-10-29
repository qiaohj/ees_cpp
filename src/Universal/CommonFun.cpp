/*
 * CommonFun.cpp
 *
 *  Created on: Oct 15, 2014
 *      Author: qiaohj
 */

#include "CommonFun.h"

std::string CommonFun::readFile(const char* path) {
    std::FILE* file = fopen(path, "rb");
    if (!file)
        return std::string("");
    std::fseek(file, 0, SEEK_END);
    long size = ftell(file);
    std::fseek(file, 0, SEEK_SET);
    std::string text;
    char* buffer = new char[size + 1];
    buffer[size] = 0;
    if (fread(buffer, 1, size, file) == (unsigned long) size)
        text = buffer;
    fclose(file);
    delete[] buffer;
    return text;
}

Json::Value CommonFun::readJson(const char* path) {
    Json::Features features;
    Json::Reader reader(features);
    Json::Value root;
    std::string input = readFile(path);
    bool parsingSuccessful = reader.parse(input, root);
    if (parsingSuccessful) {
        return root;
    } else {
        return root;
    }
}
std::string CommonFun::removeSuffix(const std::string& path, const std::string& extension) {
    if (extension.length() >= path.length())
        return std::string("");
    std::string suffix = path.substr(path.length() - extension.length());
    if (suffix != extension)
        return std::string("");
    return path.substr(0, path.length() - extension.length());
}
void CommonFun::writeFile(const std::string s, const char* path) {
    std::ofstream outfile(path);
    if (!outfile.is_open()) {
        std::cerr << "Couldn't open " << path << std::endl;
    }
    outfile << s << std::endl;
    outfile.close();
}
void CommonFun::writeFile(const std::vector<std::string> s, const char* path) {
    std::string joined = boost::algorithm::join(s, "\n");
    writeFile(joined, path);
}
void CommonFun::XY2LL(const double* adfGeoTransform, const unsigned x,
        const unsigned y, double* longitude, double* latitude) {
    *longitude = adfGeoTransform[0] + adfGeoTransform[1] * (x + .5f)
            + adfGeoTransform[2] * (y + .5f);
    *latitude = adfGeoTransform[3] + adfGeoTransform[4] * (x + .5f)
            + adfGeoTransform[5] * (y + .5f);
}

void CommonFun::LL2XY(const double* adfGeoTransform, const double longitude,
        const double latitude, unsigned* x, unsigned* y) {
    *x = (unsigned) ((longitude - adfGeoTransform[0]) / adfGeoTransform[1]);
    *y = (unsigned) ((latitude - adfGeoTransform[3]) / adfGeoTransform[5]);
}
void CommonFun::createFolder(const char* path) {
    if (!(boost::filesystem::exists(path))) {
        boost::filesystem::create_directory(path);
    }
}

std::string CommonFun::fixedLength(int value, int digits = 3) {
    unsigned int uvalue = value;
    if (value < 0) {
        uvalue = -uvalue;
    }
    std::string result;
    while (digits-- > 0) {
        result += ('0' + uvalue % 10);
        uvalue /= 10;
    }
    if (value < 0) {
        result += '-';
    }
    std::reverse(result.begin(), result.end());
    return result;
}


