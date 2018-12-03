/**
 * @file RasterObject.h
 * @brief Class RasterObject. A class for a raster file, including its attributes and behaviors.
 * @author Huijie Qiao
 * @version 1.0
 * @date 11/25/2018
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#ifndef RasterObject_H
#define RasterObject_H

#include <string>
#include <gdal.h>
#include <gdal_priv.h>
#include "../Universal/CommonFun.h"

/**
 * @brief A class for a raster file, including its attributes and behaviors.
 */
class RasterObject{
private:
    std::string rasterName;
	GDALDataset* poDataset;
	GDALRasterBand* poBand;
	unsigned xSize;
	unsigned ySize;
	float noData;
	float* valueArray;
	double* adfGeoTransform;
public:
	/**
	 * @brief Constructor of RasterObject
	 * @param p_raster_name the file name of the raster file.
	 */
	RasterObject(std::string p_raster_name);
	/**
	 * @brief Destructor of RasterObject
	 * release all the resources
	 */
	~RasterObject();
	/**
	 * @brief return the file name of the raster file
	 * @return
	 */
	std::string getRasterName();
	/**
	 * @brief return the dataset of the raster file
	 * @return
	 */
	GDALDataset* getGDALDataset();
	/**
	 * @brief return the X size of the map matrix
	 */
	unsigned getXSize();
	/**
	 * @brief return the Y size of the map matrix
	 */
	unsigned getYSize();
	/**
	 * @brief return the GeoTransform matrix of the raster layers
	 * @return
	 */
	double* getGeoTransform();
	/**
	 * @brief read a value from the raster file via X and Y index in the map matrix
	 * @param p_x X index
	 * @param p_y Y index
	 * @return
	 */
	float readByXY(unsigned p_x, unsigned p_y);
	/**
	 * @brief return NODATA value of the raster file
	 * @return
	 */
	float getNoData();
	/**
	 * @brief read a value from the raster file via longitude and latitude in the map matrix
	 * @param p_longitude Longitude
	 * @param p_latitude Latitude
	 * @return
	 */
	float readByLL(double p_longitude, double p_latitude);
	/**
	 * @brief output the information of the raster file (for debug)
	 */
	void printRasterInfo();
	/**
	 * @brief return the projection string in proj4 format
	 * @return
	 */
	const char* getGCPProjection();
	/**
	 * @brief return the ProjectionRef string in proj4 format
	 * @return
	 */
	const char* getProjectionRef();
};

#endif /* RasterObject_H */
