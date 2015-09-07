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

_INITIALIZE_EASYLOGGINGPP

//configure_base_folder, scenario_json, specied_id, result_root, memory_limit(in M), tif_limit
///home/huijieqiao/NB_NEW/Environments scenario /home/huijieqiao/temp 4000 1000 1
int main(int argc, const char* argv[]) {
	srand(static_cast<unsigned>(time(0)));
    char scenario_json_path[strlen(argv[1]) + strlen(argv[2]) + 30];
    sprintf(scenario_json_path, "%s/Scenario_Configurations/%s.json", argv[1], argv[2]);
	unsigned long memory_limit = atoi(argv[4]);
	bool with_detail = atoi(argv[6]);
	unsigned tif_limit = atoi(argv[5]);
	Scenario* scenario = new Scenario(std::string(scenario_json_path), argv[2], argv[1], argv[3], tif_limit, memory_limit, with_detail);

	if (scenario->isFinish()){
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
//	LOG(INFO)<<"Before remove scenario, Memory usage:"<<CommonFun::getCurrentRSS();
	delete scenario;
//	LOG(INFO)<<"After  remove scenario, Memory usage:"<<CommonFun::getCurrentRSS();
	if (status==0){
		LOG(INFO)<<"Well done!";
	}
	if (status==1){
		LOG(INFO)<<"To the memory limit, exit!";
	}

    return EXIT_SUCCESS;
}

