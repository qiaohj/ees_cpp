/**
 * @file CommonFun.h
 * @brief Class CommonFun. A class to implement the public functions common used in the application.
 * @author Huijie Qiao
 * @version 1.0
 * @date 11/25/2018
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
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

/**
 * @brief A class to implement the public functions common used in the application.
 */
class CommonFun {
public:
	/**
	 * @brief Convert the X, Y index to longitude and latitude.
	 * @param x X index
	 * @param y Y index
	 * @param fromWkt the source projection
	 * @param toWkt the target projection
	 */
	static void convert2LL(double* x, double* y, const char *fromWkt, const char *toWkt);
	/**
	 * @brief read the text file to a string
	 * @param path the path to the text file
	 * @return
	 */
    static std::string readFile(const char* path);
    /**
     * @brief remove the the given extension from a file name
     * @param path
     * @param extension
     * @return
     */
    static std::string removeSuffix(const std::string& path,
            const std::string& extension);
    /**
     * @brief read the JSON file to a Json object (to load the configuration of a scenario or a species from their configuration file)
     * @param path the path to the JSON file
     * @return a Json object
     */
    static Json::Value readJson(const char* path);
    /**
     * @brief output a string to a text file
     * @param s the string to save
     * @param path the path to the text file
     */
    static void writeFile(const std::string s, const char* path);
    /**
     * @brief create a folder
     * @param path the folder's path
     */
    static void createFolder(const char* path);
    /**
     * @brief output a string array to a text file
     * @param s the string array to save
     * @param path the path to the text file
     */
    static void writeFile(const std::vector<std::string> s, const char* path);
    /**
     * @brief convert longitude and latitude to X, Y index
     * @param adfGeoTransform The GeoTransform matrix of the raster layers
     * @param longitude longitude
     * @param latitude latitude
     * @param x X index
     * @param y Y index
     */
    static void LL2XY(const double* adfGeoTransform, const double longitude,
            const double latitude, unsigned* x, unsigned* y);
    /**
     * @brief convert X, Y index to longitude and latitude
     * @param adfGeoTransform The GeoTransform matrix of the raster layers
     * @param x X index
     * @param y Y index
     * @param longitude longitude
     * @param latitude latitude
     */
    static void XY2LL(const double* adfGeoTransform, const unsigned x,
            const unsigned y, double* longitude, double* latitude);

    /**
     * @brief convert the number in degrees to the radiant equivalent
     * @param deg the number in degrees
     * @return
     */
    static double deg2rad(double deg);
    /**
     * @brief calculate the great-circle distance between two points.
     * @param latitude1 latitude for point No.1
     * @param longitude1 longitude for point No.1
     * @param latitude2 latitude for point No.2
     * @param longitude2 longitude for point No.2
     * @return the great-circle distance between two points.
     */
    static double haversine_distance(double latitude1, double longitude1, double latitude2,
                              double longitude2);
    /**
     * @brief another method to calculate the great-circle distance between two points.
     * @param latitude1 latitude for point No.1
     * @param longitude1 longitude for point No.1
     * @param latitude2 latitude for point No.2
     * @param longitude2 longitude for point No.2
     * @return the great-circle distance between two points.
     */
    static double ArcInRadians(double latitude1, double longitude1, double latitude2,
            double longitude2);
    /**
     * @brief the 3rd method to calculate the great-circle distance between two points.
     * @param latitude1 latitude for point No.1
     * @param longitude1 longitude for point No.1
     * @param latitude2 latitude for point No.2
     * @param longitude2 longitude for point No.2
     * @return the great-circle distance between two points.
     */
    static double vincenty_distance(double latitude1, double longitude1, double latitude2,
                             double longitude2);
    /**
     * @brief add leading ZERO to a number and convert the number to a string with fixed length
     * @param value the number to convert
     * @param digits the length of the string
     * @return
     */
    static std::string fixedLength(int value, int digits);
    /**
     * @brief returns the peak (maximum so far) resident set size (physical * memory use) measured in bytes, or zero if the value cannot be determined on this OS.
     * @return
     */
    static size_t getPeakRSS();
    /**
     * @breif return the current used memory
     * @return
     */
    static size_t getCurrentRSS();
    /**
     * @brief output the current memory usage for the application
     * @param line
     * @param is
     * @param last
     * @return
     */
    static size_t writeMemoryUsage(unsigned line, bool is, size_t last);
    /**
     * @brief we have so many functions to calculate the great circle distance, but we used none of them so far.
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     * @param fromWkt
     * @param toWkt
     * @param geoTrans
     * @param resolution
     * @return
     */
    static double GreatCirleDistance(int x1, int y1, int x2, int y2, const char* fromWkt, const char* toWkt, const double* geoTrans, int resolution);
    /**
     * @brief we have so many functions to calculate the great circle distance, but we used none of them so far.
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     * @param poCT
     * @param geoTrans
     * @param resolution
     * @return
     */
    static double GreatCirleDistanceFast(int x1, int y1, int x2, int y2,
    		OGRCoordinateTransformation *poCT, const double* geoTrans, double resolution);
    /**
     * @brief calculate the Euclidean distance between to points.
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     * @return
     */
    template<typename T> static double EuclideanDistance(T x1, T y1, T x2,
            T y2);
    /**
     * @brief in C++, we can't compare the equivalence of two float/double variables. this function is used to detect the equivalence of two float/double variables under a given precision.
     * @param a
     * @param b
     * @return
     */
    template<typename T> static bool AlmostEqualRelative(T a, T b);
    /**
     * @brief convert any type to a string
     * @param t
     * @return
     */
    template<typename T> static std::string to_str(const T& t);
    /**
     * @brief clear a vector and release the memory
     * @param v
     */
    template<typename T> static void clearVector(T* v);
    /**
     * @brief clear a hash map and release the memory
     * @param v
     */
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
