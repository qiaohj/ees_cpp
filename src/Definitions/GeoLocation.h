/*
 * GeoLocation.h
 *
 *  Created on: Oct 29, 2014
 *      Author: qiaohj
 */

#ifndef DEFINITIONS_GEOLOCATION_H_
#define DEFINITIONS_GEOLOCATION_H_

class GeoLocation {
private:
    double longitude;
    double latitude;
public:
    GeoLocation(double p_longitude, double p_latitude);
    virtual ~GeoLocation();
    double getLongitude();
    double getLatitude();
};

#endif /* DEFINITIONS_GEOLOCATION_H_ */
