/*
 * gdaltest.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: qiaohj
 */

#include <gdal.h>
#include <gdal_priv.h>
#include <ogr_srs_api.h>
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

_INITIALIZE_EASYLOGGINGPP

//configure_base_folder, scenario_json, specied_id, result_root, memory_limit(in M), tif_limit
///home/huijieqiao/NB_NEW/Environments scenario /home/huijieqiao/temp 4000 1000 1
int main(int argc, const char* argv[]) {

	int resolution = 100;
	/*LOG(INFO)<<CommonFun::vincenty_distance(0,0, 90, 0)<<" "<<CommonFun::haversine_distance(0,0, 90, 0);
	LOG(INFO)<<CommonFun::vincenty_distance(0,0, 180, 0)<<" "<<CommonFun::haversine_distance(0,0, 180, 0);
	LOG(INFO)<<CommonFun::vincenty_distance(0,0, -180, 0)<<" "<<CommonFun::haversine_distance(0,0, -180, 0);
	LOG(INFO)<<CommonFun::vincenty_distance(180,0, -180, 0)<<" "<<CommonFun::haversine_distance(180,0, -180, 0);
	exit(1);
	*/

	RasterObject* fromRaster = new RasterObject(std::string("/home/huijieqiao/NB_2016/PCA_Reproject/Prec/0000.tif"));
	RasterObject* toRaster = new RasterObject(std::string("/home/huijieqiao/NB_2016/PCA_Data/Prec_PC1/TIF/0000.tif"));
	const char* fromWkt = fromRaster->getProjectionRef();
	const char* toWkt = toRaster->getProjectionRef();


	/*double lon, lat;
	for (int y=89; y<90;y++){
		for (int x=0; x<fromRaster->getXSize();x++){
			int v = fromRaster->readByXY(x, y);
			if (v!=NODATA){
				CommonFun::XY2LL(fromRaster->getGeoTransform(), x, y, &lon, &lat);
			}
		}
	}
*/
	//CommonFun::convert2LL(-9684.702923, 3423.101461, fromWkt, toWkt);

	/*LOG(INFO)<<"G distance:";

	LOG(INFO)<<"G distance:"<<CommonFun::GreatCirleDistance(0, 98, 347, 98, fromWkt, toWkt, fromRaster->getGeoTransform(), resolution);
	LOG(INFO)<<"E distance:"<<CommonFun::EuclideanDistance(0, 98, 347, 98);

	LOG(INFO)<<"G distance:"<<CommonFun::GreatCirleDistance(173, 98, 347, 98, fromWkt, toWkt, fromRaster->getGeoTransform(), resolution);
	LOG(INFO)<<"E distance:"<<CommonFun::EuclideanDistance(173, 98, 347, 98);
	delete fromRaster;
	delete toRaster;
	exit(1);
	*/
	if (argc==1){
		printf("configure_base_folder, scenario_json, specied_id, result_root, memory_limit(in M), is_overwrite, with_detail (unused)\n");
		exit(1);
	}
	srand(static_cast<unsigned>(time(0)));
    char scenario_json_path[strlen(argv[1]) + strlen(argv[2]) + 30];
    sprintf(scenario_json_path, "%s/Scenario_Configurations/%s.json", argv[1], argv[2]);

	unsigned long memory_limit = atoi(argv[4]);
	bool with_detail = atoi(argv[6]);
	bool is_overwrite = atoi(argv[5]);
	OGRSpatialReference oSourceSRS;
	OGRSpatialReference oTargetSRS;
	char * from_T = const_cast<char *>(fromWkt);
	char * to_T = const_cast<char *>(toWkt);
	oSourceSRS.importFromWkt(&from_T);
	oTargetSRS.importFromWkt(&to_T);
	OGRCoordinateTransformation *poCT = OGRCreateCoordinateTransformation( &oSourceSRS, &oTargetSRS );

	Scenario* scenario = new Scenario(std::string(scenario_json_path), argv[2], argv[1], argv[3], is_overwrite, memory_limit, with_detail, fromWkt, toWkt, resolution, poCT);

	if (scenario->isTerminated()){
		delete scenario;
		printf("Result folder is exist, skip this simulation!\n");
		return EXIT_SUCCESS;
	}
	el::Configurations c;
	c.setGlobally(el::ConfigurationType::Filename, scenario->getTarget() + "/runtime.log");
	el::Loggers::setDefaultConfigurations(c);
	el::Loggers::getLogger("default");
	el::Loggers::setDefaultConfigurations(c, true);

	unsigned status = scenario->run();
	LOG(INFO)<<"Before remove scenario, Memory usage:"<<CommonFun::getCurrentRSS();
	delete scenario;
	LOG(INFO)<<"After  remove scenario, Memory usage:"<<CommonFun::getCurrentRSS();
	if (status==0){
		LOG(INFO)<<"Well done!";
	}
	if (status==1){
		LOG(INFO)<<"To the memory limit, exit!";
	}
	delete fromRaster;
	delete toRaster;
    return EXIT_SUCCESS;
}

