/*
 * CoodLocation.cpp
 *
 *  Created on: Oct 29, 2014
 *      Author: qiaohj
 */

#include "CoodLocation.h"

CoodLocation::CoodLocation(unsigned p_x, unsigned p_y) {
   x = p_x;
   y = p_y;

}

CoodLocation::~CoodLocation() {

}

unsigned CoodLocation::getX(){
    return x;
}
unsigned CoodLocation::getY(){
    return y;
}
