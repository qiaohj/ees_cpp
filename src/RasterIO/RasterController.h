/*
 * RasterController.h
 *
 *  Created on: Oct 19, 2014
 *      Author: qiaohj
 */

#ifndef RASTERIO_RASTERCONTROLLER_H_
#define RASTERIO_RASTERCONTROLLER_H_

#include <string>
#include <gdal.h>
#include <gdal_priv.h>


using namespace std;
class RasterController {
private:
public:
    RasterController();
    ~RasterController();
    template<typename T> static void writeGeoTIFF(char* filename,
            unsigned xsize, unsigned ysize, double* geoTrans, T array,
            double nodata, GDALDataType datatype);

    template<typename T> static void writeRaster(char* driverName, char* filename,
                    unsigned xsize, unsigned ysize, double* geoTrans, T array,
                    double nodata, GDALDataType datatype);
};
template<typename T> void RasterController::writeGeoTIFF(char* filename,
            unsigned xsize, unsigned ysize, double* geoTrans, T array,
            double nodata, GDALDataType datatype){
    char dirver[] = "GTiff";
    writeRaster(dirver, filename, xsize, ysize, geoTrans, array, nodata, datatype);
}

template<typename T> void RasterController::writeRaster(char* driverName, char* filename,
        unsigned xsize, unsigned ysize, double* geoTrans, T array,
        double nodata, GDALDataType datatype) {
    GDALAllRegister();
    GDALDriver* driver = GetGDALDriverManager()->GetDriverByName(driverName);
    char **papszOptions = NULL;
    GDALDataset* dataset = driver->Create(filename, (int) xsize, (int) ysize, 1,
            datatype, papszOptions);
    dataset->SetGeoTransform(geoTrans);
    GDALRasterBand* band = dataset->GetRasterBand(1);

    band->SetNoDataValue(nodata);
    band->RasterIO(GF_Write, 0, 0, xsize, ysize, array, xsize, ysize, datatype,
            0, 0);
    dataset->FlushCache();
}


#endif /* RASTERIO_RASTERCONTROLLER_H_ */
