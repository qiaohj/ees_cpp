/**
 * @file GeoLocation.h
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

#ifndef DEFINITIONS_GEOLOCATION_H_
#define DEFINITIONS_GEOLOCATION_H_
/**
 * @brief A class to save the geographic location of each pixel
 */
class GeoLocation {
private:
	/**
	 * @brief Longitude
	 */
	float longitude;
	/**
	 * @brief Latitude
	 */
	float latitude;
public:
	/**
	 * @brief Constructor of GeoLocation class
	 * @param p_longitude Longitude
	 * @param p_latitude Latitude
	 */
    GeoLocation(float p_longitude, float p_latitude);
    /**
	 * @brief Destructor of GeoLocation class
	 *
	 * release all the resources
	 */
    virtual ~GeoLocation();

    /**
     * @brief Return the longitude of the object
     * @return
     */
    float getLongitude();
    /**
	 * @brief Return the latitude of the object
	 * @return
	 */
    float getLatitude();
};

#endif /* DEFINITIONS_GEOLOCATION_H_ */
