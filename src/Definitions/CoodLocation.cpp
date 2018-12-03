/**
 * @file CoodLocation.cpp
 * @brief Class CoodLocation, which is a pixel on the map, and the minimal unit of the simulation.
 * @author Huijie Qiao
 * @version 1.0
 * @date 11/25/2018
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
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
