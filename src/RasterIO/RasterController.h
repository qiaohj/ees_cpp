/**
 * @file RasterController.h
 * @brief Class RasterController. A class to handle the raster files
 * @author Huijie Qiao
 * @version 1.0
 * @date 11/25/2018
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#ifndef RASTERIO_RASTERCONTROLLER_H_
#define RASTERIO_RASTERCONTROLLER_H_

#include <string>
#include <gdal.h>
#include <gdal_priv.h>

/**
 * @brief A class to handle the raster files
 */
class RasterController {
private:
public:
	/**
	 * @brief Constructor of RasterController
	 */
    RasterController();
	/**
	 * @brief Destructor of RasterController
	 * release all the resources
	 */
    ~RasterController();
    /**
     * @brief write an array to a raster file in GTiff format
     * @param p_filename the file name to save
     * @param p_xsize X size of the map
     * @param p_ysize Y size of the map
     * @param p_geo_trans The GeoTransform matrix of the raster layers
     * @param p_array Data array
     * @param p_nodata NODATa value
     * @param p_datatype Data type
     * @param p_prj projection string in proj4 format.
     */
    template<typename T> static void writeGeoTIFF(char* p_filename,
            unsigned p_xsize, unsigned p_ysize, double* p_geo_trans, T p_array,
            double p_nodata, GDALDataType p_datatype, const char* p_prj);
    /**
	 * @brief write an array to a raster file in GTiff format
	 * @param p_driver_name Driver name to save the raster file
	 * @param p_filename the file name to save
	 * @param p_xsize X size of the map
	 * @param p_ysize Y size of the map
	 * @param p_geo_trans The GeoTransform matrix of the raster layers
	 * @param p_array Data array
	 * @param p_nodata NODATa value
	 * @param p_datatype Data type
	 * @param p_prj projection string in proj4 format.
	 */
    template<typename T> static void writeRaster(char* p_driver_name, char* p_filename,
                    unsigned xsize, unsigned ysize, double* p_geo_trans, T p_array,
                    double p_nodata, GDALDataType p_datatype, const char* p_prj);
};
template<typename T> void RasterController::writeGeoTIFF(char* p_filename,
            unsigned p_xsize, unsigned p_ysize, double* p_geo_trans, T p_array,
            double p_nodata, GDALDataType p_datatype, const char* p_prj){
    char dirver[] = "GTiff";
    writeRaster(dirver, p_filename, p_xsize, p_ysize, p_geo_trans, p_array, p_nodata, p_datatype, p_prj);
}

template<typename T> void RasterController::writeRaster(char* p_driver_name, char* p_filename,
        unsigned p_xsize, unsigned p_ysize, double* p_geo_trans, T p_array,
        double p_nodata, GDALDataType p_datatype, const char* p_prj) {
    GDALAllRegister();

    GDALDriver* driver = GetGDALDriverManager()->GetDriverByName(p_driver_name);
    char **papszOptions = NULL;
    GDALDataset* dataset = driver->Create(p_filename, (int) p_xsize, (int) p_ysize, 1,
            p_datatype, papszOptions);
    dataset->SetGeoTransform(p_geo_trans);

    dataset->SetProjection(p_prj);
    GDALRasterBand* band = dataset->GetRasterBand(1);

    band->SetNoDataValue(p_nodata);
    band->RasterIO(GF_Write, 0, 0, p_xsize, p_ysize, p_array, p_xsize, p_ysize, p_datatype,
            0, 0);
    dataset->FlushCache();
    GDALClose( (GDALDatasetH) dataset );;
}


#endif /* RASTERIO_RASTERCONTROLLER_H_ */
