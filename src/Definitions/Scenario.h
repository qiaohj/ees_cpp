
/*
 * Scenario.h
 *
 *  Created on: Oct 15, 2014
 *      Author: qiaohj
 */
#include <string>


#include "EnvironmentalCurve.h"
#include "SpeciesObject.h"
#include "../Universal/CommonFun.h"
#include "../JsonPaster/include/json/json.h"

using namespace std;
class Scenario {
private:
	vector<SpeciesObject*> species;
	vector<EnvironmentalCurve*> environments;
	unsigned currentYears;
	unsigned totalYears;
	Distribution* mask;
	unsigned minSpeciesDispersalSpeed;
public:
	Scenario(Json::Value root, string baseFolder);
	void run();
	unsigned getCurrentYears();
	virtual ~Scenario();
	float* getEnvironmentValue(double longitude, double latitude);
};


