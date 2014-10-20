/*
 * SpeciesObject.h
 *
 *  Created on: Oct 15, 2014
 *      Author: qiaohj
 */
#include <string>
#include <boost/unordered_set.hpp>
#include "../JsonPaster/include/json/json.h"
#include "Distribution.h"

using namespace std;
class Scenario;
typedef boost::unordered_map<unsigned, Distribution> map;

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
    Scenario* scenario;
    map distributions;

public:
    SpeciesObject(Json::Value root, Scenario* p_scenario);
    unsigned getDispersalAbility();
    unsigned getDispersalSpeed();
    vector<SpeciesObject*> run(unsigned year);
    virtual ~SpeciesObject();
};
