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

using namespace std;
class SpeciesObject {
private:
    unsigned id;
    vector<float*> nicheBreadth;
    unsigned dispersalAbility;
    unsigned dispersalSpeed;
    unsigned speciationYears;
    int dispersalMethod;
    int numberOfPath;
    vector<float*> seeds;

public:
    SpeciesObject(Json::Value p_root);
    virtual ~SpeciesObject();
    unsigned getDispersalAbility();
    unsigned getDispersalSpeed();
    int getDispersalMethod();
    vector<float*> getSeeds();
    unsigned getID();
    int getNumOfPath();
    vector<float*> getNicheBreadth();
};

#endif /* DEFINITIONS_SPECIESOBJECT_H_ */
