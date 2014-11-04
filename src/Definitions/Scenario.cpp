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
        std::vector<IndividualOrganism*>* t_o = new std::vector<IndividualOrganism*>();
        for (unsigned i = 0; i < seeds.size(); ++i) {

            unsigned x, y;
            CommonFun::LL2XY(geoTrans, seeds[i]->getLongitude(),
                    seeds[i]->getLatitude(), &x, &y);
            IndividualOrganism* individualOrganism = new IndividualOrganism(0,
                    species, NULL, x, y);
            boost::unordered_map<unsigned, IndividualOrganism*>* t = new boost::unordered_map<unsigned, IndividualOrganism*>();
            t->insert(std::make_pair(y * xSize + x, individualOrganism));
            actived_individualOrganisms[species->getID()] = t;
            t_o->push_back(individualOrganism);

        }
        all_individualOrganisms[0] = t_o;
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
        burnInYear = environment_item->getBurnInYears();
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
    for (unsigned year = minSpeciesDispersalSpeed; year <= totalYears; year +=
            minSpeciesDispersalSpeed) {

        printf("Current year:%d, Memory usage:%lu\n", year, CommonFun::getCurrentRSS());

        std::vector<IndividualOrganism*>* individual_organisms_in_this_year = new std::vector<IndividualOrganism*>();
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
        LOG(INFO)<<"start to simulate organism by species. Count of species is " << actived_individualOrganisms.size() << ". Count of all organisms/years is " << all_individualOrganisms.size();
        for (auto s_it : actived_individualOrganisms) {
            LOG(INFO)<<"start to simulate organism by organism. Current species is "<< s_it.first << ". Count of organisms is " << (*s_it.second).size();
            std::vector<IndividualOrganism*> new_individual_organisms;
            for (auto o_it : *s_it.second) {
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

                        //create a new organism
                        IndividualOrganism* new_individualOrganism =
                        new IndividualOrganism(year,
                                individualOrganism->getSpecies(),
                                individualOrganism, it->getX(), it->getY());

                        new_individual_organisms.push_back(new_individualOrganism);
                        individual_organisms_in_this_year->push_back(new_individualOrganism);

                    }
                    CommonFun::clearVector(&next_cells);
                } else {
                    LOG(INFO) << "Didn't run, for current year is "<<year<< " and organism run year is " << individualOrganism->getNextRunYear();
                }
            }
            for (auto it : new_individual_organisms) {
                unsigned index = it->getY() * xSize + it->getX();
                (*s_it.second)[index] = it;
            }
            new_individual_organisms.clear();
            LOG(INFO)<<"end to simulate organism by organism. Current species is "<< s_it.first << ". Count of organisms is " << (*s_it.second).size();
        }

        LOG(INFO)<<"end to simulate organism by species. Count of species is " << actived_individualOrganisms.size() << ". Count of all organisms is " << all_individualOrganisms.size();
        //LOG(INFO)<<"end to simulate cell by cell";

        //remove the unsuitable organisms
        for (auto s_it : actived_individualOrganisms) {
            LOG(INFO)<<"start to remove unsuitable organisms. Current species is "<< s_it.first << ". Count of organisms is " << (*s_it.second).size();
            std::vector<unsigned> erased_key;
            for (auto it : *s_it.second) {
                if (!it.second->isSuitable(current_environments)) {
                    erased_key.push_back(it.first);
                }
            }
            for (auto key : erased_key) {
                (*s_it.second).erase(key);
            }
            erased_key.clear();
            LOG(INFO)<<"end to remove unsuitable organisms. Current species is "<< s_it.first << ". Count of organisms is " << (*s_it.second).size();
        }

        LOG(INFO)<<"end to remove unsuitable organisms";

        LOG(INFO)<<"begin to generate distribution maps";
        //generate the distribution map for every species
        boost::unordered_map<unsigned, SparseMap*> distribution_maps;
        for (auto s_it : actived_individualOrganisms) {
            for (auto io_it : *s_it.second) {
                if (distribution_maps.find(io_it.second->getSpeciesID())
                        == distribution_maps.end()) {
                    distribution_maps[io_it.second->getSpeciesID()] =
                            new SparseMap(xSize, ySize);
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
        CommonFun::clearUnordered_map(&distribution_maps);
        LOG(INFO)<<"end to generate distribution maps";
        all_individualOrganisms[year] = individual_organisms_in_this_year;
        //remove the useless organism
        for (auto sp_it : species){
            if (year<sp_it->getDispersalSpeed()){
                continue;
            }
            unsigned speciation_year = sp_it->getSpeciationYears();
            bool is_remove_previous_span = false;
            bool is_remove_previous_speciation = false;
            if (year<(burnInYear - speciation_year)){
                is_remove_previous_span = true;
            }
            if (year>burnInYear){
                is_remove_previous_speciation = true;
            }
            int removed_year = -1;
            if (is_remove_previous_span){
                removed_year = year - sp_it->getDispersalSpeed();
            }
            if (is_remove_previous_speciation){
                removed_year = (year - sp_it->getDispersalSpeed()) - speciation_year;
            }
            if (removed_year>50000){
                printf("Burn in year:%u, %u - %u - %u = %u\n", burnInYear, year, sp_it->getDispersalSpeed(), speciation_year, removed_year);
            }
            if (removed_year>=0){
                LOG(INFO)<<"Remove the organisms in year " <<removed_year<<". Before removing, Memory usage:"<<CommonFun::getCurrentRSS();
                CommonFun::clearVector(all_individualOrganisms[removed_year]);
                delete all_individualOrganisms[removed_year];
                all_individualOrganisms.erase(removed_year);
                LOG(INFO)<<"Remove the organisms in year " <<removed_year<<". After  removing, Memory usage:"<<CommonFun::getCurrentRSS();
            }
        }
        unsigned c = 0;
        for (auto y_it : all_individualOrganisms){
            c += y_it.second->size();
        }
        LOG(INFO)<<"Total organisms are " <<c;
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
    CommonFun::clearVector(&species);
}
void Scenario::cleanActivedIndividualOrganisms() {
//    CommonFun::clearUnordered_map(all_individualOrganisms);

    for (auto it : all_individualOrganisms){
        CommonFun::clearVector(it.second);
    }
    CommonFun::clearUnordered_map(&all_individualOrganisms);
    CommonFun::clearUnordered_map(&actived_individualOrganisms);
}
void Scenario::cleanEnvironments() {
    CommonFun::clearVector(&environments);
    for (hashmap_multiply::iterator it = environment_maps.begin();
            it != environment_maps.end();) {
        CommonFun::clearVector(&it->second);
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

