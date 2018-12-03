/**
 * @file EnvironmentalHadley.h
 * @brief Class EnvironmentalHadley. A class to handle the environmental layers from the Hadley guys.
 * @author Huijie Qiao
 * @version 1.0
 * @date 11/25/2018
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
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
/**
 * @brief A class to handle the environmental layers from the Hadley guys.
 */
class EnvironmentalHadley {
private:
    boost::unordered_map<unsigned, SparseMap*> layers;
    unsigned burnInYears;
    float noData;
    double* geoTrans;
public:

    /**
     * @brief Constructor of EnvironmentalHadley class
     * @param p_basefolder The base folder where the application can load the raster files.
     * @param p_geoTrans The GeoTransform matrix of the raster layers, can be extract from the mask or any environmental layers.
     * @param p_burn_in_year Burn-in year of the simulation
     * @param p_begin_year Start time step of the simulation
     * @param p_end_year End time step of the simulation
     * @param p_step The step length of the simualtion
     */
	EnvironmentalHadley(const std::string p_basefolder, double* p_geoTrans, unsigned p_burn_in_year,
			unsigned p_begin_year, unsigned p_end_year, unsigned p_step);

	/**
	 * @brief Destructor of EnvironmentalHadley class
	 *
	 * release all the resources
	 */
	virtual ~EnvironmentalHadley();

	/**
	 * @brief Return the burn in year.
	 */
    unsigned getBurnInYears();

    /**
     * @brief Get the environmental layer of the given time step
     * @param p_year time step
     * @return A SparseMap object of the environmental layer of the given time step.
     */
    SparseMap* getValues(unsigned p_year);

    /**
     * @brief Get the value based on the longitude and latitude
     * @param p_year time step
     * @param p_longitude longitude
     * @param p_latitude latitude
     * @return the value on the longitude and latitude
     */
	float readByLL(unsigned p_year, double p_longitude, double p_latitude);

	/**
	 * @brief Get the value based on the x, y of the matrix
	 * @param p_year time step
	 * @param p_x x
	 * @param p_y y
	 * @return the value on the longitude and latitude
	 */
	float readByXY(unsigned p_year, unsigned p_x, unsigned p_y);
};

#endif /* EnvironmentalHadley_H */
