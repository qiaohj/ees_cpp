/*
 * SpeciesObject.h
 *
 *  Created on: Oct 15, 2014
 *      Author: qiaohj
 */

#ifndef SpeciesObject_H
#define SpeciesObject_H

#include <string>
#include <boost/unordered_map.hpp>
#include "../JsonPaster/include/json/json.h"
#include "SparseMap.h"

using namespace std;

typedef boost::unordered_map <unsigned, SparseMap> hashmap;

class SpeciesObject {
private:
    string id;
    vector<float*> nicheBreadth;
    unsigned dispersalAbility;
    unsigned dispersalSpeed;
    unsigned nextRunYear;
    unsigned speciationYears;
    int dispersalMethod;
    int numberOfPath;
    vector<float*> seeds;
    hashmap distributions;

public:
    SpeciesObject(Json::Value root);
    virtual ~SpeciesObject();
    unsigned getDispersalAbility();
    unsigned getDispersalSpeed();
    vector<SpeciesObject*> run(const unsigned current_year, const float* environmental_values);
};

#endif /* SpeciesObject_H */
