/**
 * @file SparseMap.h
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

#ifndef SparseMap_H
#define SparseMap_H


#include <string>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
#include "../RasterIO/RasterObject.h"
#include "../Universal/const.h"
#include "../Universal/CommonFun.h"

typedef boost::numeric::ublas::compressed_matrix<int> mapvalue;
/**
 *@brief A class to handle the raster file
 */
class SparseMap {
private:
    mapvalue* value;
    unsigned xSize;
    unsigned ySize;
    std::string filename;
    RasterObject* raster;
public:
    SparseMap(unsigned p_x_size, unsigned p_y_size);
    SparseMap(mapvalue* p_value);
    SparseMap(RasterObject* p_raster, bool p_is_binary);
    virtual ~SparseMap();
    RasterObject* getRaster();
    int readByXY(unsigned p_x, unsigned p_y);
    int readByLL(double* p_geoTrans, double p_longitude, double p_latitude);
    int readByLL(double p_longitude, double p_latitude);
    unsigned getXSize();
    unsigned getYSize();
    void setValue(unsigned p_x, unsigned p_y, int p_value);
    void getFirstValues(unsigned* x, unsigned* y, int* v);
    int* toArray();
    std::string getFilename();
};

#endif /* SparseMap_H */
