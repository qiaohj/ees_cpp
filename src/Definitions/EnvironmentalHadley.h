/*
 * EnvironmentalHadley.h
 *
 *  Created on: Oct 14, 2014
 *      Author: qiaohj
 */

#ifndef EnvironmentalHadley_H
#define EnvironmentalHadley_H

#include <string>
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include <boost/unordered_map.hpp>
#include "../ExpressionParser/parser.h"
#include "../JsonPaster/include/json/json.h"
#include "../RasterIO/RasterObject.h"
#include "../Definitions/SparseMap.h"
#include "../Universal/CommonFun.h"
#include "../Universal/const.h"

class EnvironmentalHadley {
private:
    boost::unordered_map<unsigned, SparseMap*> layers;
    unsigned burnInYears;
    float noData;
    double* geoTrans;
public:
	EnvironmentalHadley(const std::string p_basefolder, double* p_geoTrans, unsigned p_burn_in_year,
			unsigned p_begin_year, unsigned p_end_year, unsigned p_step);
	virtual ~EnvironmentalHadley();
    unsigned getBurnInYears();
    SparseMap* getValues(unsigned p_year);
	float readByLL(unsigned p_year, double p_longitude, double p_latitude);
	float readByXY(unsigned p_year, unsigned p_x, unsigned p_y);
};

#endif /* EnvironmentalHadley_H */
