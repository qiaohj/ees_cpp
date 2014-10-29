/*
 * SpeciesObject.h
 *
 *  Created on: Oct 25, 2014
 *      Author: qiaohj
 */

#ifndef DEFINITIONS_SPECIESOBJECT_H_
#define DEFINITIONS_SPECIESOBJECT_H_

#include <string>
#include <vector>
#include "../JsonPaster/include/json/json.h"
#include "SparseMap.h"
#include "NicheBreadth.h"
#include "GeoLocation.h"

class SpeciesObject {
private:
    unsigned id;
    std::vector<NicheBreadth*> nicheBreadth;
    unsigned dispersalAbility;
    unsigned dispersalSpeed;
    unsigned speciationYears;
    int dispersalMethod;
    int numberOfPath;
    std::vector<GeoLocation*> seeds;

public:
    SpeciesObject(Json::Value p_root);
    virtual ~SpeciesObject();
    unsigned getDispersalAbility();
    unsigned getDispersalSpeed();
    int getDispersalMethod();
    std::vector<GeoLocation*> getSeeds();
    unsigned getID();
    int getNumOfPath();
    std::vector<NicheBreadth*> getNicheBreadth();
};

#endif /* DEFINITIONS_SPECIESOBJECT_H_ */
