/*
 * CommonFun.h
 *
 *  Created on: Oct 15, 2014
 *      Author: qiaohj
 */

#ifndef CommonFun_H
#define CommonFun_H

#include <gdal.h>
#include <gdal_priv.h>
#include <ogr_srs_api.h>
#include <ogr_spatialref.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <float.h>
#include <math.h>
#include <unistd.h>
#include <sys/resource.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread/thread.hpp>

#include "../JsonPaster/include/json/json.h"
#include "const.h"
#include "log.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI    3.1415926535897932384626433832795
#endif

#ifndef earth_radius_km
#define earth_radius_km    6371.0
#endif

class CommonFun {
public:
	static void convert2LL(double* x, double* y, const char *fromWkt, const char *toWkt);
    static std::string readFile(const char* path);
    static std::string removeSuffix(const std::string& path,
            const std::string& extension);
    static Json::Value readJson(const char* path);
    static void writeFile(const std::string s, const char* path);
    static void createFolder(const char* path);
    static void writeFile(const std::vector<std::string> s, const char* path);
    static void LL2XY(const double* adfGeoTransform, const double longitude,
            const double latitude, unsigned* x, unsigned* y);
    static void XY2LL(const double* adfGeoTransform, const unsigned x,
            const unsigned y, double* longitude, double* latitude);
    static double deg2rad(double deg);
    static double haversine_distance(double latitude1, double longitude1, double latitude2,
                              double longitude2);
    static double vincenty_distance(double latitude1, double longitude1, double latitude2,
                             double longitude2);
    static std::string fixedLength(int value, int digits);
    static size_t getPeakRSS();
    static size_t getCurrentRSS();
    static size_t writeMemoryUsage(unsigned line, bool is, size_t last);
    static double GreatCirleDistance(int x1, int y1, int x2, int y2, const char* fromWkt, const char* toWkt, const double* geoTrans, int resolution);
    static double GreatCirleDistanceFast(int x1, int y1, int x2, int y2,
    		OGRCoordinateTransformation *poCT, const double* geoTrans, int resolution);
    template<typename T> static double EuclideanDistance(T x1, T y1, T x2,
            T y2);
    template<typename T> static bool AlmostEqualRelative(T a, T b);
    template<typename T> static std::string to_str(const T& t);
    template<typename T> static void clearVector(T* v);
    template<typename T> static void clearUnordered_map(T* v);
};
template<typename T> void CommonFun::clearUnordered_map(T* v) {
    std::vector<unsigned> erased_key;
    for (auto it : *v) {
        erased_key.push_back(it.first);
    }
    for (auto key : erased_key) {
        if ((*v)[key] != NULL) {
            delete (*v)[key];
        }
        v->erase(key);
    }
    v->clear();
    erased_key.clear();
}

template<typename T> void CommonFun::clearVector(T* v) {
    for (typename T::iterator it = v->begin(); it != v->end(); ++it) {
        delete *it;
        *it = NULL;
    }
    v->clear();
    T().swap(*v);
}
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
template<typename T> std::string CommonFun::to_str(const T& t) {
    std::ostringstream os;
    os << t;
    return os.str();
}
#endif /* CommonFun_H */
