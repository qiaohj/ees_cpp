/*
 * RasterObject.h
 *
 *  Created on: Oct 14, 2014
 *      Author: qiaohj
 */

#ifndef RasterObject_H
#define RasterObject_H

#include <string>
#include <gdal.h>
#include <gdal_priv.h>
#include "../Universal/CommonFun.h"

class RasterObject{
private:
    const char* rasterName;
	GDALDataset* poDataset;
	GDALRasterBand* poBand;
	unsigned xSize;
	unsigned ySize;
	float noData;
	float* valueArray;
	double* adfGeoTransform;
public:
	RasterObject(char const* rasterName);
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

};

#endif /* RasterObject_H */
