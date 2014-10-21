/*
 * SparseMap.cpp
 *
 *  Created on: Oct 16, 2014
 *      Author: qiaohj
 */

#include "SparseMap.h"

SparseMap::SparseMap(){

}
SparseMap::SparseMap(mapvalue* p_value){
    value = p_value;
    xSize = value->size1();
    ySize = value->size2();
}
SparseMap::SparseMap(unsigned x_size, unsigned y_size){
    xSize = x_size;
    ySize = y_size;
    value = new mapvalue(x_size, y_size, x_size * y_size);
}
SparseMap::SparseMap(RasterObject* raster, bool is_binary = false) {
    xSize = raster->getXSize();
    ySize = raster->getYSize();
    value = new mapvalue(xSize, ySize, xSize * ySize);
    for (unsigned x = 0; x < xSize; ++ x){
        for (unsigned y = 0; y < ySize; ++ y){
            (*value)(x, y) = (int)raster->readByXY(x, y);
        }
    }
}
SparseMap::SparseMap(SparseMap* potential, SparseMap* dispersal){
    xSize = potential->getXSize();
    ySize = potential->getYSize();
    value = new mapvalue(xSize, ySize, xSize * ySize);
    for (unsigned x = 0; x < xSize; ++ x){
        for (unsigned y = 0; y < ySize; ++ y){
            (*value)(x, y) = potential->readByXY(x, y)&&dispersal->readByXY(x, y);
        }
    }
}
void SparseMap::setValue(unsigned x, unsigned y, int p_value){
    (*value)(x, y) = p_value;
}
int SparseMap::readByXY(unsigned x, unsigned y){
    return (*value)(x, y);
}
unsigned SparseMap::getXSize(){
    return xSize;
}
unsigned SparseMap::getYSize(){
    return ySize;
}

SparseMap::~SparseMap() {
}

