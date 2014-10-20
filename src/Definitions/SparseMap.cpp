/*
 * SparseMap.cpp
 *
 *  Created on: Oct 16, 2014
 *      Author: qiaohj
 */

#include "SparseMap.h"

SparseMap::SparseMap(){

}
SparseMap::SparseMap(RasterObject* raster, bool is_binary = false) {
    xSize = raster->getXSize();
    ySize = raster->getYSize();
    boost::numeric::ublas::compressed_matrix<int> t (xSize, ySize, xSize * ySize);
    for (unsigned x = 0; x < xSize; ++ x){
        for (unsigned y = 0; y < ySize; ++ y){
            t(x, y) = (int)raster->readByXY(x, y);
        }
    }
    value = t;
}
SparseMap::SparseMap(SparseMap* potential, SparseMap* dispersal){
    xSize = potential->getXSize();
    ySize = potential->getYSize();
    boost::numeric::ublas::compressed_matrix<int> t (xSize, ySize, xSize * ySize);
    for (unsigned x = 0; x < xSize; ++ x){
        for (unsigned y = 0; y < ySize; ++ y){
            t(x, y) = potential->readByXY(x, y)&&dispersal->readByXY(x, y);
        }
    }
    value = t;
}
int SparseMap::readByXY(unsigned x, unsigned y){
    return value(x, y);
}
unsigned SparseMap::getXSize(){
    return xSize;
}
unsigned SparseMap::getYSize(){
    return ySize;
}
SparseMap::~SparseMap() {
}

