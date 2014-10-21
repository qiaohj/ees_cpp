
/*
 * Scenario.h
 *
 *  Created on: Oct 15, 2014
 *      Author: qiaohj
 */

#ifndef Scenario_H
#define Scenario_H

#include <string>
#include "EnvironmentalCurve.h"
#include "SpeciesObject.h"
#include "../RasterIO/SparseMap.h"
#include "../Universal/CommonFun.h"
#include "../JsonPaster/include/json/json.h"

using namespace std;

class Scenario {
private:
	vector<SpeciesObject*> species;
	vector<EnvironmentalCurve*> environments;
	unsigned currentYears;
	unsigned totalYears;
	unsigned minSpeciesDispersalSpeed;
	SparseMap* mask;
	hashmap environment_maps;
	double* geoTrans;
public:
	Scenario(Json::Value root, string baseFolder);
	void run();
	virtual ~Scenario();
	float* getEnvironmentValue(unsigned year, double longitude, double latitude);
	SparseMap* getEnvironmenMap(unsigned year);
};

#endif /* Scenario_H */

