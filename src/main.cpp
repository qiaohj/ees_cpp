/**
 * @file main.cpp
 * @brief The main entrance of the simulation application
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 * The parameters to run the simulation are listed below.
 * 1. configure's base folder, which the application can load the configuration for species and scenario.
 * 2. scenario configuration. A JSON format configuration file to set up the parameter of the scenario
 * 3. species configuration. A JSON format configuration file to set up the parameter of the species in the simulation.
 * 4. result folder. A path to save the result
 * 5. memory limit (in MB). A number to set up the maximum memory allocate to the application
 * 6. is overwrite. An ZERO value means skip the simulation if the result folder exists and NONE-ZERO value means run the simulation no matter that the folder exists or not.
 * 7. with detail. An ZERO value means output the details of the simulation or not.
 *
 */



#include <gdal.h>
#include <gdal_priv.h>
#include <ogr_srs_api.h>
#include <ogr_spatialref.h>
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

/*-----------------------------------------
 * Main entrance for the simulation application
 * Parameters
 * 1. configure's base folder, which the application can load the configuration for species and scenario.
 * 2. scenario configuration. A JSON format configuration file to set up the parameter of the scenario
 * 3. species configuration. A JSON format configuration file to set up the parameter of the species in the simulation.
 * 4. result folder. A path to save the result
 * 5. memory limit (in MB). A number to set up the maximum memory allocate to the application
 * 6. is overwrite. An ZERO value means skip the simulation if the result folder exists and NONE-ZERO value means run the simulation no matter
 * that the folder exists or not.
 * 7. with detail. An ZERO value means output the details of the simulation or not.
 *
 *-----------------------------------------*/
int main(int argc, const char* argv[]) {
	// Check the validity of the input
	// If the length of parameters are not satisfied with the required number, the application will skip this simulation and show out a warning.
	if (argc==1){
		printf("configure_base_folder, scenario_json, specied_id, result_root, memory_limit(in M), is_overwrite, with_detail (unused), isSQLite\n");
		exit(1);
	}
	// Set up the timer.
	srand(static_cast<unsigned>(time(0)));

	// Load the parameters
    char scenario_json_path[strlen(argv[1]) + strlen(argv[2]) + 30];
    sprintf(scenario_json_path, "%s/Scenario_Configurations/%s.json", argv[1], argv[2]);

	unsigned long memory_limit = atoi(argv[4]);
	bool with_detail = atoi(argv[6]);
	bool is_overwrite = atoi(argv[5]);
	bool is_sqlite =  atoi(argv[7]);
	//initialize the main scenario
	Scenario* scenario = new Scenario(std::string(scenario_json_path), argv[2], argv[1], argv[3], is_overwrite, memory_limit, with_detail, is_sqlite);

	if (scenario->isTerminated()){
		//delete scenario;
		printf("Result folder is exist, skip this simulation!\n");
		return EXIT_SUCCESS;
	}

	//initialize the logger
	el::Configurations c;
	c.setGlobally(el::ConfigurationType::Filename, scenario->getTarget() + "/runtime.log");
	el::Loggers::setDefaultConfigurations(c);
	el::Loggers::getLogger("default");
	el::Loggers::setDefaultConfigurations(c, true);

	//run the simulation and get the runtime status
	unsigned status = scenario->run();

	if (status==0){
		LOG(INFO)<<"Done!";
	}
	if (status==1){
		LOG(INFO)<<"To the memory limit, exit!";
	}
	//delete fromRaster;
	//delete toRaster;
    return EXIT_SUCCESS;
}

