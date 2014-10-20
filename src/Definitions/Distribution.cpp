/*
 * Distribution.cpp
 *
 *  Created on: Oct 16, 2014
 *      Author: qiaohj
 */

#include "Distribution.h"


Distribution::Distribution(RasterObject* raster, bool is_binary) {
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
Distribution::Distribution(Distribution* potential, Distribution* dispersal){
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
int Distribution::readByXY(unsigned x, unsigned y){
    return value(x, y);
}
unsigned Distribution::getXSize(){
    return xSize;
}
unsigned Distribution::getYSize(){
    return ySize;
}
Distribution::~Distribution() {
}

