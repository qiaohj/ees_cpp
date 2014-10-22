
/*
 * gdaltest.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: qiaohj
 */
using namespace std;


#include <gdal_priv.h>
#include <stdio.h>
#include <stdlib.h>

#include "JsonPaster/include/json/json.h"
#include <algorithm> // sort
#include <string>


#include "ExpressionParser/parser.h"
#include "Definitions/Scenario.h"



void binary_mask(){
    RasterObject* mask = new RasterObject("/home/qiaohj/workspace/NicheBreadth/data/environment_layers/mask.tif");
    int* array = new int[mask->getXSize() * mask->getYSize()];
    for (unsigned x=0; x<mask->getXSize(); ++x){
        for (unsigned y=0;y<mask->getYSize();++y){
            float v = mask->readByXY(x, y);
            if (CommonFun::AlmostEqualRelative(v, mask->getNoData())){
                array[y * mask->getXSize() + x] = NODATA;
            }else{
                array[y * mask->getXSize() + x] = 1;
            }
        }
    }
    RasterController::writeGeoTIFF("/home/qiaohj/workspace/NicheBreadth/data/environment_layers/mask.tif",
            mask->getXSize(), mask->getYSize(), mask->getGeoTransform(), array, (double)NODATA, GDT_Int32);
    delete [] array;
    delete mask;
}
void cutEnvironmentLayers(char* layerName, char* newName){
    RasterObject* mask = new RasterObject("/home/qiaohj/workspace/NicheBreadth/data/environment_layers/mask.tif");
    RasterObject* env_layer = new RasterObject(layerName);
    float* array = new float[mask->getXSize() * mask->getYSize()];
    for (unsigned x=0; x<mask->getXSize(); ++x){
        for (unsigned y=0;y<mask->getYSize();++y){
            float v = mask->readByXY(x, y);
            if (CommonFun::AlmostEqualRelative(v, mask->getNoData())){
                array[y * mask->getXSize() + x] = NODATA;
            }else{
                double longitude, latitude;
                CommonFun::XY2LL(mask->getGeoTransform(), x, y, &longitude, &latitude);
                float env_v = env_layer->readByLL(longitude, latitude);
                array[y * mask->getXSize() + x] = env_v;
            }
        }
    }
    RasterController::writeGeoTIFF(newName,
                mask->getXSize(), mask->getYSize(), mask->getGeoTransform(), array, (double)NODATA, GDT_Float32);
}
int main(int argc, const char* argv[]) {
    //binary_mask();
//    cutEnvironmentLayers("/home/qiaohj/workspace/NicheBreadth/data/environment_layers/bio_var_CCSM_6k_global_bio1.origin.tif",
//            "/home/qiaohj/workspace/NicheBreadth/data/environment_layers/bio_var_CCSM_6k_global_bio1.tif");
//    cutEnvironmentLayers("/home/qiaohj/workspace/NicheBreadth/data/environment_layers/bio_var_CCSM_6k_global_bio12.origin.tif",
//                "/home/qiaohj/workspace/NicheBreadth/data/environment_layers/bio_var_CCSM_6k_global_bio12.tif");
//    cutEnvironmentLayers("/home/qiaohj/workspace/NicheBreadth/data/environment_layers/bio_var_CCSM_21k_global_bio1.origin.tif",
//                "/home/qiaohj/workspace/NicheBreadth/data/environment_layers/bio_var_CCSM_21k_global_bio1.tif");
//    cutEnvironmentLayers("/home/qiaohj/workspace/NicheBreadth/data/environment_layers/bio_var_CCSM_21k_global_bio12.origin.tif",
//                "/home/qiaohj/workspace/NicheBreadth/data/environment_layers/bio_var_CCSM_21k_global_bio12.tif");

	/*
	 * Scenario Json Test
	 * */
    char path[] = "/home/qiaohj/workspace/NicheBreadth/data/scenarios/scenario.json";
	Json::Value root_Scenario = CommonFun::readJson(path);
	Scenario* scenario = new Scenario(root_Scenario, "/home/qiaohj/workspace/NicheBreadth/data");
	scenario->run();
	delete scenario;
	printf("done!");
	return EXIT_SUCCESS;
}


