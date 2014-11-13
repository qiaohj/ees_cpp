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
        createSpeciesFolder(species);
        std::vector<GeoLocation*> seeds = species->getSeeds();
        boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > t_o;
        for (unsigned i = 0; i < seeds.size(); ++i) {

            unsigned x, y;
            CommonFun::LL2XY(geoTrans, seeds[i]->getLongitude(),
                    seeds[i]->getLatitude(), &x, &y);
            IndividualOrganism* individualOrganism = new IndividualOrganism(0,
                    species, NULL, x, y);
            boost::unordered_map<unsigned, IndividualOrganism*> t;
            t[y * xSize + x] = individualOrganism;
            actived_individualOrganisms[species] = t;
            t_o[y * xSize + x].push_back(individualOrganism);

        }
        all_individualOrganisms[0][species] = t_o;
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
char* Scenario::getSpeciesFolder(SpeciesObject* p_species) {
    if (p_species->getParent() == NULL) {
        char* speciesFolder[target.length() + 6];
        sprintf(*speciesFolder, "%s/%s", target.c_str(),
                CommonFun::fixedLength(p_species->getID(), 5).c_str());
        CommonFun::createFolder(*speciesFolder);
        return *speciesFolder;
    } else {
        char* parentFolder = getSpeciesFolder(p_species->getParent());
        char* speciesFolder[strlen(parentFolder) + 6];
        sprintf(*speciesFolder, "%s/%s", parentFolder,
                CommonFun::fixedLength(p_species->getID(), 5).c_str());
        CommonFun::createFolder (*speciesFolder);
        return *speciesFolder;
    }
}
void Scenario::createSpeciesFolder(SpeciesObject* p_species) {
    char* speciesFolder = getSpeciesFolder(p_species);

    char dispersalFolder[target.length() + 6 + 10];
    sprintf(dispersalFolder, "%s/dispersal", speciesFolder);
    CommonFun::createFolder(dispersalFolder);

    char groupsmapFolder[target.length() + 6 + 10];
    sprintf(groupsmapFolder, "%s/groupsmap", speciesFolder);
    CommonFun::createFolder(groupsmapFolder);
}
void Scenario::run() {
    std::vector<std::string> env_output;
    unsigned x = 99999, y = 99999;
//    bool is_write_memory_usage = false;
    for (unsigned year = minSpeciesDispersalSpeed; year <= totalYears; year +=
            minSpeciesDispersalSpeed) {
//        if (year>=50000){
//            is_write_memory_usage = true;
//        }
        LOG(INFO)<<"Current year:"<<year<<" Memory usage:"<<CommonFun::getCurrentRSS();

        boost::unordered_map<SpeciesObject*, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > > individual_organisms_in_current_year;
        std::vector<SparseMap*> current_environments = getEnvironmenMap(year);

        //save the env data
        if (x == 99999) {
            int value;
            current_environments[0]->getFirstValues(&x, &y, &value);
        }
        char line[30];
        int v = current_environments[0]->readByXY(x, y);
        sprintf(line, "%u,%u,%u,%d", year, x, y, v);
        env_output.push_back(line);

//        LOG(INFO)<<"start to simulate organism by species. Count of species is " << actived_individualOrganisms.size() << ". Count of all organisms/years is " << all_individualOrganisms.size();
        for (auto s_it : actived_individualOrganisms) {
//            LOG(INFO)<<"start to simulate organism by organism. Current species is "<< s_it.first << ". Count of organisms is " << (*s_it.second).size();
            std::vector<IndividualOrganism*> new_individual_organisms;
            for (auto o_it : s_it.second) {
                IndividualOrganism* individualOrganism = o_it.second;
                //if current year no smaller than individual organism's next run year, then move this organism.
//                LOG(INFO)<<"Organism index is "<< individualOrganism->getX()<<","<<individualOrganism->getY()<<". Current year is "<<year<<". Next year is "<<individualOrganism->getNextRunYear();
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
//                        LOG(INFO)<<"Put "<<new_individualOrganism->getX()<<", "<<new_individualOrganism->getY()<<" into the new organism group.";
                        new_individual_organisms.push_back(new_individualOrganism);
                    }
                    for (std::vector<CoodLocation*>::iterator it =
                            next_cells.begin(); it != next_cells.end(); ++it) {
                        delete *it;
                    }
                    next_cells.clear();
                    std::vector<CoodLocation*>().swap(next_cells);
                } else {
//                    LOG(INFO) << "Didn't run, for current year is "<<year<< " and organism run year is " << individualOrganism->getNextRunYear();
                }
            }

            for (auto it : new_individual_organisms) {
                unsigned index = it->getY() * xSize + it->getX();
                //species id, index
                individual_organisms_in_current_year[s_it.first][index].push_back(it);
            }
            new_individual_organisms.clear();
//            LOG(INFO)<<"end to simulate organism by organism. Current species is "<< s_it.first << ". Count of organisms is " << (*s_it.second).size();
        }
        for (auto sp_it : individual_organisms_in_current_year) {
            for (auto o_it : sp_it.second) {
                for (auto o_it2 : o_it.second) {
                    unsigned index = xSize * o_it2->getY() + o_it2->getX();
                    actived_individualOrganisms[sp_it.first][index] = o_it2;
                }
            }
        }
//        LOG(INFO)<<"end to simulate organism by species. Count of species is " << actived_individualOrganisms.size() << ". Count of all organisms is " << all_individualOrganisms.size();
        //LOG(INFO)<<"end to simulate cell by cell";

        //remove the unsuitable organisms
        unsigned cc = 0;
        for (auto sp_it : actived_individualOrganisms){
            cc += sp_it.second.size();
        }
        LOG(INFO)<<"begin to remove the unsuitable organisms. Total actived organisms is "<<cc;
        boost::unordered_map<SpeciesObject*, std::vector<unsigned>> erased_keys;
        for (auto s_it : actived_individualOrganisms) {
//            LOG(INFO)<<"start to remove unsuitable organisms. Current species is "<< s_it.first << ". Count of organisms is " << (*s_it.second).size();
            std::vector<unsigned> erased_key;
            for (auto it : s_it.second) {
                if (!it.second->isSuitable(current_environments)) {
                    erased_key.push_back(it.first);
                }
            }
            erased_keys[s_it.first] = erased_key;
//            LOG(INFO)<<"end to remove unsuitable organisms. Current species is "<< s_it.first << ". Count of organisms is " << (*s_it.second).size();
        }
        for (auto sp_it : erased_keys) {
            for (auto key : sp_it.second){
                actived_individualOrganisms[sp_it.first].erase(key);
            }
        }

        cc = 0;
        for (auto sp_it : actived_individualOrganisms) {
            cc += sp_it.second.size();
        }
        LOG(INFO)<<"end to remove unsuitable organisms. Total actived organisms is "<<cc;

        //mark the group id for every organisms in this year, seperated by species id;
        LOG(INFO)<<"Begin to mark the group id, and detect the speciation.";
        for (auto sp_it : individual_organisms_in_current_year) {
            boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > organisms = sp_it.second;
            SpeciesObject* species = sp_it.first;
            unsigned short current_group_id = 1;
            if (year>=(burnInYear + species->getSpeciationYears())){
                IndividualOrganism* unmarked_organism = getUnmarkedOrganism(organisms);
                while (unmarked_organism!=NULL) {
                    markJointOrganism(current_group_id, unmarked_organism, organisms);
                    current_group_id++;
                    unmarked_organism = getUnmarkedOrganism(organisms);
                }
                if (current_group_id>2) {
                    LOG(INFO)<<"Group number in year "<< year <<" is "<< current_group_id-1;
                }
                //detect the speciation
                unsigned short temp_species_id = 1;
                for (unsigned short group_id_1=0; group_id_1<current_group_id-2; group_id_1++){
                    unsigned short temp_species_id_1 = getTempSpeciesID(group_id_1, organisms);
                    for (unsigned short group_id_2=group_id_1+1; group_id_2<current_group_id-1; group_id_2++){
                        unsigned short temp_species_id_2 = getTempSpeciesID(group_id_2, organisms);
                        //if both groups were marked, and they have the same id, skip it.
                        if ((temp_species_id_1!=0)&&(temp_species_id_2!=0)&&(temp_species_id_1==temp_species_id_2)){
                            continue;
                        }
                        unsigned min_divided_year = getMinDividedYear(group_id_1, group_id_2, organisms);
                        if (min_divided_year>=species->getSpeciationYears()){
                            //if a speciation happened, marked them with two ids if they were not marked.
                            if (temp_species_id_1==0){
                                markedSpeciesID(group_id_1, temp_species_id, organisms);
                                temp_species_id++;
                            }
                            if (temp_species_id_2==0){
                                markedSpeciesID(group_id_2, temp_species_id, organisms);
                                temp_species_id++;
                            }
                        }else{
                            //if there is not speciation, marked them with the same id
                            unsigned short t_id = (temp_species_id_1==0)?temp_species_id_2:temp_species_id_1;
                            t_id = (t_id==0)?temp_species_id:t_id;
                            markedSpeciesID(group_id_1, t_id, organisms);
                            markedSpeciesID(group_id_2, t_id, organisms);

                        }
                    }
                }


            }else{
                for (auto y_it : sp_it.second){
                    for (auto o_it : y_it.second){
                        o_it->setGroupId(current_group_id);
                    }
                }
            }
        }
        LOG(INFO)<<"end to mark the group id, and detect the speciation.";

        boost::unordered_map<SpeciesObject*, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > > new_individual_organisms_in_current_year;
        boost::unordered_map<SpeciesObject*,
                                boost::unordered_map<unsigned, IndividualOrganism*> > new_actived_individualOrganisms;
        for (auto sp_it : individual_organisms_in_current_year){
            boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > organisms = sp_it.second;
            //count all the species
            boost::unordered_map<unsigned short, unsigned short> species_ids;
            for (auto c_it : sp_it.second){
                if (c_it.second.size()>0){
                    species_ids[c_it.second.front()->getTempSpeciesId()] = species_ids.size();
                }
            }

            if (species_ids.size()>1){
                for (auto sp_id_it : species_ids){
                    SpeciesObject* new_species = new SpeciesObject(sp_id_it.second, sp_it.first);
                    createSpeciesFolder(new_species);
                    species.push_back(new_species);
                    for (auto c_it : sp_it.second){
                        for (auto o_it : c_it.second){
                            if (o_it->getTempSpeciesId()==sp_id_it.first){
                                new_individual_organisms_in_current_year[new_species][c_it.first].push_back(o_it);
                                o_it->setSpecies(new_species);
                            }
                        }
                        new_actived_individualOrganisms[new_species][c_it.first] = actived_individualOrganisms[sp_it.first][c_it.first];
                    }

                }
            }else{
                new_individual_organisms_in_current_year[sp_it.first] = sp_it.second;
                new_actived_individualOrganisms[sp_it.first] = actived_individualOrganisms[sp_it.first];
            }
        }
        individual_organisms_in_current_year = new_individual_organisms_in_current_year;
        actived_individualOrganisms = new_actived_individualOrganisms;

        LOG(INFO)<<"begin to generate group maps";
        boost::unordered_map<SpeciesObject*, SparseMap*> group_maps;
        for (auto sp_it : individual_organisms_in_current_year) {
            if (group_maps.find(sp_it.first)==group_maps.end()) {
                group_maps[sp_it.first] = new SparseMap(xSize, ySize);
            }
            for (auto o_id : sp_it.second) {
                if (o_id.second.size()>0) {
                    group_maps[sp_it.first]->setValue(o_id.second[0]->getX(), o_id.second[0]->getY(), o_id.second[0]->getGroupId());
                }
            }
        }
        for (auto it : group_maps) {
            //save distribution
            char* speciesFolder = getSpeciesFolder(it.first);
            char tiffName[strlen(speciesFolder) + 28];
            sprintf(tiffName, "%s/groupsmap/%s.tif", speciesFolder,
                    CommonFun::fixedLength(year, 7).c_str());
            int* array = it.second->toArray();
            RasterController::writeGeoTIFF(tiffName, xSize, ySize, geoTrans,
                    array, (double) NODATA, GDT_Int32);
            delete[] array;

        }

        //clear group_maps;
        std::vector<SpeciesObject*> erased_key;
        for (auto it : group_maps) {
            erased_key.push_back(it.first);
        }
        for (auto key : erased_key) {
            if (group_maps[key] != NULL) {
                delete group_maps[key];
            }
            group_maps.erase(key);
        }
        group_maps.clear();
        erased_key.clear();

        all_individualOrganisms[year] = individual_organisms_in_current_year;
        //remove the useless organism
        for (auto sp_it : species) {
            if (year<sp_it->getDispersalSpeed()) {
                continue;
            }
            unsigned speciation_year = sp_it->getSpeciationYears();
            bool is_remove_previous_span = false;
            bool is_remove_previous_speciation = false;
            if (year<(burnInYear - speciation_year)) {
                is_remove_previous_span = true;
            }
            if (year>burnInYear) {
                is_remove_previous_speciation = true;
            }
            int removed_year = -1;
            if (is_remove_previous_span) {
                removed_year = year - sp_it->getDispersalSpeed();
            }
            if (is_remove_previous_speciation) {
                removed_year = (year - sp_it->getDispersalSpeed()) - speciation_year;
            }
            if (removed_year>=0) {
                LOG(INFO)<<"Remove the organisms in year " <<removed_year<<". Before removing, Memory usage:"<<CommonFun::getCurrentRSS();

                boost::unordered_map<SpeciesObject*, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > > temp_o = all_individualOrganisms[removed_year];
                for (auto it1 : temp_o) {
                    for (auto it2 : it1.second) {
                        CommonFun::clearVector(&it2.second);
                    }
                }
                all_individualOrganisms.erase(removed_year);
                LOG(INFO)<<"Remove the organisms in year " <<removed_year<<". After  removing, Memory usage:"<<CommonFun::getCurrentRSS();
            }
        }
        unsigned c = 0;
        for (auto y_it : all_individualOrganisms) {
            for (auto s_it : y_it.second) {
                for (auto l_it : s_it.second) {
                    c += l_it.second.size();
                }
            }

        }
        CommonFun::clearVector(&current_environments);
        LOG(INFO)<<"Total organisms are " <<c;
    }
    char filepath[target.length() + 15];
    sprintf(filepath, "%s/env_curve.csv", target.c_str());
    CommonFun::writeFile(env_output, filepath);
    env_output.clear();
}
unsigned short Scenario::getTempSpeciesID(unsigned short group_id, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > organisms){
    for (auto c_it : organisms){
        for (auto o_it : c_it.second){
            return o_it->getTempSpeciesId();
        }
    }
    return 0;
}
void Scenario::markedSpeciesID(unsigned short group_id, unsigned short temp_species_id, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > organisms){
    for (auto c_it : organisms){
        for (auto o_it : c_it.second){
            if (o_it->getGroupId()==group_id){
                o_it->setTempSpeciesId(temp_species_id);
            }
        }
    }
}
unsigned Scenario::getMinDividedYear(unsigned short group_id_1, unsigned short group_id_2, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > organisms){
    unsigned nearest_divided_year = 0;
    unsigned current_year = 0;
    std::vector<IndividualOrganism*> group_1;
    std::vector<IndividualOrganism*> group_2;
    for (auto c_it : organisms){
        for (auto o_it : c_it.second){
            current_year = o_it->getYear();
            if (o_it->getGroupId()==group_id_1){
                group_1.push_back(o_it);
            }else if (o_it->getGroupId()==group_id_2){
                group_2.push_back(o_it);
            }
        }
    }
    for (auto o_it_1 : group_1){
        for (auto o_it_2 :  group_2){
            unsigned divided_year = getDividedYear(o_it_1, o_it_2);
            nearest_divided_year = (divided_year>nearest_divided_year)?divided_year:nearest_divided_year;
        }
    }
    return current_year - nearest_divided_year;
}
unsigned Scenario::getDividedYear(IndividualOrganism* o_1, IndividualOrganism* o_2){
    IndividualOrganism* parent_1 = o_1->getParent();
    IndividualOrganism* parent_2 = o_2->getParent();
    if ((parent_1==NULL)||(parent_2==NULL)){
        return 0;
    }
    if (parent_1==parent_2){
        return parent_1->getYear();
    }else{
        return getDividedYear(parent_1, parent_2);
    }
}
void Scenario::markJointOrganism(unsigned short p_group_id,
        IndividualOrganism* p_unmarked_organism,
        boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > organisms) {
    unsigned short x = p_unmarked_organism->getX();
    unsigned short y = p_unmarked_organism->getY();
    for (unsigned i_x = (x - p_unmarked_organism->getDispersalAbility());
            i_x <= (x + p_unmarked_organism->getDispersalAbility()); ++i_x) {
        i_x = (((int) i_x) < 0) ? 0 : i_x;
        if ((unsigned) i_x >= xSize)
            break;
        for (unsigned i_y = (y - p_unmarked_organism->getDispersalAbility());
                i_y <= (y + p_unmarked_organism->getDispersalAbility());
                ++i_y) {
            i_y = (((int) i_y) < 0) ? 0 : i_y;
            if ((unsigned) i_y >= ySize)
                break;

            double distance = CommonFun::EuclideanDistance((int) i_x, (int) i_y,
                    (int) (x), (int) (y));
            //                printf("%u, %u vs %u, %u, Distance:%f\n", i_x, i_y, x, y,
            //                        distance);
            if ((distance < p_unmarked_organism->getDispersalAbility())
                    || (CommonFun::AlmostEqualRelative(distance,
                            (double) p_unmarked_organism->getDispersalAbility()))) {
                for (auto it : organisms[i_y * xSize + i_x]) {
                    if (it->getGroupId() == 0) {
                        it->setGroupId(p_group_id);
                        if ((x != i_x) || (y != i_y)) {
                            markJointOrganism(p_group_id, it, organisms);
                        }
                    }
                }

            }
        }
    }
}

IndividualOrganism* Scenario::getUnmarkedOrganism(
        boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > organisms) {
    for (auto p_it : organisms) {
        for (auto it : p_it.second) {
            if (it->getGroupId() == 0) {
                return it;
            }
        }
    }
    return NULL;
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

    for (auto y_it : all_individualOrganisms) {
        for (auto s_it : y_it.second) {
            for (auto l_it : s_it.second) {
                CommonFun::clearVector(&l_it.second);
            }
        }
    }
//    CommonFun::clearUnordered_map(&all_individualOrganisms);
//    CommonFun::clearUnordered_map(&actived_individualOrganisms);
}
void Scenario::cleanEnvironments() {
//    CommonFun::clearVector(&environments);
//    for (hashmap_multiply::iterator it = environment_maps.begin();
//            it != environment_maps.end();) {
//        CommonFun::clearVector(&it->second);
//        it = environment_maps.erase(it);
//    }
//    environment_maps.clear();
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

