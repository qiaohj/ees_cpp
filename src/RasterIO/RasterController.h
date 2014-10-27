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
    template<typename T> static void writeGeoTIFF(char* p_filename,
            unsigned p_xsize, unsigned p_ysize, double* p_geo_trans, T p_array,
            double p_nodata, GDALDataType p_datatype);

    template<typename T> static void writeRaster(char* p_driver_name, char* p_filename,
                    unsigned xsize, unsigned ysize, double* p_geo_trans, T p_array,
                    double p_nodata, GDALDataType p_datatype);
};
template<typename T> void RasterController::writeGeoTIFF(char* p_filename,
            unsigned p_xsize, unsigned p_ysize, double* p_geo_trans, T p_array,
            double p_nodata, GDALDataType p_datatype){
    char dirver[] = "GTiff";
    writeRaster(dirver, p_filename, p_xsize, p_ysize, p_geo_trans, p_array, p_nodata, p_datatype);
}

template<typename T> void RasterController::writeRaster(char* p_driver_name, char* p_filename,
        unsigned p_xsize, unsigned p_ysize, double* p_geo_trans, T p_array,
        double p_nodata, GDALDataType p_datatype) {
    GDALAllRegister();
    GDALDriver* driver = GetGDALDriverManager()->GetDriverByName(p_driver_name);
    char **papszOptions = NULL;
    GDALDataset* dataset = driver->Create(p_filename, (int) p_xsize, (int) p_ysize, 1,
            p_datatype, papszOptions);
    dataset->SetGeoTransform(p_geo_trans);
    GDALRasterBand* band = dataset->GetRasterBand(1);

    band->SetNoDataValue(p_nodata);
    band->RasterIO(GF_Write, 0, 0, p_xsize, p_ysize, p_array, p_xsize, p_ysize, p_datatype,
            0, 0);
    dataset->FlushCache();
}


#endif /* RASTERIO_RASTERCONTROLLER_H_ */
