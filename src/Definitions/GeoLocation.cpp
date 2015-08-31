/*
 * GeoLocation.cpp
 *
 *  Created on: Oct 29, 2014
 *      Author: qiaohj
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
