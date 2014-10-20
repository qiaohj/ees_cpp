
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




int main(int argc, const char* argv[]) {
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



