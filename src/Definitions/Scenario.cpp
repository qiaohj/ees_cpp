/*
 * Scenario.cpp
 *
 *  Created on: Oct 15, 2014
 *      Author: qiaohj
 */

#include "Scenario.h"


Scenario::Scenario(Json::Value root, string baseFolder) {
    totalYears = root.get("total_years", 500000).asInt();
    mask = new RasterObject(root.get("mask", "").asCString());

    Json::Value species_json_array = root["species"];
    species.reserve(species_json_array.size());
    for (unsigned int index = 0; index < species_json_array.size(); ++index){
        string species_json_path = baseFolder + string("/niche_definations/")
                + species_json_array[index].asString() + string(".json");
        Json::Value species_json = CommonFun::readJson(species_json_path.c_str());
        SlaveSpeciesObject* species_item = new SlaveSpeciesObject(species_json);
        species.push_back(species_item);
    }

    Json::Value environment_json_array = root["environments"];
    environments.reserve(environment_json_array.size());
	for (unsigned int index = 0; index < environment_json_array.size(); ++index){
		string environment_json_path = baseFolder + string("/environment_curves/")
		        + environment_json_array[index].asString() + string(".json");
		Json::Value environment_json = CommonFun::readJson(environment_json_path.c_str());
		EnvironmentalCurve* environment_item = new EnvironmentalCurve(environment_json);
		environments.push_back(environment_item);
	}
}
float* Scenario::getEnvironmentValue(int year, double longitude, double latitude){
    float* result = new float[environments.size()];
    for (unsigned int i=0;i<environments.size();i++){
        result[i] = environments[i]->getValue(year, longitude, latitude);
    }
    return result;
}
Scenario::~Scenario() {
    delete mask;

    for (vector<SlaveSpeciesObject*>::iterator it = species.begin() ; it != species.end(); ++it){
        delete *it;
    }

    for (vector<EnvironmentalCurve*>::iterator it = environments.begin() ; it != environments.end(); ++it){
        delete *it;
    }

    vector<SlaveSpeciesObject*>().swap(species);
    vector<EnvironmentalCurve*>().swap(environments);

}

