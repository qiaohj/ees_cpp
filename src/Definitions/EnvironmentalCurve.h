/**
 * @file EnvironmentalCurve.h
 * @brief Class EnvironmentalCurve. It is used to create a virtual environmental variables based on the configuration. It was used in the Am. Nat. paper. We keep it in the source code, just for the honor.
 * @author Huijie Qiao
 * @version 1.0
 * @date 11/25/2018
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
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
/**
 * @brief used to create a virtual environmental variables based on the configuration. It was used in the Am. Nat. paper. We keep it in the source code, just for the honor.
 */
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
    /**
	 * @brief Constructor of EnvironmentalCurve class
	 * @param json_path configuration file
	 */
	EnvironmentalCurve(const std::string json_path);

	/**
	 * @brief Destructor of EnvironmentalCurve class
	 *
	 * release all the resources
	 */
	virtual ~EnvironmentalCurve();
    unsigned getBurnInYears();
    SparseMap* getValues(unsigned p_year);
	float readByLL(float p_curve_value, unsigned p_year, double p_longitude, double p_latitude);
	float readByXY(float p_curve_value, unsigned p_year, unsigned p_x, unsigned p_y);
	float getCurveValue(unsigned p_year);
};

#endif /* EnvironmentalCurve_H */
