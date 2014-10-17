/*
 * Scenario.h
 *
 *  Created on: Oct 15, 2014
 *      Author: qiaohj
 */
using namespace std;
#include "SlaveSpeciesObject.h"
#include "EnvironmentalCurve.h"
#include "../Universal/CommonFun.h"
#include "../JsonPaster/include/json/json.h"
#include <string>
class Scenario {
private:
	vector<SlaveSpeciesObject*> species;
	vector<EnvironmentalCurve*> environments;
	int totalYears;
	RasterObject* mask;

public:
	Scenario(Json::Value root, string baseFolder);
	virtual ~Scenario();
	float* getEnvironmentValue(int year, double longitude, double latitude);
};
