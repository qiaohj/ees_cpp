/*
 * CoodLocation.h
 *
 *  Created on: Oct 29, 2014
 *      Author: qiaohj
 */

#ifndef DEFINITIONS_COODLOCATION_H_
#define DEFINITIONS_COODLOCATION_H_

class CoodLocation {
private:
    unsigned short x, y;
public:
    CoodLocation(unsigned short p_x, unsigned short p_y);
    virtual ~CoodLocation();
    unsigned short getX();
    unsigned short getY();
};

#endif /* DEFINITIONS_COODLOCATION_H_ */
