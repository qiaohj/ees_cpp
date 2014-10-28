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
SparseMap::SparseMap(unsigned p_x_size, unsigned p_y_size) {
    xSize = p_x_size;
    ySize = p_y_size;
    value = new mapvalue(p_x_size, p_y_size, p_x_size * p_y_size);
}
SparseMap::SparseMap(RasterObject* p_raster, bool p_is_binary = false) {
    xSize = p_raster->getXSize();
    ySize = p_raster->getYSize();
    value = new mapvalue(xSize, ySize, xSize * ySize);
    for (unsigned x = 0; x < xSize; ++x) {
        for (unsigned y = 0; y < ySize; ++y) {
            float v = p_raster->readByXY(x, y);
            if (!CommonFun::AlmostEqualRelative(v, p_raster->getNoData())) {
                (*value)(x, y) = (p_is_binary) ? 1 : (int) v;
            }

        }
    }
}
void SparseMap::setValue(unsigned p_x, unsigned p_y, int p_value) {
    (*value)(p_x, p_y) = p_value;
}
int SparseMap::readByXY(unsigned p_x, unsigned p_y) {
    return (*value)(p_x, p_y);
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
//vector<CellObject*> SparseMap::getValues() {
//    vector<CellObject*> cell_list;
//    typedef boost::numeric::ublas::compressed_matrix<int>::iterator1 it1_t;
//    typedef boost::numeric::ublas::compressed_matrix<int>::iterator2 it2_t;
//    for (it1_t it1 = value->begin1(); it1 != value->end1(); it1++) {
//        for (it2_t it2 = it1.begin(); it2 != it1.end(); it2++) {
//            cell_list.push_back(
//                    new CellObject(it2.index1(), it2.index2(), *it2));
//        }
//    }
//    return cell_list;
//}
//int* SparseMap::toArray(){
//    int* array = new int[xSize * ySize];
//    for (unsigned i=0; i< (xSize * ySize); ++i){
//        array[i] = (int)NODATA;
//    }
//    vector<CellObject*> cell_list = getValues();
//    for (unsigned i=0; i<cell_list.size(); ++i){
//        array[cell_list[i]->getY() * xSize + cell_list[i]->getX()] = cell_list[i]->getValue();
//    }
//    CellObject::clearCellObject(cell_list);
//    return array;
//}
SparseMap::~SparseMap() {
    value->clear();
    delete value;
}

