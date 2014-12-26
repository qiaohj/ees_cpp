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

//configure_base_folder, scenario_file, result_root, memory_limit(in M), tif_limit
///home/huijieqiao/workspace/NicheBreadth/data scenario.json /home/huijieqiao/temp 8000 1000
int main(int argc, const char* argv[]) {

    signal(SIGSEGV, handler);

    char path[strlen(argv[1]) + strlen(argv[2]) + 20];
    sprintf(path, "%s/scenarios/%s", argv[1], argv[2]);

	Json::Value root_Scenario = CommonFun::readJson(path);
	unsigned long memory_limit = atoi(argv[4]);
	unsigned tif_limit = atoi(argv[5]);
	Scenario* scenario = new Scenario(root_Scenario, argv[1], argv[3], memory_limit, tif_limit);
	scenario->run();
	LOG(INFO)<<"Before remove scenario, Memory usage:"<<CommonFun::getCurrentRSS();
	delete scenario;
	LOG(INFO)<<"After  remove scenario, Memory usage:"<<CommonFun::getCurrentRSS();

    printf("done!");
    return EXIT_SUCCESS;
}

