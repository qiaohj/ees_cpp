/*
 * CommonFun.h
 *
 *  Created on: Oct 15, 2014
 *      Author: qiaohj
 */

#ifndef CommonFun_H
#define CommonFun_H

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <float.h>
#include <math.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/filesystem.hpp>

#include "../JsonPaster/include/json/json.h"

using namespace std;
class CommonFun {
public:
    static string readFile(const char* path);
    static string removeSuffix(const string& path, const string& extension);
    static Json::Value readJson(const char* path);
    static void writeFile(const string s, const char* path);
    static void createFolder(const char* path);
    static void writeFile(const vector<string> s, const char* path);
    static void LL2XY(const double* adfGeoTransform, const double longitude,
            const double latitude, unsigned* x, unsigned* y);
    static void XY2LL(const double* adfGeoTransform, const unsigned x,
            const unsigned y, double* longitude, double* latitude);
    static string fixedLength(int value, int digits);
    template<typename T> static double EuclideanDistance(T x1, T y1, T x2,
            T y2);
    template<typename T> static bool AlmostEqualRelative(T a, T b);
    template<typename T> static string to_str(const T& t);
};
template<typename T> double CommonFun::EuclideanDistance(T x1, T y1, T x2,
        T y2) {
    T x = x1 - x2;
    T y = y1 - y2;
    double dist;
    dist = pow(x, 2) + pow(y, 2);
    dist = sqrt(dist);
    return dist;
}

template<typename T> bool CommonFun::AlmostEqualRelative(T a, T b) {
    return fabs(a - b) < FLT_EPSILON;
}
template<typename T> string CommonFun::to_str(const T& t) {
    ostringstream os;
    os << t;
    return os.str();
}
#endif /* CommonFun_H */
