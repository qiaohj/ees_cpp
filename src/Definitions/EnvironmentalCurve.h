/*
 * EnvironmentalCurve.h
 *
 *  Created on: Oct 14, 2014
 *      Author: qiaohj
 */

#ifndef EnvironmentalCurve_H
#define EnvironmentalCurve_H

#include <string>
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include "../ExpressionParser/parser.h"
#include "../JsonPaster/include/json/json.h"
#include "../RasterIO/RasterObject.h"
#include "../RasterIO/SparseMap.h"
#include "../Universal/CommonFun.h"
#include "../Universal/const.h"

using namespace std;
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
    float noData;

public:
	EnvironmentalCurve(Json::Value root);
	virtual ~EnvironmentalCurve();
    int getBurnInYears();
    SparseMap* getValues(unsigned year);
	float readByLL(float curve_value, unsigned year, double longitude, double latitude);
	float readByXY(float curve_value, unsigned year, unsigned x, unsigned y);
	float getCurveValue(unsigned year);

};

#endif /* EnvironmentalCurve_H */
