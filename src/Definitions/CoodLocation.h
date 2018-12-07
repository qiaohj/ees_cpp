/**
 * @file CoodLocation.h
 * @brief Class CoodLocation A pixel on the map, and the minimal unit of the simulation
 * @author Huijie Qiao
 * @version 1.0
 * @date 11/25/2018
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#ifndef DEFINITIONS_COODLOCATION_H_
#define DEFINITIONS_COODLOCATION_H_
/**
 * @brief A pixel on the map, and the minimal unit of the simulation
 */
class CoodLocation {
private:
    unsigned short x, y;
public:
    /**
     * @brief Constructor of CoodLocation class
     * @param p_x
     * @param p_y
     */
    CoodLocation(unsigned short p_x, unsigned short p_y);

    /**
	 * @brief Destructor of CoodLocation class
	 *
	 * release all the resources
	 */
    virtual ~CoodLocation();
    /**
     * return the X of the location
     */
    unsigned short getX();
    /**
	* return the Y of the location
	*/
    unsigned short getY();
};

#endif /* DEFINITIONS_COODLOCATION_H_ */
