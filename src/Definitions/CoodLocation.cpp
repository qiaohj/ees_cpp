/*
 * CoodLocation.cpp
 *
 *  Created on: Oct 29, 2014
 *      Author: qiaohj
 */

#include "CoodLocation.h"

CoodLocation::CoodLocation(short unsigned p_x, short unsigned p_y) {
   x = p_x;
   y = p_y;

}

CoodLocation::~CoodLocation() {

}

unsigned short CoodLocation::getX(){
    return x;
}
unsigned short CoodLocation::getY(){
    return y;
}
