/*
 * Scenario.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: qiaohj
 */

#include "Scenario.h"

Scenario::Scenario(Json::Value p_root, std::string p_base_folder,
        std::string p_target, unsigned p_tif_limit) {
    tifLimit = p_tif_limit;
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
std::string Scenario::getSpeciesFolder(SpeciesObject* p_species) {
    if (p_species->getParent() == NULL) {
        char speciesFolder[target.length() + 6];
        sprintf(speciesFolder, "%s/%s", target.c_str(),
                CommonFun::fixedLength(p_species->getID(), 2).c_str());
        CommonFun::createFolder(speciesFolder);
        return std::string(speciesFolder);
    } else {
        std::string parentFolder = getSpeciesFolder(p_species->getParent());
        char speciesFolder[parentFolder.length() + 6];
        sprintf(speciesFolder, "%s/%s", parentFolder.c_str(),
                CommonFun::fixedLength(p_species->getID(), 2).c_str());
        CommonFun::createFolder(speciesFolder);
        return std::string(speciesFolder);
    }
}
void Scenario::createSpeciesFolder(SpeciesObject* p_species) {
    std::string speciesFolder = getSpeciesFolder(p_species);

//    char dispersalFolder[speciesFolder.length() + 6 + 10];
//    sprintf(dispersalFolder, "%s/dispersal", speciesFolder.c_str());
//    CommonFun::createFolder(dispersalFolder);

    char groupsmapFolder[speciesFolder.length() + 6 + 10];
    sprintf(groupsmapFolder, "%s/groupsmap", speciesFolder.c_str());
    CommonFun::createFolder(groupsmapFolder);
    char groupslogFolder[speciesFolder.length() + 6 + 10];
    sprintf(groupslogFolder, "%s/group_log", speciesFolder.c_str());
    CommonFun::createFolder(groupslogFolder);
}
void Scenario::run() {
    std::vector<std::string> env_output;
    unsigned x = 99999, y = 99999;
    unsigned tif_number = 0;

//    bool is_write_memory_usage = false;
    for (unsigned year = minSpeciesDispersalSpeed; year <= totalYears; year +=
            minSpeciesDispersalSpeed) {
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
        boost::unordered_map<SpeciesObject*,
        boost::unordered_map<unsigned, IndividualOrganism*> > actived_individualOrganisms;
        for (auto sp_it : all_individualOrganisms[year - minSpeciesDispersalSpeed]) {
            for (auto c_it : sp_it.second) {
                if (c_it.second.size()>0) {
                    actived_individualOrganisms[sp_it.first][c_it.first] = c_it.second.front();
                }
            }
        }
        LOG(INFO)<<"start to simulate organism by species. Count of species is " << actived_individualOrganisms.size();
        for (auto s_it : actived_individualOrganisms) {
            //LOG(INFO)<<"start to simulate organism by organism. Current species is "<< s_it.first << ". Count of organisms is " << s_it.second.size();
            std::vector<IndividualOrganism*> new_individual_organisms;
            for (auto o_it : s_it.second) {
                IndividualOrganism* individualOrganism = o_it.second;
                //if current year no smaller than individual organism's next run year, then move this organism.
                //LOG(INFO)<<"Organism index is "<< individualOrganism->getX()<<","<<individualOrganism->getY()<<". Current year is "<<year<<". Next year is "<<individualOrganism->getNextRunYear();
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
                    }
                    for (std::vector<CoodLocation*>::iterator it =
                            next_cells.begin(); it != next_cells.end(); ++it) {
                        delete *it;
                    }
                    next_cells.clear();
                    std::vector<CoodLocation*>().swap(next_cells);
                } else {
                    LOG(INFO) << "Didn't run, for current year is "<<year<< " and organism run year is " << individualOrganism->getNextRunYear();
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

//        LOG(INFO)<<"end to simulate organism by species. Count of species is " << actived_individualOrganisms.size() << ". Count of all organisms is " << all_individualOrganisms.size();
        //LOG(INFO)<<"end to simulate cell by cell";

        //remove the unsuitable organisms

        LOG(INFO)<<"begin to remove the unsuitable organisms.";
        boost::unordered_map<SpeciesObject*, std::vector<unsigned>> erased_keys;
        for (auto s_it : individual_organisms_in_current_year) {
//            LOG(INFO)<<"start to remove unsuitable organisms. Current species is "<< s_it.first << ". Count of organisms is " << (*s_it.second).size();
            std::vector<unsigned> erased_key;
            for (auto it : s_it.second) {
                if (it.second.size()>0) {
                    if (!it.second.front()->isSuitable(current_environments)) {
                        erased_key.push_back(it.first);
                    }
                }
            }
            erased_keys[s_it.first] = erased_key;
//            LOG(INFO)<<"end to remove unsuitable organisms. Current species is "<< s_it.first << ". Count of organisms is " << (*s_it.second).size();
        }
        for (auto sp_it : erased_keys) {
            for (auto key : sp_it.second) {
                for (std::vector<IndividualOrganism*>::iterator it = individual_organisms_in_current_year[sp_it.first][key].begin();
                        it != individual_organisms_in_current_year[sp_it.first][key].end(); ++it) {
                    (*it)->getParent()->removeChild(*it);
                    delete *it;
                }
                individual_organisms_in_current_year[sp_it.first][key].clear();
                std::vector<IndividualOrganism*>().swap(individual_organisms_in_current_year[sp_it.first][key]);
                individual_organisms_in_current_year[sp_it.first].erase(key);
            }
        }

        LOG(INFO)<<"end to remove unsuitable organisms.";

        //mark the group id for every organisms in this year, seperated by species id;
        LOG(INFO)<<"Begin to mark the group id, and detect the speciation.";
        for (auto sp_it : individual_organisms_in_current_year) {
            //printf("Species ID:%u\n", sp_it.first->getID());
            boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > organisms = sp_it.second;
            SpeciesObject* species = sp_it.first;
            unsigned short current_group_id = 1;
            if (year>=(burnInYear + species->getSpeciationYears())) {
                IndividualOrganism* unmarked_organism = getUnmarkedOrganism(organisms);
                while (unmarked_organism!=NULL) {
                    markJointOrganism(current_group_id, unmarked_organism, organisms);
                    current_group_id++;
                    unmarked_organism = getUnmarkedOrganism(organisms);
                }
                //detect the speciation
                unsigned short temp_species_id = 1;
                std::vector<std::string> group_output;
                for (unsigned short group_id_1=1; group_id_1<current_group_id-1; group_id_1++) {
                    unsigned short temp_species_id_1 = getTempSpeciesID(group_id_1, organisms);
                    for (unsigned short group_id_2=group_id_1+1; group_id_2<current_group_id; group_id_2++) {
                        unsigned short temp_species_id_2 = getTempSpeciesID(group_id_2, organisms);
                        //printf("%u,%u,%u,%u,%u,%u,%s\n", year, group_id_1, group_id_2, temp_species_id_1, temp_species_id_2, 0, "info");
                        //if both groups were marked, and they have the same id, skip it.
                        if ((temp_species_id_1!=0)&&(temp_species_id_2!=0)&&(temp_species_id_1==temp_species_id_2)) {
                            char line[100];
                            sprintf(line, "%u,%u,%u,%u,%u,%u,%s", year, group_id_1, group_id_2, temp_species_id_1, temp_species_id_2, 0, "skip");
                            //printf("%u,%u,%u,%u,%u,%u,%s\n", year, group_id_1, group_id_2, temp_species_id_1, temp_species_id_2, 0, "skip");
                            group_output.push_back(line);
                            continue;
                        }

                        unsigned min_divided_year = getMinDividedYear(group_id_1, group_id_2, organisms);
                        if (min_divided_year>=species->getSpeciationYears()) {
                            //if a speciation happened, marked them with two ids if they were not marked.
                            if (temp_species_id_1==0) {
                                char line[100];
                                sprintf(line, "%u,%u,%u,%u,%u,%u,%s", year, group_id_1, group_id_2, temp_species_id, 0, min_divided_year, "new species, mark group 1");
                                //printf("%u,%u,%u,%u,%u,%u,%s\n", year, group_id_1, group_id_2, temp_species_id, 0, min_divided_year, "new species, mark group 1");
                                group_output.push_back(line);
                                markedSpeciesID(group_id_1, temp_species_id, organisms);
                                temp_species_id_1 = temp_species_id;
                                temp_species_id++;
                            }
                            if (temp_species_id_2==0) {
                                char line[100];
                                sprintf(line, "%u,%u,%u,%u,%u,%u,%s", year, group_id_1, group_id_2, temp_species_id, 0, min_divided_year, "new species, mark group 2");
                                //printf("%u,%u,%u,%u,%u,%u,%s\n", year, group_id_1, group_id_2, temp_species_id, 0, min_divided_year, "new species, mark group 2");
                                group_output.push_back(line);
                                markedSpeciesID(group_id_2, temp_species_id, organisms);
                                temp_species_id_2 = temp_species_id;
                                temp_species_id++;
                            }
                        } else {
                            //if there is not speciation, marked them with the same id
                            unsigned short t_id = (temp_species_id_1==0)?temp_species_id_2:temp_species_id_1;
                            t_id = (t_id==0)?temp_species_id:t_id;
                            markedSpeciesID(group_id_1, t_id, organisms);
                            temp_species_id_1 = t_id;
//                            char line1[100];
//                            sprintf(line1, "%u,%u,%u,%u,%u,%u,%s", year, group_id_1, group_id_2, t_id, 0, min_divided_year, "same species, mark group 1");
//                            //printf("%u,%u,%u,%u,%u,%u,%s\n", year, group_id_1, group_id_2, t_id, 0, min_divided_year, "same species, mark group 1");
//                            group_output.push_back(line1);

                            markedSpeciesID(group_id_2, t_id, organisms);
                            temp_species_id_2 = t_id;
//                            char line2[100];
//                            sprintf(line2, "%u,%u,%u,%u,%u,%u,%s", year, group_id_1, group_id_2, t_id, 0, min_divided_year, "same species, mark group 2");
                            //printf("%u,%u,%u,%u,%u,%u,%s\n", year, group_id_1, group_id_2, t_id, 0, min_divided_year, "same species, mark group 2");
//                            group_output.push_back(line2);
                        }
                    }
                }
                if (group_output.size()>0) {
                    char filepath[target.length() + 50];
                    sprintf(filepath, "%s/group_log/%u_group.txt", getSpeciesFolder(sp_it.first).c_str(), year);
                    CommonFun::writeFile(group_output, filepath);
                    group_output.clear();
                }
            } else {
                for (auto y_it : sp_it.second) {
                    for (auto o_it : y_it.second) {
                        o_it->setGroupId(current_group_id);
                    }
                }
            }
        }
        LOG(INFO)<<"end to mark the group id, and detect the speciation.";

        LOG(INFO)<<"Begin to rebuild the organism structure in this year";
        boost::unordered_map<SpeciesObject*, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > > new_individual_organisms_in_current_year;
        for (auto sp_it : individual_organisms_in_current_year) {
            boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > organisms = sp_it.second;
            //count all the species
            boost::unordered_map<unsigned short, unsigned short> species_ids;
            boost::unordered_set<unsigned short> temp_species_ids;
            for (auto c_it : sp_it.second) {
                if (c_it.second.size()>0) {
                    IndividualOrganism* organism_item = c_it.second.front();
                    temp_species_ids.insert(organism_item->getTempSpeciesId());
                }
            }
            unsigned short i = 1;
            for (auto it : temp_species_ids) {
                species_ids[it] = i++;
            }

            if (species_ids.size()>1) {
                for (auto sp_id_it : species_ids) {
                    SpeciesObject* new_species = new SpeciesObject(sp_id_it.second, sp_it.first, year);
                    createSpeciesFolder(new_species);
                    species.push_back(new_species);
                    for (auto c_it : sp_it.second) {
                        for (auto o_it : c_it.second) {
                            if (o_it->getTempSpeciesId()==sp_id_it.first) {
                                new_individual_organisms_in_current_year[new_species][c_it.first].push_back(o_it);
                                o_it->setSpecies(new_species);
                            }
                        }
                    }

                }
            } else {
                new_individual_organisms_in_current_year[sp_it.first] = sp_it.second;
            }
        }
        individual_organisms_in_current_year = new_individual_organisms_in_current_year;

        LOG(INFO)<<"End to rebuild the organism structure in this year";

        LOG(INFO)<<"begin to generate group maps";
        boost::unordered_map<SpeciesObject*, SparseMap*> group_maps;
        for (auto sp_it : individual_organisms_in_current_year) {
            if (group_maps.find(sp_it.first)==group_maps.end()) {
                group_maps[sp_it.first] = new SparseMap(xSize, ySize);
            }
            if (sp_it.second.size()>0) {
                for (auto o_id : sp_it.second) {
                    if (o_id.second.size()>0) {
                        group_maps[sp_it.first]->setValue(o_id.second[0]->getX(), o_id.second[0]->getY(), o_id.second[0]->getGroupId());
                    }
                }
            } else {
                sp_it.first->setDisappearedYear(year);
                group_maps[sp_it.first] = NULL;
            }
        }
        for (auto it : group_maps) {
            if (it.second!=NULL) {
                if ((tifLimit>=tif_number)||it.first->isNewSpecies()){
                    //save distribution
                    //LOG(INFO)<<"Save distribution no." << tif_number;
                    std::string speciesFolder = getSpeciesFolder(it.first);
                    char tiffName[speciesFolder.length() + 28];
                    sprintf(tiffName, "%s/groupsmap/%s.tif", speciesFolder.c_str(),
                            CommonFun::fixedLength(year, 7).c_str());
                    int* array = it.second->toArray();
                    RasterController::writeGeoTIFF(tiffName, xSize, ySize, geoTrans,
                            array, (double) NODATA, GDT_Int32);
                    delete[] array;
                    tif_number++;
                }else{
                    //LOG(INFO)<<"To the limitation, skip to save useless distribution map!";
                }
            } else {

                individual_organisms_in_current_year.erase(it.first);
            }
            it.first->setNewSpecies(false);
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
        LOG(INFO)<<"Remove the organisms. Before removing, Memory usage:"<<CommonFun::getCurrentRSS();
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
                boost::unordered_map<SpeciesObject*, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > > temp_o = all_individualOrganisms[removed_year];
                for (auto it1 : temp_o) {
                    for (auto it2 : it1.second) {
                        for (auto o_it : it2.second) {
                            o_it->clearChildren();
                        }
                        CommonFun::clearVector(&it2.second);
                    }
                }
                all_individualOrganisms.erase(removed_year);
            }
        }
        LOG(INFO)<<"Remove the organisms. After  removing, Memory usage:"<<CommonFun::getCurrentRSS();
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

    //generate the speciation information
    //get root species object
    std::vector<SpeciesObject*> roots;
    for (auto sp_it : species) {
        if (sp_it->getDisappearedYear() == 0) {
            sp_it->setDisappearedYear(totalYears);
        }
    }
    for (auto sp_it : species) {
        if (sp_it->getAppearedYear() == 0) {
            sp_it->markNode(totalYears);
            roots.push_back(sp_it);
        }
    }

    for (auto sp_it : roots) {
        std::string folder = getSpeciesFolder(sp_it);
        std::string newick = folder + "/tree.new";
        CommonFun::writeFile(sp_it->getNewickTree(true, false), newick.c_str());
        std::string html = folder + "/Phylogram.html";
        CommonFun::writeFile(sp_it->getHTMLTree(), html.c_str());
        std::string stat = folder + "/stat.csv";
        CommonFun::writeFile(sp_it->getSpeciationExtinction(true, totalYears),
                stat.c_str());

    }

}
unsigned short Scenario::getTempSpeciesID(unsigned short group_id,
        boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > organisms) {
    for (auto c_it : organisms) {
        for (auto o_it : c_it.second) {
            if (o_it->getGroupId()==group_id){
                return o_it->getTempSpeciesId();
            }
        }
    }
    return 0;
}
void Scenario::markedSpeciesID(unsigned short group_id,
        unsigned short temp_species_id,
        boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > organisms) {
    for (auto c_it : organisms) {
        for (auto o_it : c_it.second) {
            if (o_it->getGroupId() == group_id) {
                o_it->setTempSpeciesId(temp_species_id);
            }
        }
    }
}
unsigned Scenario::getMinDividedYear(unsigned short group_id_1,
        unsigned short group_id_2,
        boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > organisms) {
    unsigned nearest_divided_year = 0;
    unsigned current_year = 0;
    std::vector<IndividualOrganism*> group_1;
    std::vector<IndividualOrganism*> group_2;
    for (auto c_it : organisms) {
        for (auto o_it : c_it.second) {
            current_year = o_it->getYear();
            if (o_it->getGroupId() == group_id_1) {
                group_1.push_back(o_it);
            } else if (o_it->getGroupId() == group_id_2) {
                group_2.push_back(o_it);
            }
        }
    }
    for (auto o_it_1 : group_1) {
        for (auto o_it_2 : group_2) {
            unsigned divided_year = getDividedYear(o_it_1, o_it_2);
            nearest_divided_year =
                    (divided_year > nearest_divided_year) ?
                            divided_year : nearest_divided_year;
        }
        //printf("%u/%u\n", i++, group_1.size() * group_2.size());
    }
    return current_year - nearest_divided_year;
}
unsigned Scenario::getDividedYear(IndividualOrganism* o_1,
        IndividualOrganism* o_2) {
    IndividualOrganism* parent_1 = o_1->getParent();
    IndividualOrganism* parent_2 = o_2->getParent();
    if ((parent_1 == NULL) || (parent_2 == NULL)) {
        return 0;
    }
    if (parent_1->getGroupId() == parent_2->getGroupId()) {
        return parent_1->getYear();
    } else {
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

