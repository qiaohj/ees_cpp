
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
    char path[] = "/home/qiaohj/workspace/NicheBreadth/data/Scenarios/Scenario.json";
	Json::Value root_Scenario = CommonFun::readJson(path);
	Scenario* scenario = new Scenario(root_Scenario, "/home/qiaohj/workspace/NicheBreadth/data");
	vector<string> result;
	using namespace boost::numeric::ublas;
	mapped_matrix<double> m (1944000, 5000, 1944000 * 5000);

	for (unsigned i=0;i<=500000;i+=5000){
	    printf("%d\n", i);
	    float* results = scenario->getEnvironmentValue(i, 100, 59);
	    char line[30];
	    sprintf(line, "%.6f, %.6f", results[0], results[1]);
	    result.push_back(line);


	    delete[] results;
	}


	CommonFun::writeFile(result, "/home/qiaohj/workspace/NicheBreadth/data/temp/environments.csv");
	delete scenario;
	printf("done!");
	return EXIT_SUCCESS;
}



