/*
 * SpeciesObject.h
 *
 *  Created on: Oct 15, 2014
 *      Author: qiaohj
 */

#ifndef SpeciesObject_H
#define SpeciesObject_H

#include <string>
#include "../JsonPaster/include/json/json.h"
#include "../RasterIO/SparseMap.h"
#include "../Universal/CommonType.h"

using namespace std;

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
    SpeciesObject(Json::Value root, unsigned x_size, unsigned y_size, double* geoTrans);
    virtual ~SpeciesObject();
    unsigned getDispersalAbility();
    unsigned getDispersalSpeed();
    vector<SpeciesObject*> run(const unsigned current_year, const SparseMap* environmental_values);
};

#endif /* SpeciesObject_H */
