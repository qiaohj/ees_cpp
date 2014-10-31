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
        this->species.push_back(species);
        createSpeciesFolder(species->getID());
        std::vector<GeoLocation*> seeds = species->getSeeds();
        for (unsigned i = 0; i < seeds.size(); ++i) {

            unsigned x, y;
            CommonFun::LL2XY(geoTrans, seeds[i]->getLongitude(),
                    seeds[i]->getLatitude(), &x, &y);
            IndividualOrganism* individualOrganism = new IndividualOrganism(0,
                    species, NULL, x, y);
            actived_individualOrganisms[y * xSize + x] = individualOrganism;
            all_individualOrganisms.push_back(individualOrganism);
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
        char line[30];
        int v = environment_maps[year][0]->readByXY(x, y);
        sprintf(line, "%u,%u,%u,%d", year, x, y, v);

        std::vector<SparseMap*> current_environments = environment_maps[year];

        env_output.push_back(line);

        LOG(INFO)<<"start to simulate organism by organism. No. of organisms. " << actived_individualOrganisms.size();

        for (auto o_it : actived_individualOrganisms) {
            IndividualOrganism* individualOrganism = o_it.second;
            //if current year no smaller than individual organism's next run year, then move this organism.
            if (year >= individualOrganism->getNextRunYear()) {
                std::vector<CoodLocation*> next_cells;
                switch (individualOrganism->getDispersalMethod()) {
                //only the new individual organisms can move
                case 1:
                    ;
                    break;
                    //all the individual organisms can move
                case 2:
                    next_cells = getDispersalMap_2(individualOrganism,
                            current_environments);
                    break;
                default:
                    ;
                }
                for (auto it : next_cells) {
                    unsigned index = it->getY() * xSize + it->getX();
                    //create a new organism
                    index = it->getY() * xSize + it->getX();
                    IndividualOrganism* new_individualOrganism =
                            new IndividualOrganism(year,
                                    individualOrganism->getSpecies(),
                                    individualOrganism, it->getX(), it->getY());

                    actived_individualOrganisms[index] = new_individualOrganism;
                    all_individualOrganisms.push_back(new_individualOrganism);
                }
                individualOrganism->addNextRunYear();
                CommonFun::clearVector(next_cells);
            }
        }

        LOG(INFO)<<"end to simulate organism by organism. No. of organisms. " << actived_individualOrganisms.size();

        //LOG(INFO)<<"end to simulate cell by cell";

        //remove the unsuitable organisms
        LOG(INFO)<<"begin to remove unsuitable organisms";
        std::vector<unsigned> erased_key;

        for (auto it : actived_individualOrganisms) {
            if (!it.second->isSuitable(current_environments)){
                erased_key.push_back(it.first);
            }
        }

        for (auto key : erased_key) {
            actived_individualOrganisms.erase(key);
        }
        erased_key.clear();

        LOG(INFO)<<"end to remove unsuitable organisms";

        LOG(INFO)<<"begin to generate distribution maps";
        //generate the distribution map for every species
        boost::unordered_map<unsigned, SparseMap*> distribution_maps;
        for (auto io_it : actived_individualOrganisms) {
            if (distribution_maps.find(io_it.second->getSpeciesID())
                    == distribution_maps.end()) {
                distribution_maps[io_it.second->getSpeciesID()] = new SparseMap(
                        xSize, ySize);
                distribution_maps[io_it.second->getSpeciesID()]->setValue(
                        io_it.second->getX(), io_it.second->getY(), 1);
            } else {
                distribution_maps[io_it.second->getSpeciesID()]->setValue(
                        io_it.second->getX(), io_it.second->getY(),
                        distribution_maps[io_it.second->getSpeciesID()]->readByXY(
                                io_it.second->getX(), io_it.second->getY())
                                + 1);
            }

        }


        for (auto it : distribution_maps) {
            //save distribution
            char tiffName[target.length() + 28];
            sprintf(tiffName, "%s/%s/dispersal/%s.tif", target.c_str(),
                    CommonFun::fixedLength(it.first, 5).c_str(),
                    CommonFun::fixedLength(year, 7).c_str());
            int* array = it.second->toArray();
            RasterController::writeGeoTIFF(tiffName, xSize, ySize, geoTrans,
                    array, (double) NODATA, GDT_Int32);
            delete[] array;

            //get the groups

        }
        CommonFun::clearUnordered_map(distribution_maps);
        LOG(INFO)<<"end to generate distribution maps";

    }
    char filepath[target.length() + 15];
    sprintf(filepath, "%s/env_curve.csv", target.c_str());
    CommonFun::writeFile(env_output, filepath);
    env_output.clear();
}
std::vector<SparseMap*> Scenario::getEnvironmenMap(unsigned p_year) {
    std::vector<SparseMap*> result;
    for (unsigned i = 0; i < environments.size(); ++i) {
        result.push_back(environments[i]->getValues(p_year));
    }
    return result;
}
void Scenario::cleanSpecies() {
    CommonFun::clearVector(species);
}
void Scenario::cleanActivedIndividualOrganisms() {
    CommonFun::clearUnordered_map(actived_individualOrganisms);
}
void Scenario::cleanEnvironments() {
    CommonFun::clearVector(environments);
    for (hashmap_multiply::iterator it = environment_maps.begin();
            it != environment_maps.end();) {
        CommonFun::clearVector(it->second);
        it = environment_maps.erase(it);
    }
    environment_maps.clear();
}
std::vector<CoodLocation*> Scenario::getDispersalMap_2(
        IndividualOrganism* individualOrganism,
        std::vector<SparseMap*> p_current_environments) {
    std::vector<CoodLocation*> new_cells;

    //unfinished part

    //get all the cells whose E-distances are not longer than dispersal ability.
    //When number of path = 1, ignore the dispersal method parameter.
    if (individualOrganism->getNumOfPath() == -1) {
        unsigned x = individualOrganism->getX();
        unsigned y = individualOrganism->getY();
        for (unsigned i_x = (x - individualOrganism->getDispersalAbility());
                i_x <= (x + individualOrganism->getDispersalAbility()); ++i_x) {
            i_x = (((int) i_x) < 0) ? 0 : i_x;
            if ((unsigned) i_x >= xSize)
                break;
            for (unsigned i_y = (y - individualOrganism->getDispersalAbility());
                    i_y <= (y + individualOrganism->getDispersalAbility());
                    ++i_y) {
                i_y = (((int) i_y) < 0) ? 0 : i_y;
                if ((unsigned) i_y >= ySize)
                    break;

                double distance = CommonFun::EuclideanDistance((int) i_x,
                        (int) i_y, (int) (x), (int) (y));
//                printf("%u, %u vs %u, %u, Distance:%f\n", i_x, i_y, x, y,
//                        distance);
                if ((distance < individualOrganism->getDispersalAbility())
                        || (CommonFun::AlmostEqualRelative(distance,
                                (double) individualOrganism->getDispersalAbility()))) {
                    CoodLocation* v = new CoodLocation(i_x, i_y);
                    new_cells.push_back(v);
                }
            }
        }
    }
    return new_cells;
}

Scenario::~Scenario() {
    delete[] geoTrans;
    delete mask;
    cleanEnvironments();
    cleanActivedIndividualOrganisms();
    cleanSpecies();
}

