/*
 * SpeciesObject.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: qiaohj
 */

#include "SpeciesObject.h"

SpeciesObject::SpeciesObject(Json::Value p_root) {
    id = p_root.get("id", "").asInt();
    dispersalAbility = p_root.get("dispersal_ability", 1).asInt();
    dispersalSpeed = p_root.get("dispersal_speed", 1000).asInt();
    dispersalMethod = p_root.get("dispersal_method", 1).asInt();
    numberOfPath = p_root.get("number_of_path", -1).asInt();
    speciationYears = p_root.get("speciation_years", 10000).asInt();

    Json::Value niche_breadth_array = p_root["niche_breadth"];
    for (unsigned index = 0; index < niche_breadth_array.size(); ++index) {
        Json::Value niche_breadth_json = niche_breadth_array[index];
        NicheBreadth* niche_breadth = new NicheBreadth(
                niche_breadth_json[0].asFloat(),
                niche_breadth_json[1].asFloat());
        nicheBreadth.push_back(niche_breadth);
    }

    Json::Value initial_seeds_array = p_root["initial_seeds"];
    for (unsigned index = 0; index < initial_seeds_array.size(); ++index) {
        Json::Value initial_seeds_json = initial_seeds_array[index];
        GeoLocation* initial_seed = new GeoLocation(
                initial_seeds_json[0].asDouble(),
                initial_seeds_json[1].asDouble());
        seeds.push_back(initial_seed);
    }

}

SpeciesObject::~SpeciesObject() {
    CommonFun::clearVector(nicheBreadth);
    CommonFun::clearVector(seeds);
}

unsigned SpeciesObject::getDispersalAbility() {
    return dispersalAbility;
}
unsigned SpeciesObject::getDispersalSpeed() {
    return dispersalSpeed;
}
std::vector<GeoLocation*> SpeciesObject::getSeeds() {
    return seeds;
}
unsigned SpeciesObject::getID() {
    return id;
}
int SpeciesObject::getDispersalMethod() {
    return dispersalMethod;
}
int SpeciesObject::getNumOfPath() {
    return numberOfPath;
}
std::vector<NicheBreadth*> SpeciesObject::getNicheBreadth() {
    return nicheBreadth;
}
unsigned SpeciesObject::getSpeciationYears(){
    return speciationYears;
}
