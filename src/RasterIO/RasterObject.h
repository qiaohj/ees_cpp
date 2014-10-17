/*
 * RasterObject.h
 *
 *  Created on: Oct 14, 2014
 *      Author: qiaohj
 */
using namespace std;

#include <string>
#include <gdal.h>
#include <gdal_priv.h>

class RasterObject{
private:
    const char* rasterName;
	GDALDataset* poDataset;
	GDALRasterBand* poBand;
	int xSize;
	int ySize;
	float noData;
	float* valueArray;
	double* adfGeoTransform;
public:
	RasterObject(char const* rasterName);
	~RasterObject();
	string getRasterName();
	GDALDataset* getGDALDataset();
	int getXSize();
	int getYSize();
	double* getGeoTransform();
	float readByXY(int x, int y);
	float getNoData();
	void XY2LL(int x, int y, double* longitude, double* latitude);
	void LL2XY(double longitude, double latitude, int* x, int* y);
	float readByLL(double longitude, double latitude);
	void printRasterInfo();

};


