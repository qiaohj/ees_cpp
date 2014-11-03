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
#include "../Definitions/SparseMap.h"
#include "../Universal/CommonFun.h"
#include "../Universal/const.h"

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
    std::string burn_in_curve;
    std::string part_1_curve;
    std::string part_2_curve;
    std::string plateau_1_curve;
    std::string plateau_2_curve;
    float noData;

public:
	EnvironmentalCurve(Json::Value p_root);
	virtual ~EnvironmentalCurve();
    unsigned getBurnInYears();
    SparseMap* getValues(unsigned p_year);
	float readByLL(float p_curve_value, unsigned p_year, double p_longitude, double p_latitude);
	float readByXY(float p_curve_value, unsigned p_year, unsigned p_x, unsigned p_y);
	float getCurveValue(unsigned p_year);
};

#endif /* EnvironmentalCurve_H */
