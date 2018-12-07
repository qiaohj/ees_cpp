/**
 * @file EnvironmentalCurve.h
 * @brief Class EnvironmentalCurve. Used to generate a virtual environmental variable based on the configuration. This class was used in the Qiao et al. 2016. Am. Nat. paper. We keep it in the source code to honor it
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
 * @brief Used to generate a virtual environmental variable based on the configuration. This class was used in the Qiao et al. 2016. Am. Nat. paper. We keep it in the source code to honor it
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
	/**
	 * @brief Return the burn-in year used in the simulation.
	 */
    unsigned getBurnInYears();
    /**
     * @brief Return the environmental layers by a given time step.
     * @param p_year time step
     * @return
     */
    SparseMap* getValues(unsigned p_year);
    /**
	 * @brief Get the value based on the longitude and latitude
	 * @param p_year time step
	 * @param p_longitude longitude
	 * @param p_latitude latitude
	 * @return the value on the longitude and latitude
	 */
	float readByLL(float p_curve_value, unsigned p_year, double p_longitude, double p_latitude);
	/**
	 * @brief Get the value based on the x, y of the matrix
	 * @param p_year time step
	 * @param p_x x
	 * @param p_y y
	 * @return the value on the longitude and latitude
	 */
	float readByXY(float p_curve_value, unsigned p_year, unsigned p_x, unsigned p_y);
	float getCurveValue(unsigned p_year);
};

#endif /* EnvironmentalCurve_H */
