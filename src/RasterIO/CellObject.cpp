/*
 * CellObject.cpp
 *
 *  Created on: Oct 19, 2014
 *      Author: qiaohj
 */

#include "CellObject.h"

CellObject::CellObject(unsigned p_x, unsigned p_y, int p_value){
    x = p_x;
    y = p_y;
    value = p_value;
}
CellObject::~CellObject(){
}
unsigned CellObject::getX(){
    return x;
}
unsigned CellObject::getY(){
    return y;
}
int CellObject::getValue(){
    return value;
}



