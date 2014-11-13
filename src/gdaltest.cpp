/*
 * gdaltest.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: qiaohj
 */

#include <gdal_priv.h>
#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

//#include "JsonPaster/include/json/json.h"
#include <algorithm> // sort
#include <string>
#include <math.h>

#include <unistd.h>
#include <boost/thread/thread.hpp>

#include "ExpressionParser/parser.h"
#include "Definitions/Scenario.h"
#include "Universal/log.hpp"
#include "Universal/CommonFun.h"
#include "Definitions/IndividualOrganism.h"
void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

//void binary_mask(){
//    RasterObject* mask = new RasterObject("/home/qiaohj/workspace/NicheBreadth/data/environment_layers/mask.tif");
//    int* array = new int[mask->getXSize() * mask->getYSize()];
//    for (unsigned x=0; x<mask->getXSize(); ++x){
//        for (unsigned y=0;y<mask->getYSize();++y){
//            float v = mask->readByXY(x, y);
//            if (CommonFun::AlmostEqualRelative(v, mask->getNoData())){
//                array[y * mask->getXSize() + x] = NODATA;
//            }else{
//                array[y * mask->getXSize() + x] = 1;
//            }
//        }
//    }
//    char a []= "/home/qiaohj/workspace/NicheBreadth/data/environment_layers/mask.tif";
//    RasterController::writeGeoTIFF(a,
//            mask->getXSize(), mask->getYSize(), mask->getGeoTransform(), array, (double)NODATA, GDT_Int32);
//    delete [] array;
//    delete mask;
//}
//void cutEnvironmentLayers(char* layerName, char* newName){
//    RasterObject* mask = new RasterObject("/home/qiaohj/workspace/NicheBreadth/data/environment_layers/mask.tif");
//    RasterObject* env_layer = new RasterObject(layerName);
//    float* array = new float[mask->getXSize() * mask->getYSize()];
//    for (unsigned x=0; x<mask->getXSize(); ++x){
//        for (unsigned y=0;y<mask->getYSize();++y){
//            float v = mask->readByXY(x, y);
//            if (CommonFun::AlmostEqualRelative(v, mask->getNoData())){
//                array[y * mask->getXSize() + x] = NODATA;
//            }else{
//                double longitude, latitude;
//                CommonFun::XY2LL(mask->getGeoTransform(), x, y, &longitude, &latitude);
//                float env_v = env_layer->readByLL(longitude, latitude);
//                array[y * mask->getXSize() + x] = env_v;
//            }
//        }
//    }
//    RasterController::writeGeoTIFF(newName,
//                mask->getXSize(), mask->getYSize(), mask->getGeoTransform(), array, (double)NODATA, GDT_Float32);
//    delete[] array;
//}
//void integerEnvironmentLayers(char* layerName, char* newName){
//    RasterObject* env_layer = new RasterObject(layerName);
//    int* array = new int [env_layer->getXSize() * env_layer->getYSize()];
//    for (unsigned x=0; x<env_layer->getXSize(); ++x){
//        for (unsigned y=0;y<env_layer->getYSize();++y){
//            float v = env_layer->readByXY(x, y);
//            if (CommonFun::AlmostEqualRelative(v, env_layer->getNoData())){
//                array[y * env_layer->getXSize() + x] = NODATA;
//            }else{
//                array[y * env_layer->getXSize() + x] = round(v * 1000);
//            }
//        }
//    }
//    RasterController::writeGeoTIFF(newName,
//            env_layer->getXSize(), env_layer->getYSize(), env_layer->getGeoTransform(), array, (double)NODATA, GDT_Int32);
//    delete[] array;
//}

_INITIALIZE_EASYLOGGINGPP

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

//    integerEnvironmentLayers("/home/qiaohj/workspace/NicheBreadth/data/environment_layers/bio_var_CCSM_6k_global_bio1.float.tif",
//            "/home/qiaohj/workspace/NicheBreadth/data/environment_layers/bio_var_CCSM_6k_global_bio1.tif");
//    integerEnvironmentLayers("/home/qiaohj/workspace/NicheBreadth/data/environment_layers/bio_var_CCSM_6k_global_bio12.float.tif",
//                "/home/qiaohj/workspace/NicheBreadth/data/environment_layers/bio_var_CCSM_6k_global_bio12.tif");
//    integerEnvironmentLayers("/home/qiaohj/workspace/NicheBreadth/data/environment_layers/bio_var_CCSM_21k_global_bio1.float.tif",
//                "/home/qiaohj/workspace/NicheBreadth/data/environment_layers/bio_var_CCSM_21k_global_bio1.tif");
//    integerEnvironmentLayers("/home/qiaohj/workspace/NicheBreadth/data/environment_layers/bio_var_CCSM_21k_global_bio12.float.tif",
//                "/home/qiaohj/workspace/NicheBreadth/data/environment_layers/bio_var_CCSM_21k_global_bio12.tif");

    /*
     * Scenario Json Test
     * */
//    printf("%lu\n", sizeof(IndividualOrganism));
//    printf("%lu\n", sizeof(IndividualOrganism*));
//    printf("%lu\n", sizeof(long unsigned));
//    printf("%lu\n", sizeof(unsigned));
    signal(SIGSEGV, handler);
    char path[] = "/home/qiaohj/workspace/NicheBreadth/data/scenarios/scenario.json";
	Json::Value root_Scenario = CommonFun::readJson(path);
	Scenario* scenario = new Scenario(root_Scenario, "/home/qiaohj/workspace/NicheBreadth/data", "/home/qiaohj/temp");
	scenario->run();
	LOG(INFO)<<"Before remove scenario, Memory usage:"<<CommonFun::getCurrentRSS();
	delete scenario;
	LOG(INFO)<<"After  remove scenario, Memory usage:"<<CommonFun::getCurrentRSS();

//    LOG(INFO)<<"Memory usage 1:"<<CommonFun::getCurrentRSS();
//    boost::this_thread::sleep( boost::posix_time::seconds(1) );
//    boost::unordered_map<unsigned, std::vector<IndividualOrganism*>* > all;
//    for (int j=0; j< 10000; j++){
//        std::vector<IndividualOrganism*>* x = new std::vector<IndividualOrganism*>();
//        for (int i=0; i< 10000; i++){
//            IndividualOrganism* o = new IndividualOrganism(0, NULL, NULL, 0, 0);
//            x->push_back(o);
//        }
//        all[j] = x;
//        LOG(INFO)<<"Memory usage 2:"<<CommonFun::getCurrentRSS();
//        CommonFun::clearVector(all[j]);
//        LOG(INFO)<<"Memory usage 3:"<<CommonFun::getCurrentRSS();
//        all.erase(j);
//        LOG(INFO)<<"Memory usage 4:"<<CommonFun::getCurrentRSS();
//    }
//    while (true) {
//        boost::this_thread::sleep( boost::posix_time::seconds(1) );
//        CommonFun::writeMemoryUsage(137, true, 0);
//        std::vector<CoodLocation*> next_cells;
//        for (unsigned i = 0; i < 6; i++) {
//            CoodLocation* c = new CoodLocation(1, 1);
//            next_cells.push_back(c);
//        }
//        boost::this_thread::sleep( boost::posix_time::seconds(1) );
//        CommonFun::writeMemoryUsage(144, true, 0);
//        for (std::vector<CoodLocation*>::iterator it = next_cells.begin();
//                it != next_cells.end(); ++it) {
//            delete *it;
//        }
//        next_cells.clear();
//        boost::this_thread::sleep( boost::posix_time::seconds(1) );
//        std::vector<CoodLocation*>().swap(next_cells);
//        CommonFun::writeMemoryUsage(151, true, 0);
//    }
    printf("done!");
    return EXIT_SUCCESS;
}

