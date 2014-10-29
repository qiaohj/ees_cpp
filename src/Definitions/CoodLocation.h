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
    unsigned x, y;
public:
    CoodLocation(unsigned p_x, unsigned p_y);
    virtual ~CoodLocation();
    unsigned getX();
    unsigned getY();
};

#endif /* DEFINITIONS_COODLOCATION_H_ */
