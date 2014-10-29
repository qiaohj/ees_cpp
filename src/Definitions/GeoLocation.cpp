/*
 * GeoLocation.cpp
 *
 *  Created on: Oct 29, 2014
 *      Author: qiaohj
 */

#include "GeoLocation.h"

GeoLocation::GeoLocation(double p_longitude, double p_latitude) {
    longitude = p_longitude;
    latitude = p_latitude;

}

GeoLocation::~GeoLocation() {

}

double GeoLocation::getLongitude(){
    return longitude;
}
double GeoLocation::getLatitude(){
    return latitude;
}
