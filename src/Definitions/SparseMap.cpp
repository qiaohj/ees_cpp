/**
 * @file SparseMap.cpp
 * @brief Class SparseMap. A class to handle the raster file
 * @author Huijie Qiao
 * @version 1.0
 * @date 11/25/2018
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#include "SparseMap.h"

SparseMap::SparseMap(mapvalue* p_value) {
    value = p_value;
    xSize = value->size1();
    ySize = value->size2();
}
SparseMap::SparseMap(unsigned p_x_size, unsigned p_y_size) {
    xSize = p_x_size;
    ySize = p_y_size;
    value = new mapvalue(p_x_size, p_y_size, p_x_size * p_y_size);
}
SparseMap::SparseMap(RasterObject* p_raster, bool p_is_binary = false) {
	raster = p_raster;
    xSize = p_raster->getXSize();
    ySize = p_raster->getYSize();
    filename = p_raster->getRasterName();
    value = new mapvalue(xSize, ySize, xSize * ySize);
    for (unsigned x = 0; x < xSize; ++x) {
        for (unsigned y = 0; y < ySize; ++y) {
            float v = p_raster->readByXY(x, y);
            if (!CommonFun::AlmostEqualRelative(v, p_raster->getNoData())) {
            	if (CommonFun::AlmostEqualRelative(v, 0.0f)){
            		v = MATRIX_ZERO;
            	}
                (*value)(x, y) = (p_is_binary) ? 1 : (int) v;
            }else{
            	(*value)(x, y) = NODATA;
            }

        }
    }
}
std::string SparseMap::getFilename(){
	return filename;
}
void SparseMap::setValue(unsigned p_x, unsigned p_y, int p_value) {
    (*value)(p_x, p_y) = p_value;
}
int SparseMap::readByXY(unsigned p_x, unsigned p_y) {
	int v = (*value)(p_x, p_y);
	if (v == MATRIX_ZERO){
		v = 0;
	}
    return v;
}
int SparseMap::readByLL(double* p_geoTrans, double p_longitude, double p_latitude){
    unsigned x, y;
	CommonFun::LL2XY(p_geoTrans, p_longitude, p_latitude, &x, &y);
	return readByXY(x, y);
}
int SparseMap::readByLL(double p_longitude, double p_latitude){
    unsigned x, y;
	CommonFun::LL2XY(raster->getGeoTransform(), p_longitude, p_latitude, &x, &y);
	return readByXY(x, y);
}
RasterObject* SparseMap::getRaster(){
	return raster;
}
unsigned SparseMap::getXSize() {
    return xSize;
}
unsigned SparseMap::getYSize() {
    return ySize;
}
void SparseMap::getFirstValues(unsigned* x, unsigned* y, int* v){
    typedef boost::numeric::ublas::compressed_matrix<int>::iterator1 it1_t;
    typedef boost::numeric::ublas::compressed_matrix<int>::iterator2 it2_t;
    for (it1_t it1 = value->begin1(); it1 != value->end1(); it1++) {
        for (it2_t it2 = it1.begin(); it2 != it1.end(); it2++) {
            *x = it2.index1();
            *y = it2.index2();
            *v = *it2;
        }
    }
}
int* SparseMap::toArray(){
    int* array = new int[xSize * ySize];
    for (unsigned i=0; i< (xSize * ySize); ++i){
        array[i] = (int)NODATA;
    }
    typedef boost::numeric::ublas::compressed_matrix<int>::iterator1 it1_t;
    typedef boost::numeric::ublas::compressed_matrix<int>::iterator2 it2_t;
    for (it1_t it1 = value->begin1(); it1 != value->end1(); it1++) {
        for (it2_t it2 = it1.begin(); it2 != it1.end(); it2++) {
            array[it2.index2() * xSize + it2.index1()] = *it2;
        }
    }
    return array;
}
SparseMap::~SparseMap() {
    value->clear();
    delete value;
}

