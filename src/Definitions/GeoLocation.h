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
	float longitude;
	float latitude;
public:
    GeoLocation(float p_longitude, float p_latitude);
    virtual ~GeoLocation();
    float getLongitude();
    float getLatitude();
};

#endif /* DEFINITIONS_GEOLOCATION_H_ */
