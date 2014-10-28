
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
#include "../Definitions/SparseMap.h"
#include "../Universal/CommonFun.h"
#include "../JsonPaster/include/json/json.h"

using namespace std;

class Scenario {
private:
	vector<SpeciesObject*> species;
	vector<EnvironmentalCurve*> environments;
	unsigned totalYears;
	unsigned minSpeciesDispersalSpeed;
	SparseMap* mask;
	hashmap_multiply environment_maps;
	string baseFolder;
	string target;
	double* geoTrans;
	void cleanSpecies();
	void cleanEnvironments();
public:
	Scenario(Json::Value p_root, string p_base_folder, string p_target);
	void run();
	virtual ~Scenario();
	float* getEnvironmentValue(unsigned p_year, double p_longitude, double p_latitude);
	vector<SparseMap*> getEnvironmenMap(unsigned p_year);
};

#endif /* Scenario_H */

