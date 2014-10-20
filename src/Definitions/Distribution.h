/*
 * Distribution.h
 *
 *  Created on: Oct 16, 2014
 *      Author: qiaohj
 */

using namespace std;

#include <string>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
#include "../RasterIO/RasterObject.h"

class Distribution {
private:
    boost::numeric::ublas::compressed_matrix<int> value;
    unsigned xSize;
    unsigned ySize;
public:
    Distribution(RasterObject* raster);
    Distribution(Distribution* potential, Distribution* dispersal);
    int readByXY(unsigned x, unsigned y);
    unsigned getXSize();
    unsigned getYSize();
    virtual ~Distribution();
};

