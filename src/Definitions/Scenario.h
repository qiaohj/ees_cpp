/*
 * Scenario.h
 *
 *  Created on: Oct 25, 2014
 *      Author: qiaohj
 */

#ifndef DEFINITIONS_SCENARIO_H_
#define DEFINITIONS_SCENARIO_H_
#include <string>
#include <boost/unordered_map.hpp>
#include "../Universal/CommonFun.h"
#include "../JsonPaster/include/json/json.h"
#include "../Universal/CommonType.h"
#include "SparseMap.h"
#include "EnvironmentalCurve.h"
#include "CellObject.h"

using namespace std;

class Scenario {
private:
    boost::unordered_map<unsigned, CellObject*> cells;
    vector<EnvironmentalCurve*> environments;
    unsigned totalYears;
    unsigned minSpeciesDispersalSpeed;
    SparseMap* mask;
    hashmap_multiply environment_maps;
    string baseFolder;
    string target;
    double* geoTrans;
    unsigned x_size, y_size;
    void cleanSpecies();
    void cleanEnvironments();
public:
    Scenario(Json::Value p_root, string p_base_folder, string p_target);
    virtual ~Scenario();
    void run();
    float* getEnvironmentValue(unsigned p_year, double p_longitude, double p_latitude);
    vector<SparseMap*> getEnvironmenMap(unsigned p_year);
};

#endif /* DEFINITIONS_SCENARIO_H_ */
