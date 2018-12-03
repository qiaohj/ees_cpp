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
 *@brief A class to handle the raster file's values as a matrix
 */
class SparseMap {
private:
    mapvalue* value;
    unsigned xSize;
    unsigned ySize;
    std::string filename;
    RasterObject* raster;
public:
    /**
	 * @brief Constructor of SparseMap class No.1 (Create a blank map matrix with the given X and Y sizes)
	 * @param p_x_size X size of the map matrix
	 * @param max p_y_size limit of the niche breadth
	 */
    SparseMap(unsigned p_x_size, unsigned p_y_size);
    /**
     * @brief Constructor of SparseMap class No.2 (Create a map matrix based on a map matrix)
     * @param p_value a map matrix
     */
    SparseMap(mapvalue* p_value);

    /**
	 * @brief Constructor of SparseMap class No.3 (Create a map matrix based on the raster object)
	 * @param p_raster a raster object
	 * @param p_is_binary If False, using the raw values to create the object, or binary the values of the raster file (Zero and Non-Zero).
	 */
    SparseMap(RasterObject* p_raster, bool p_is_binary);

    /**
	 * @brief Destructor of SparseMap class
	 *
	 * release all the resources
	 */
    virtual ~SparseMap();

    /**
     * @brief return the raster object used to create the object
     * @return
     */
    RasterObject* getRaster();

    /**
     * @brief read the map value based on the X and Y index of the matrix
     * @param p_x X index of the matrix
     * @param p_y Y index of the matrix
     * @return return the map value on the X and Y index of the matrix
     */
    int readByXY(unsigned p_x, unsigned p_y);

    /**
     * @brief read the map value based on the longitude and latitude and a given geotransform matrix.
     * @param p_geoTrans The GeoTransform matrix of the raster layers
     * @param p_longitude Longitude
     * @param p_latitude Latitude
     * @return
     */
    int readByLL(double* p_geoTrans, double p_longitude, double p_latitude);
    /**
	 * @brief read the map value based on the longitude and latitude
	 * @param p_longitude Longitude
	 * @param p_latitude Latitude
	 * @return
	 */
    int readByLL(double p_longitude, double p_latitude);

    /**
     * @brief return the X size of the map matrix
     */
    unsigned getXSize();

    /**
	 * @brief return the Y size of the map matrix
	 */
    unsigned getYSize();

    /**
     * @brief set a value to the map matrix based on the X, Y index
     * @param p_x X index of the map matrix
     * @param p_y Y index of the map matrix
     * @param p_value The value to set to the X, Y idnex.
     */
    void setValue(unsigned p_x, unsigned p_y, int p_value);
    /**
     * @brief get the First NON-NODATA value in the map matrix.
     * @param x X index of the map matrix
     * @param y Y index of the map matrix
     * @param v returned value.
     */
    void getFirstValues(unsigned* x, unsigned* y, int* v);

    /**
     * @brief convert the map matrix to an array.
     * @return
     */
    int* toArray();

    /**
     * @brief return the file name of the raster object.
     * @return
     */
    std::string getFilename();
};

#endif /* SparseMap_H */
