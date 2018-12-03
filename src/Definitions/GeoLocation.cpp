/**
 * @file GeoLocation.cpp
 * @brief Class GeoLocation. A class to save the geographic location of each pixel
 * @author Huijie Qiao
 * @version 1.0
 * @date 11/25/2018
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */
#include "GeoLocation.h"

GeoLocation::GeoLocation(float p_longitude, float p_latitude) {
    longitude = p_longitude;
    latitude = p_latitude;

}

GeoLocation::~GeoLocation() {

}

float GeoLocation::getLongitude(){
    return longitude;
}
float GeoLocation::getLatitude(){
    return latitude;
}
