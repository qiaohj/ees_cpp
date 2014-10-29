/*
 * Scenario.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: qiaohj
 */

#include "Scenario.h"

Scenario::Scenario(Json::Value p_root, std::string p_base_folder,
        std::string p_target) {
    baseFolder = p_base_folder;
    target = p_target;
    CommonFun::createFolder(target.c_str());
    totalYears = p_root.get("total_years", 500000).asInt();
    RasterObject* mask_raster = new RasterObject(
            p_root.get("mask", "").asCString());
    geoTrans = new double[6];
    memcpy(geoTrans, mask_raster->getGeoTransform(), 6 * sizeof(*geoTrans));

    mask = new SparseMap(mask_raster, true);
    xSize = mask_raster->getXSize();
    ySize = mask_raster->getYSize();
    minSpeciesDispersalSpeed = totalYears;
    Json::Value species_json_array = p_root["species"];

    for (unsigned index = 0; index < species_json_array.size(); ++index) {
        std::string species_json_path = baseFolder
                + std::string("/niche_definations/")
                + species_json_array[index].asString() + std::string(".json");
        Json::Value species_json = CommonFun::readJson(
                species_json_path.c_str());
        SpeciesObject* species = new SpeciesObject(species_json);
        createSpeciesFolder(species->getID());
        std::vector<GeoLocation*> seeds = species->getSeeds();
        for (unsigned i = 0; i < seeds.size(); ++i) {
            unsigned x, y;
            CommonFun::LL2XY(geoTrans, seeds[i]->getLongitude(),
                    seeds[i]->getLatitude(), &x, &y);
            IndividualOrganism* individualOrganism = new IndividualOrganism(0,
                    species, NULL);
            if (cells.find(y * xSize + x) == cells.end()) {
                CellObject* cell = new CellObject(x, y);
                cells[y * xSize + x] = cell;
            }
            cells[y * xSize + x]->addIndividualOrganism(individualOrganism);
        }

        minSpeciesDispersalSpeed =
                (species->getDispersalSpeed() < minSpeciesDispersalSpeed) ?
                        species->getDispersalSpeed() : minSpeciesDispersalSpeed;
    }

    Json::Value environment_json_array = p_root["environments"];
    environments.reserve(environment_json_array.size());
    for (unsigned index = 0; index < environment_json_array.size(); ++index) {
        std::string environment_json_path = baseFolder
                + std::string("/environment_curves/")
                + environment_json_array[index].asString()
                + std::string(".json");
        Json::Value environment_json = CommonFun::readJson(
                environment_json_path.c_str());
        EnvironmentalCurve* environment_item = new EnvironmentalCurve(
                environment_json);
        environments.push_back(environment_item);
    }
    delete mask_raster;
}
void Scenario::createSpeciesFolder(unsigned p_species_id) {
    char speciesFolder[target.length() + 6];
    sprintf(speciesFolder, "%s/%s", target.c_str(),
            CommonFun::fixedLength(p_species_id, 5).c_str());
    CommonFun::createFolder(speciesFolder);

    char dispersalFolder[target.length() + 6 + 10];
    sprintf(dispersalFolder, "%s/dispersal", speciesFolder);
    CommonFun::createFolder(dispersalFolder);
}
void Scenario::run() {
    std::vector<std::string> env_output;
    unsigned x = 99999, y = 99999;
    for (unsigned year = 0; year <= totalYears; year +=
            minSpeciesDispersalSpeed) {
        printf("Current year:%d\n", year);
        if (environment_maps.find(year) == environment_maps.end()) {
            environment_maps[year] = getEnvironmenMap(year);
        }
        //save the env data
        if (x == 99999) {
            int value;
            environment_maps[year][0]->getFirstValues(&x, &y, &value);
        }
        char line[20];
        sprintf(line, "%u,%u,%u,%d", year, x, y,
                environment_maps[year][0]->readByXY(x, y));
        std::vector<SparseMap*> current_environments = environment_maps[year];

        env_output.push_back(line);
        boost::unordered_map<unsigned, CellObject*> new_cells;
        for (auto cell_it : cells) {
            boost::unordered_map<unsigned, CellObject*> new_subcells =
                    cell_it.second->run(year, current_environments, xSize,
                            ySize);
            for (auto it : new_subcells) {
                if (new_cells.find(it.first) == new_cells.end()) {
                    new_cells[it.first] = it.second;
                } else {
                    new_cells[it.first]->merge(it.second);
                }
            }

        }

        for (auto cell_it : new_cells) {
            if (cells.find(cell_it.first) == cells.end()) {
                cells[cell_it.first] = cell_it.second;
            } else {
                cells[cell_it.first]->merge(cell_it.second);
            }
        }

        //remove the unsuitable organisms
        LOG(INFO)<<"begin to remove unsuitable organisms";
        for (auto cell_it : cells) {
            cell_it.second->removeUnsuitable(current_environments);
        }
        LOG(INFO)<<"end to remove unsuitable organisms";

        LOG(INFO)<<"begin to generate distribution maps";
        //generate the distribution map for every species
        boost::unordered_map<unsigned, SparseMap*> distribution_maps;
        for (auto cell_it : cells) {
            for (auto io_it : cell_it.second->getIndividualOrganisms()) {
                if (distribution_maps.find(io_it->getSpeciesID())
                        == distribution_maps.end()) {
                    distribution_maps[io_it->getSpeciesID()] = new SparseMap(
                            xSize, ySize);
                    distribution_maps[io_it->getSpeciesID()]->setValue(
                            cell_it.second->getX(), cell_it.second->getY(), 1);
                } else {
                    distribution_maps[io_it->getSpeciesID()]->setValue(
                            cell_it.second->getX(), cell_it.second->getY(),
                            distribution_maps[io_it->getSpeciesID()]->readByXY(
                                    cell_it.second->getX(),
                                    cell_it.second->getY()) + 1);
                }

            }
        }
        LOG(INFO)<<"end to generate distribution maps";
        for (auto it : distribution_maps) {
            char tiffName[target.length() + 28];
            sprintf(tiffName, "%s/%s/dispersal/%s.tif", target.c_str(),
                    CommonFun::fixedLength(it.first, 5).c_str(),
                    CommonFun::fixedLength(year, 7).c_str());
            RasterController::writeGeoTIFF(tiffName, xSize, ySize, geoTrans,
                    it.second->toArray(), (double) NODATA, GDT_Int32);
        }
    }
    char filepath[target.length() + 15];
    sprintf(filepath, "%s/env_curve.csv", target.c_str());
    CommonFun::writeFile(env_output, filepath);
    env_output.clear();
}
Scenario::~Scenario() {
    // TODO Auto-generated destructor stub
}
std::vector<SparseMap*> Scenario::getEnvironmenMap(unsigned p_year) {
    std::vector<SparseMap*> result;
    for (unsigned i = 0; i < environments.size(); ++i) {
        result.push_back(environments[i]->getValues(p_year));
    }
    return result;
}
