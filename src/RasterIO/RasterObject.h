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

using namespace std;
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
	string getRasterName();
	GDALDataset* getGDALDataset();
	unsigned getXSize();
	unsigned getYSize();
	double* getGeoTransform();
	float readByXY(unsigned x, unsigned y);
	float getNoData();
	void XY2LL(unsigned x, unsigned y, double* longitude, double* latitude);
	void LL2XY(double longitude, double latitude, unsigned* x, unsigned* y);
	float readByLL(double longitude, double latitude);
	void printRasterInfo();

};

#endif /* RasterObject_H */
