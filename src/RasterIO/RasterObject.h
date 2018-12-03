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
	RasterObject(std::string p_raster_name);
	~RasterObject();
	std::string getRasterName();
	GDALDataset* getGDALDataset();
	unsigned getXSize();
	unsigned getYSize();
	double* getGeoTransform();
	float readByXY(unsigned p_x, unsigned p_y);
	float getNoData();
	float readByLL(double p_longitude, double p_latitude);
	void printRasterInfo();
	const char* getGCPProjection();
	const char* getProjectionRef();
};

#endif /* RasterObject_H */
