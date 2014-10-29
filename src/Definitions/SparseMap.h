/*
 * SparseMap.h
 *
 *  Created on: Oct 16, 2014
 *      Author: qiaohj
 */

#ifndef SparseMap_H
#define SparseMap_H


#include <string>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
#include "../RasterIO/RasterObject.h"
#include "../Universal/const.h"

typedef boost::numeric::ublas::compressed_matrix<int> mapvalue;

class SparseMap {
private:
    mapvalue* value;
    unsigned xSize;
    unsigned ySize;
public:
    SparseMap(unsigned p_x_size, unsigned p_y_size);
    SparseMap(mapvalue* p_value);
    SparseMap(RasterObject* p_raster, bool p_is_binary);
    virtual ~SparseMap();

    int readByXY(unsigned p_x, unsigned p_y);
    unsigned getXSize();
    unsigned getYSize();
    void setValue(unsigned p_x, unsigned p_y, int p_value);
    void getFirstValues(unsigned* x, unsigned* y, int* v);
    int* toArray();
};

#endif /* SparseMap_H */
