/*
 * EnvironmentalCurve.h
 *
 *  Created on: Oct 14, 2014
 *      Author: qiaohj
 */

using namespace std;
#include <string>
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include "../ExpressionParser/parser.h"
#include "../JsonPaster/include/json/json.h"
#include "../RasterIO/RasterObject.h"
#include "../Universal/Templete.cpp"

class EnvironmentalCurve {
private:
    RasterObject* glacial;
    RasterObject* interglacial;
    unsigned burnInYears;
    unsigned part_1_years;
    unsigned part_2_years;
    unsigned plateau_1_years;
    unsigned plateau_2_years;
    unsigned cycleYears;
    string burn_in_curve;
    string part_1_curve;
    string part_2_curve;
    string plateau_1_curve;
    string plateau_2_curve;

public:
	EnvironmentalCurve(Json::Value root);
	virtual ~EnvironmentalCurve();
    int getBurnInYears();
	float getValue(unsigned year, double longitude, double latitude);
};


