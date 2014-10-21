/*
 * SparseMap.h
 *
 *  Created on: Oct 16, 2014
 *      Author: qiaohj
 */

#ifndef SparseMap_H
#define SparseMap_H


#include <string>
#include "../RasterIO/RasterObject.h"
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace std;

typedef boost::numeric::ublas::compressed_matrix<int> mapvalue;

class SparseMap {
private:
    mapvalue* value;
    unsigned xSize;
    unsigned ySize;
public:
    SparseMap();
    SparseMap(unsigned x_size, unsigned y_size);
    SparseMap(mapvalue* p_value);
    SparseMap(RasterObject* raster, bool is_binary);
    SparseMap(SparseMap* potential, SparseMap* dispersal);
    virtual ~SparseMap();

    int readByXY(unsigned x, unsigned y);
    unsigned getXSize();
    unsigned getYSize();
    void setValue(unsigned x, unsigned y, int value);
};

#endif /* SparseMap_H */
