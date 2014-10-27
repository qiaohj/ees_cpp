/*
 * Scenario.cpp
 *
 *  Created on: Oct 15, 2014
 *      Author: qiaohj
 */

#include "Scenario.h"

Scenario::Scenario(Json::Value p_root, string p_base_folder, string p_target) {
    baseFolder = p_base_folder;
    target = p_target;
    totalYears = p_root.get("total_years", 500000).asInt();
    RasterObject* mask_raster = new RasterObject(
            p_root.get("mask", "").asCString());
    geoTrans = new double[6];
    memcpy(geoTrans, mask_raster->getGeoTransform(), 6 * sizeof(*geoTrans));

    mask = new SparseMap(mask_raster, true);
    unsigned x_size = mask_raster->getXSize();
    unsigned y_size = mask_raster->getYSize();
    minSpeciesDispersalSpeed = totalYears;
    Json::Value species_json_array = p_root["species"];
    species.reserve(species_json_array.size());
    for (unsigned index = 0; index < species_json_array.size(); ++index) {
        string species_json_path = baseFolder + string("/niche_definations/")
                + species_json_array[index].asString() + string(".json");
        Json::Value species_json = CommonFun::readJson(
                species_json_path.c_str());
        SpeciesObject* species_item = new SpeciesObject(species_json, x_size,
                y_size, geoTrans);
        minSpeciesDispersalSpeed =
                (species_item->getDispersalSpeed() < minSpeciesDispersalSpeed) ?
                        species_item->getDispersalSpeed() :
                        minSpeciesDispersalSpeed;
        species.push_back(species_item);
    }

    Json::Value environment_json_array = p_root["environments"];
    environments.reserve(environment_json_array.size());
    for (unsigned index = 0; index < environment_json_array.size(); ++index) {
        string environment_json_path = baseFolder
                + string("/environment_curves/")
                + environment_json_array[index].asString() + string(".json");
        Json::Value environment_json = CommonFun::readJson(
                environment_json_path.c_str());
        EnvironmentalCurve* environment_item = new EnvironmentalCurve(
                environment_json);
        environments.push_back(environment_item);
    }
    delete mask_raster;
}

float* Scenario::getEnvironmentValue(unsigned p_year, double p_longitude,
        double p_latitude) {
    float* result = new float[environments.size()];
    for (unsigned i = 0; i < environments.size(); ++i) {
        result[i] = environments[i]->readByLL(-1, p_year, p_longitude, p_latitude);
    }
    return result;
}
void Scenario::run() {

    vector<string> env_output;
    unsigned x = 99999, y = 99999;
    for (unsigned year = 0; year <= totalYears; year +=
            minSpeciesDispersalSpeed) {
        printf("Current year:%d, number of species:%zu\n", year,
                species.size());
        vector<SpeciesObject*> new_species;
        if (environment_maps.find(year) == environment_maps.end()) {
            environment_maps[year] = getEnvironmenMap(year);
        }
        //save the env data
        if (x == 99999) {
            vector<CellObject*> values = environment_maps[year][0]->getValues();
            CellObject* map = values[0];
            x = map->getX();
            y = map->getY();
            CellObject::clearCellObject(values);
        }
        char line[20];
        sprintf(line, "%u,%u,%u,%d", year, x, y,
                environment_maps[year][0]->readByXY(x, y));
        env_output.push_back(line);
        vector<SparseMap*> current_environments = environment_maps[year];
        for (unsigned i = 0; i < species.size(); ++i) {
            char species_folder[target.length() + 6];
            sprintf(species_folder, "%s/%s", target.c_str(),
                    CommonFun::fixedLength(species[i]->getID(), 5).c_str());
            CommonFun::createFolder(species_folder);
            vector<SpeciesObject*> new_species_item = species[i]->run(year,
                    current_environments, target, geoTrans);
            for (vector<SpeciesObject*>::iterator it = new_species_item.begin();
                    it != new_species_item.end(); ++it) {
                if (*it != NULL)
                    new_species.push_back(*it);
            }
        }
        species = new_species;
    }
    char filepath[target.length() + 15];
    sprintf(filepath, "%s/env_curve.csv", target.c_str());
    CommonFun::writeFile(env_output, filepath);
    env_output.clear();
}
vector<SparseMap*> Scenario::getEnvironmenMap(unsigned p_year) {
    vector<SparseMap*> result;
    for (unsigned i = 0; i < environments.size(); ++i) {
        result.push_back(environments[i]->getValues(p_year));
    }
    return result;
}
void Scenario::cleanSpecies() {
    for (vector<SpeciesObject*>::iterator it = species.begin();
            it != species.end(); ++it) {
        delete *it;
    }
    vector<SpeciesObject*>().swap(species);
}
void Scenario::cleanEnvironments() {
    for (vector<EnvironmentalCurve*>::iterator it = environments.begin();
            it != environments.end(); ++it) {
        delete *it;
    }
    vector<EnvironmentalCurve*>().swap(environments);

    for (hashmap_multiply::iterator it = environment_maps.begin();
            it != environment_maps.end();) {
        for (vector<SparseMap*>::iterator it2 = it->second.begin();
                it2 != it->second.end(); ++it2) {
            delete *it2;
        }
        vector<SparseMap*>().swap(it->second);
        it = environment_maps.erase(it);
    }
    environment_maps.clear();
}
Scenario::~Scenario() {
    delete mask;
    cleanSpecies();
    cleanEnvironments();
    delete[] geoTrans;
}

