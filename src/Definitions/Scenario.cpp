/*
 * Scenario.cpp
 *
 *  Created on: Oct 15, 2014
 *      Author: qiaohj
 */

#include "Scenario.h"


Scenario::Scenario(Json::Value root, string baseFolder) {
    currentYears = 0;
    totalYears = root.get("total_years", 500000).asInt();
    mask = new Distribution(new RasterObject(root.get("mask", "").asCString()));
    minSpeciesDispersalSpeed = totalYears;
    Json::Value species_json_array = root["species"];
    species.reserve(species_json_array.size());
    for (unsigned index = 0; index<species_json_array.size(); ++index){
        string species_json_path = baseFolder + string("/niche_definations/")
                + species_json_array[index].asString() + string(".json");
        Json::Value species_json = CommonFun::readJson(species_json_path.c_str());
        SpeciesObject* species_item = new SpeciesObject(species_json, this);
        minSpeciesDispersalSpeed = (species_item->getDispersalSpeed()<minSpeciesDispersalSpeed)?
                species_item->getDispersalSpeed():minSpeciesDispersalSpeed;
        species.push_back(species_item);
    }

    Json::Value environment_json_array = root["environments"];
    environments.reserve(environment_json_array.size());
	for (unsigned index = 0; index<environment_json_array.size(); ++index){
		string environment_json_path = baseFolder + string("/environment_curves/")
		        + environment_json_array[index].asString() + string(".json");
		Json::Value environment_json = CommonFun::readJson(environment_json_path.c_str());
		EnvironmentalCurve* environment_item = new EnvironmentalCurve(environment_json);
		environments.push_back(environment_item);
	}
}

float* Scenario::getEnvironmentValue(double longitude, double latitude){
    float* result = new float[environments.size()];
    for (unsigned i=0; i<environments.size(); ++i){
        result[i] = environments[i]->getValue(currentYears, longitude, latitude);
    }
    return result;
}
void Scenario::run(){
    for (unsigned year = currentYears; year<=totalYears; year+=minSpeciesDispersalSpeed){
        printf("Current year:%d, number of species:%zu\n", year, species.size());
        vector<SpeciesObject*> new_species;
        for (unsigned i=0; i<species.size(); ++i){
            vector<SpeciesObject*> new_species_item = species[i]->run(year);
            for (vector<SpeciesObject*>::iterator it = new_species_item.begin() ; it != new_species_item.end(); ++it){
                if (*it!=NULL) new_species.push_back(*it);
            }
        }
        species = new_species;
    }
}
unsigned Scenario::getCurrentYears(){
    return currentYears;
}
Scenario::~Scenario() {
    delete mask;

    for (vector<SpeciesObject*>::iterator it = species.begin() ; it != species.end(); ++it){
        delete *it;
    }

    for (vector<EnvironmentalCurve*>::iterator it = environments.begin() ; it != environments.end(); ++it){
        delete *it;
    }

    vector<SpeciesObject*>().swap(species);
    vector<EnvironmentalCurve*>().swap(environments);

}

