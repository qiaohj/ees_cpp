//============================================================================
// Name        : RasterObject.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================
#include "RasterObject.h"


RasterObject::RasterObject(char const* rasterName) :
		rasterName(rasterName){
	GDALAllRegister();
//	printf("%s\n", rasterName);
	poDataset = (GDALDataset *) GDALOpen(rasterName, GA_ReadOnly);
	poBand = poDataset->GetRasterBand(1);
	xSize = poBand->GetXSize();
	ySize = poBand->GetYSize();
	noData = poBand->GetNoDataValue();
	valueArray = new float[xSize * ySize];
	poBand->RasterIO(GF_Read, 0, 0, xSize, ySize, valueArray, xSize, ySize, GDT_Float32, 0, 0);
	adfGeoTransform = new double[6];
	poDataset->GetGeoTransform(adfGeoTransform);
//	printRasterInfo();
}
RasterObject::~RasterObject(){
    delete[] adfGeoTransform;
    delete[] valueArray;
}
string RasterObject::getRasterName(){
	return rasterName;
}
GDALDataset* RasterObject::getGDALDataset(){
	return poDataset;
}
unsigned RasterObject::getXSize(){
	return xSize;
}
unsigned RasterObject::getYSize(){
	return ySize;
}
double* RasterObject::getGeoTransform(){
	return adfGeoTransform;
}
float RasterObject::getNoData(){
	return noData;
}
float RasterObject::readByXY(unsigned p_x, unsigned p_y){
	if (p_x<0||p_x>=xSize||p_y<0||p_y>=ySize){
		return noData;
	}
	return *(valueArray + (p_y * xSize + p_x));
}
float RasterObject::readByLL(double p_longitude, double p_latitude){
    unsigned x, y;
	CommonFun::LL2XY(adfGeoTransform, p_longitude, p_latitude, &x, &y);
	return readByXY(x, y);
}
void RasterObject::printRasterInfo(){
	printf("Driver: %s/%s\n",
			poDataset->GetDriver()->GetDescription(),
			poDataset->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ));
	printf("Size is %dx%dx%d\n",
			poDataset->GetRasterXSize(), poDataset->GetRasterYSize(),
			poDataset->GetRasterCount() );
	if (poDataset->GetProjectionRef()  != NULL)
		printf( "Projection is `%s'\n", poDataset->GetProjectionRef() );
	printf( "Origin = (%.6f,%.6f)\n",
			adfGeoTransform[0], adfGeoTransform[3] );
	printf( "Pixel Size = (%.6f,%.6f)\n",
			adfGeoTransform[1], adfGeoTransform[5] );
}
