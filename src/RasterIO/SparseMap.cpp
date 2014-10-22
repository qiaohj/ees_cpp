/*
 * SparseMap.cpp
 *
 *  Created on: Oct 16, 2014
 *      Author: qiaohj
 */

#include "SparseMap.h"

SparseMap::SparseMap(mapvalue* p_value) {
    value = p_value;
    xSize = value->size1();
    ySize = value->size2();
}
SparseMap::SparseMap(unsigned x_size, unsigned y_size) {
    xSize = x_size;
    ySize = y_size;
    value = new mapvalue(x_size, y_size, x_size * y_size);
}
SparseMap::SparseMap(RasterObject* raster, bool is_binary = false) {
    xSize = raster->getXSize();
    ySize = raster->getYSize();
    value = new mapvalue(xSize, ySize, xSize * ySize);
    for (unsigned x = 0; x < xSize; ++x) {
        for (unsigned y = 0; y < ySize; ++y) {
            float v = raster->readByXY(x, y);
            if (!CommonFun::AlmostEqualRelative(v, raster->getNoData())) {
                (*value)(x, y) = (is_binary) ? 1 : (int) v;
            }

        }
    }
}
void SparseMap::setValue(unsigned x, unsigned y, int p_value) {
    (*value)(x, y) = p_value;
}
int SparseMap::readByXY(unsigned x, unsigned y) {
    return (*value)(x, y);
}
unsigned SparseMap::getXSize() {
    return xSize;
}
unsigned SparseMap::getYSize() {
    return ySize;
}
vector<CellObject*> SparseMap::getValues() {
    vector<CellObject*> cell_list;
    typedef boost::numeric::ublas::compressed_matrix<int>::iterator1 it1_t;
    typedef boost::numeric::ublas::compressed_matrix<int>::iterator2 it2_t;
    for (it1_t it1 = value->begin1(); it1 != value->end1(); it1++) {
        for (it2_t it2 = it1.begin(); it2 != it1.end(); it2++) {
            cell_list.push_back(
                    new CellObject(it2.index1(), it2.index2(), *it2));
        }
    }
    return cell_list;
}
int* SparseMap::toArray(){
    int* array = new int[xSize * ySize];
    for (unsigned i=0; i< (xSize * ySize); ++i){
        array[i] = (int)NODATA;
    }
    vector<CellObject*> cell_list = getValues();
    for (unsigned i=0; i<cell_list.size(); ++i){
        array[cell_list[i]->getY() * xSize + cell_list[i]->getX()] = cell_list[i]->getValue();
    }
    return array;
}
SparseMap::~SparseMap() {
}

