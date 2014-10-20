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

using namespace std;

class SparseMap {
private:
    boost::numeric::ublas::compressed_matrix<int> value;
    unsigned xSize;
    unsigned ySize;
public:
    SparseMap();
    SparseMap(RasterObject* raster, bool is_binary);
    SparseMap(SparseMap* potential, SparseMap* dispersal);
    int readByXY(unsigned x, unsigned y);
    unsigned getXSize();
    unsigned getYSize();
    virtual ~SparseMap();
};

#endif /* SparseMap_H */
