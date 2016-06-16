/*
 * Scenario.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: qiaohj
 */

#include "Scenario.h"

Scenario::Scenario(const std::string p_scenario_json_path, std::string p_scenario_id,
		std::string p_base_folder, std::string p_target, bool p_overwrite,
		unsigned long p_mem_limit, bool p_with_detail,
		const char* p_fromWkt, const char* p_toWkt, int p_resolution,
		OGRCoordinateTransformation *p_poCT) {
	fromWkt = p_fromWkt;
	toWkt = p_toWkt;
	poCT = p_poCT;
	resolution = p_resolution;
	with_detail = p_with_detail;
	Json::Value root_Scenario = CommonFun::readJson(p_scenario_json_path.c_str());
	memLimit = p_mem_limit;
	baseFolder = p_base_folder;
	target = p_target + "/" + p_scenario_id;
	LOG(INFO)<<"Save result to " <<target;

	isFinished = boost::filesystem::exists(target);
	isOverwrite = p_overwrite;
	//isFinished = false;
	if ((isFinished)&&(!p_overwrite)){
		return;
	}

	CommonFun::createFolder(target.c_str());
	CommonFun::createFolder((target + "/Map_Folder").c_str());

	totalYears = root_Scenario.get("total_years", 120000).asInt();

	RasterObject* mask_raster = new RasterObject(
			root_Scenario.get("mask", "").asString());
	geoTrans = new double[6];
	memcpy(geoTrans, mask_raster->getGeoTransform(), 6 * sizeof(*geoTrans));

	mask = new SparseMap(mask_raster, true);
	xSize = mask_raster->getXSize();
	ySize = mask_raster->getYSize();
	minSpeciesDispersalSpeed = totalYears;
	Json::Value species_json_array = root_Scenario["species"];

	for (unsigned index = 0; index < species_json_array.size(); ++index) {
		std::string species_json_path = baseFolder
				+ std::string("/Species_Configurations/")
				+ species_json_array[index].asString() + std::string(".json");
		SpeciesObject* species = new SpeciesObject(species_json_path.c_str());
		this->species.push_back(species);
		createSpeciesFolder(species, true);
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
	LOG(INFO)<<"Load environments";
	Json::Value environment_json_array = root_Scenario["environments"];
	environments.reserve(environment_json_array.size());
	burnInYear = root_Scenario.get("burn_in_year", 0).asInt();
	for (unsigned index = 0; index < environment_json_array.size(); ++index) {
		LOG(INFO)<<"Load environments of "<<index;
		std::string environment_folder_path = environment_json_array[index].asString();
		EnvironmentalHadley* layer = new EnvironmentalHadley(environment_folder_path, geoTrans, burnInYear, 120000, 0, 100);
		environments.push_back(layer);
	}

	delete mask_raster;
	LOG(INFO)<<"Finished";
}
/*
bool Scenario::generateEnv(){

	for (unsigned y=0;y<=500000;y+=500){
		std::vector<SparseMap*> current_environments = getEnvironmenMap(y);
		for (unsigned i = 0; i < current_environments.size(); ++i) {
			std::string folder = "/home/huijieqiao/NBProject/NB_Configurations/EurAsia/env_list";
			char tiffName[folder.length() + 28];
			sprintf(tiffName, "%s/%d_%s.tif", folder.c_str(), i, CommonFun::fixedLength(y, 7).c_str());
			RasterController::writeGeoTIFF(tiffName, current_environments[i]->getXSize(), current_environments[i]->getYSize(), geoTrans,
					current_environments[i]->toArray(), (double) NODATA, GDT_Int32);
		}
	}

	return true;
}
*/
std::string Scenario::getSpeciesFolder(SpeciesObject* p_species) {
	if (p_species->getParent() == NULL) {
		char speciesFolder[target.length() + 6];
		sprintf(speciesFolder, "%s", target.c_str());
		CommonFun::createFolder(speciesFolder);
		return std::string(speciesFolder);
	} else {
		std::string parentFolder = getSpeciesFolder(p_species->getParent());
		char speciesFolder[parentFolder.length() + 6];
		sprintf(speciesFolder, "%s", parentFolder.c_str());
		//CommonFun::createFolder(speciesFolder);
		return std::string(speciesFolder);
	}
}
bool Scenario::isTerminated() {
	if ((isFinished)&&(!isOverwrite)){
			return true;
	}
	return false;
}
void Scenario::createSpeciesFolder(SpeciesObject* p_species, bool isRoot) {
	std::string speciesFolder = getSpeciesFolder(p_species);

//    char dispersalFolder[speciesFolder.length() + 6 + 10];
//    sprintf(dispersalFolder, "%s/dispersal", speciesFolder.c_str());
//    CommonFun::createFolder(dispersalFolder);

	char groupsmapFolder[speciesFolder.length() + 6 + 10];
	sprintf(groupsmapFolder, "%s/groupsmap", speciesFolder.c_str());
//	CommonFun::createFolder(groupsmapFolder);
//	char groupslogFolder[speciesFolder.length() + 6 + 10];
//	sprintf(groupslogFolder, "%s/group_log", speciesFolder.c_str());
//	CommonFun::createFolder(groupslogFolder);
	if (isRoot){
		char statsFolder[speciesFolder.length() + 6 + 10];
		sprintf(statsFolder, "%s/stats", speciesFolder.c_str());
		CommonFun::createFolder(statsFolder);
	}

}
void Scenario::saveGroupmap(unsigned year, boost::unordered_map<SpeciesObject*, SparseMap*> species_group_maps){
	if (species_group_maps.size()==0){
		return;
	}
	std::vector<std::string> output;

	for (auto sp_it : species_group_maps){
		SpeciesObject* sp = sp_it.first;
		SparseMap* map = sp_it.second;
		if (map){
			for (unsigned x = 0; x<map->getXSize();x++){
				for (unsigned y=0;y<map->getYSize();y++){
					int v = map->readByXY(x, y);
					if (v>0){
						std::string id = sp->getIDWithParentID();
						char line[id.size() + 30];
						sprintf(line, "%u,%u,%u,%d,%s", year, x, y, v,id.c_str());
						output.push_back(line);
					}
				}
			}
		}
	}
	if (output.size()>0){
		char filepath2[target.length() + 15];
		sprintf(filepath2, "%s/Map_Folder/%d.csv", target.c_str(), year);
		CommonFun::writeFile(output, filepath2);
		output.clear();
	}
}
unsigned Scenario::run() {
	boost::unordered_map<SpeciesObject*, SparseMap*> species_group_maps;

	unsigned tif_number = 0;

	std::vector<std::string> stat_output;

	//std::vector<std::string> env_output;
	//unsigned x = 99999, y = 99999;
	/*for (unsigned year = minSpeciesDispersalSpeed; year <= totalYears; year +=
			minSpeciesDispersalSpeed) {
		std::vector<SparseMap*> current_environments = getEnvironmenMap(year);
		if (x == 99999) {
			int value;
			current_environments[0]->getFirstValues(&x, &y, &value);
		}
		char line[30];
		int v = current_environments[0]->readByXY(x, y);
		sprintf(line, "%u,%u,%u,%d", year, x, y, v);
		env_output.push_back(line);
		//CommonFun::clearVector(&current_environments);
	}
	char filepath2[target.length() + 15];
	sprintf(filepath2, "%s/env_curve.csv", target.c_str());
	CommonFun::writeFile(env_output, filepath2);
	env_output.clear();
	*/
//    return;
//    bool is_write_memory_usage = false;
	for (unsigned year = minSpeciesDispersalSpeed; year <= totalYears; year +=
			minSpeciesDispersalSpeed) {
		LOG(INFO)<<"Current year:"<<year << " @ " << target <<" Memory usage:"<<CommonFun::getCurrentRSS();

		boost::unordered_map<SpeciesObject*, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > >
			individual_organisms_in_current_year;
		std::vector<SparseMap*> current_environments = getEnvironmenMap(year);

		//save the env data
		char line[100];
		boost::unordered_map<SpeciesObject*, boost::unordered_map<unsigned, IndividualOrganism*> > actived_individualOrganisms;
		for (auto sp_it : all_individualOrganisms[year - minSpeciesDispersalSpeed]) {

			SpeciesObject* sp = sp_it.first;
			for (auto c_it : sp_it.second) {
				if (c_it.second.size()>0) {
					actived_individualOrganisms[sp][c_it.first] = c_it.second.front();
				}
			}
		}
		//LOG(INFO)<<"start to simulate organism by species. Count of species is " << actived_individualOrganisms.size();
		for (auto s_it : actived_individualOrganisms) {
			//Generate a suitable layer for the species;
			if (year==minSpeciesDispersalSpeed){
				std::string speciesFolder = getSpeciesFolder(s_it.first);
				std::vector<NicheBreadth*> nicheBreadth = s_it.first->getNicheBreadth();
				char tiffName[speciesFolder.length() + 28];
				sprintf(tiffName, "%s/suitable.tif", speciesFolder.c_str());
				std::vector<SparseMap*> current_environments = getEnvironmenMap(year);
				int* array = current_environments[0]->toArray();
				//LOG(INFO)<<"Begin to generate the suitable area";
				for (unsigned x=0; x<current_environments[0]->getXSize();x++){
					for (unsigned y=0;y<current_environments[0]->getYSize();y++){
					    for (unsigned i = 0; i < nicheBreadth.size(); ++i) {
					        int env_value = current_environments[i]->readByXY(x, y);

					        if (env_value == NODATA) {
								array[x + y * current_environments[0]->getXSize()] = NODATA;
								break;
							}else{
								//LOG(INFO)<<env_value<<"; min:"<<nicheBreadth[i]->getMin()<<"; max:"<<nicheBreadth[i]->getMax();
								if ((env_value > nicheBreadth[i]->getMax())
										|| (env_value < nicheBreadth[i]->getMin())) {
									array[x + y * current_environments[0]->getXSize()] = 0;
									break;
								}else{
									array[x + y * current_environments[0]->getXSize()] = 1;
								}
							}
						}
					}
				}
				RasterController::writeGeoTIFF(tiffName, xSize, ySize, geoTrans,
						array, (double) NODATA, GDT_Int32);

				//LOG(INFO)<<"END to generate the suitable area";
				//exit(1);
			}
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
							individualOrganism->setRandomDispersalAbility();
							next_cells = getDispersalMap_2(individualOrganism, getSpeciesFolder(s_it.first), year);
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
						new_individualOrganism->setRandomDispersalAbility();
						new_individual_organisms.push_back(new_individualOrganism);
					}
					for (std::vector<CoodLocation*>::iterator it =
							next_cells.begin(); it != next_cells.end(); ++it) {
						delete *it;
					}
					next_cells.clear();
					std::vector<CoodLocation*>().swap(next_cells);
				} else {
//					LOG(INFO) << "Didn't run, for current year is "<<year<< " and organism run year is " << individualOrganism->getNextRunYear();
				}
			}

			for (auto it : new_individual_organisms) {
				unsigned index = it->getY() * xSize + it->getX();
				//species id, index
				individual_organisms_in_current_year[s_it.first][index].push_back(it);
			}
			new_individual_organisms.clear();
            //LOG(INFO)<<"end to simulate organism by organism.";
		}

        //LOG(INFO)<<"end to simulate organism by species. Count of species is " << actived_individualOrganisms.size() << ". Count of all organisms is " << all_individualOrganisms.size();
		//LOG(INFO)<<"end to simulate cell by cell";

		//remove the unsuitable organisms
		//LOG(INFO)<<"begin to remove the unsuitable organisms.";
		boost::unordered_map<SpeciesObject*, std::vector<unsigned>> erased_keys;
		for (auto s_it : individual_organisms_in_current_year) {
            //LOG(INFO)<<"start to remove unsuitable organisms.";
			std::vector<unsigned> erased_key;
			for (auto it : s_it.second) {
				if (it.second.size()>0) {
					if (!it.second.front()->isSuitable(&current_environments)) {
						erased_key.push_back(it.first);
					}
				}
			}
			erased_keys[s_it.first] = erased_key;
            //LOG(INFO)<<"end to remove unsuitable organisms.";
		}
		for (auto sp_it : erased_keys) {
			for (auto key : sp_it.second) {
				for (std::vector<IndividualOrganism*>::iterator it = individual_organisms_in_current_year[sp_it.first][key].begin();
						it != individual_organisms_in_current_year[sp_it.first][key].end(); ++it) {
//                    (*it)->getParent()->removeChild(*it);
					delete *it;
				}
				individual_organisms_in_current_year[sp_it.first][key].clear();
				std::vector<IndividualOrganism*>().swap(individual_organisms_in_current_year[sp_it.first][key]);
				individual_organisms_in_current_year[sp_it.first].erase(key);
			}
		}
		boost::unordered_map<SpeciesObject*, std::vector<unsigned>> erased_keys2;
		//Remove the species which distribution is smaller than X for Y time steps
		for (auto sp_it : individual_organisms_in_current_year) {
			//LOG(INFO)<<"Group map size"<<sp_it.second.size()<<" CurrentSpeciesExtinctionTimeSteps"<<species->getCurrentSpeciesExtinctionTimeSteps()<<"/"<<species->getSpeciesExtinctionTimeSteps();
			SpeciesObject* species = sp_it.first;
			if ((sp_it.second.size()>0)
					&&((species->getCurrentSpeciesExtinctionTimeSteps()<species->getSpeciesExtinctionTimeSteps()))
					&&(sp_it.second.size()>=(species->getMaxSpeciesDistribution() * species->getSpeciesExtinctionThreaholdPercentage()))) {
				//if ((sp_it.second.size()>0)) {
				species->setMaxSpeciesDistribution((sp_it.second.size()>species->getMaxSpeciesDistribution())?sp_it.second.size():species->getMaxSpeciesDistribution());
				if (sp_it.second.size()<=species->getSpeciesExtinctionThreshold()) {
					species->addCurrentSpeciesExtinctionTimeSteps();
				} else {
					species->setCurrentSpeciesExtinctionTimeSteps(0);
				}
			} else {
				/*LOG(INFO)<<"Present "<<sp_it.second.size()<<" Max "<<species->getMaxSpeciesDistribution()<<" per " <<
						(species->getMaxSpeciesDistribution() * species->getSpeciesExtinctionThreaholdPercentage());
				if ((sp_it.second.size()>0)
					&&(sp_it.second.size()<(species->getMaxSpeciesDistribution() * species->getSpeciesExtinctionThreaholdPercentage()))){
					LOG(INFO)<<"Smaller than 80% detected";
				}*/
				std::vector<unsigned> erased_key;
				for (auto it : sp_it.second) {
					if (it.second.size()>0) {
						erased_key.push_back(it.first);
					}
				}
				erased_keys2[sp_it.first] = erased_key;
			}
		}
		for (auto sp_it : erased_keys2) {
			for (auto key : sp_it.second) {
				for (std::vector<IndividualOrganism*>::iterator it = individual_organisms_in_current_year[sp_it.first][key].begin();
						it != individual_organisms_in_current_year[sp_it.first][key].end(); ++it) {
//                    (*it)->getParent()->removeChild(*it);
					delete *it;
				}
				individual_organisms_in_current_year[sp_it.first][key].clear();
				std::vector<IndividualOrganism*>().swap(individual_organisms_in_current_year[sp_it.first][key]);
				individual_organisms_in_current_year[sp_it.first].erase(key);
			}
			sp_it.second.clear();
			sp_it.first->setDisappearedYear(year);
		}

		//LOG(INFO)<<"end to remove unsuitable organisms.";

		//mark the group id for every organisms in this year, seperated by species id;
		//LOG(INFO)<<"Begin to mark the group id, and detect the speciation.";
		for (auto sp_it : individual_organisms_in_current_year) {
			//printf("Species ID:%u\n", sp_it.first->getID());
			boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > organisms = sp_it.second;
			SpeciesObject* species = sp_it.first;
			unsigned short current_group_id = 1;
			if (year>=(burnInYear + species->getSpeciationYears())) {
				//LOG(INFO)<<"Begin to mark the organism.";
				IndividualOrganism* unmarked_organism = getUnmarkedOrganism(&organisms);
				while (unmarked_organism!=NULL) {
					//LOG(INFO)<<"Unmarked organism is "<<unmarked_organism->getX() <<", "<<unmarked_organism->getY()
					//		<<" dispersal ability is "<<unmarked_organism->getDispersalAbility()<<". current_group_id is "<<current_group_id;
					markJointOrganism(current_group_id, unmarked_organism, &organisms);
					current_group_id++;
					unmarked_organism = getUnmarkedOrganism(&organisms);
				}
				/*if (current_group_id>1000){
					LOG(INFO)<<"current_group_id is "<<current_group_id;
					exit(1);
				}*/
				//LOG(INFO)<<"End to mark the organism.";
				//detect the speciation
				unsigned short temp_species_id = 1;
//				std::vector<std::string> group_output;
				//LOG(INFO)<<"Begin to detect speciation.";
				for (unsigned short group_id_1=1; group_id_1<current_group_id-1; group_id_1++) {
					//LOG(INFO)<<"getTempSpeciesID 1 for group "<<group_id_1 <<" current_group_id is "<<current_group_id;
					unsigned short temp_species_id_1 = getTempSpeciesID(group_id_1, &organisms);
					for (unsigned short group_id_2=group_id_1+1; group_id_2<current_group_id; group_id_2++) {
						//LOG(INFO)<<"getTempSpeciesID 2 for group "<<group_id_2 <<" current_group_id is "<<current_group_id;
						unsigned short temp_species_id_2 = getTempSpeciesID(group_id_2, &organisms);
						//if both groups were marked, and they have the same id, skip it.
						if ((temp_species_id_1!=0)&&(temp_species_id_2!=0)&&(temp_species_id_1==temp_species_id_2)) {
//							char line[100];
//							sprintf(line, "%u,%u,%u,%u,%u,%u,%s", year, group_id_1, group_id_2, temp_species_id_1, temp_species_id_2, 0, "skip");
//							group_output.push_back(line);
							continue;
						}
						//LOG(INFO)<<"get min_divided_year.";
						unsigned min_divided_year = getMinDividedYear(sp_it.first->getSpeciationYears(), group_id_1, group_id_2, &organisms, year);
						//LOG(INFO)<<"get min_divided_year is "<<min_divided_year;

						if (min_divided_year>=species->getSpeciationYears()) {
							//LOG(INFO)<<"mark species id with new id.";
							//if a speciation happened, marked them with two ids if they were not marked.
							if (temp_species_id_1==0) {
//								char line[100];
//								sprintf(line, "%u,%u,%u,%u,%u,%u,%s", year, group_id_1, group_id_2, temp_species_id, 0, min_divided_year, "new species, mark group 1");
//								group_output.push_back(line);
								markedSpeciesID(group_id_1, temp_species_id, &organisms);
								temp_species_id_1 = temp_species_id;
								temp_species_id++;
							}
							if (temp_species_id_2==0) {
//								char line[100];
//								sprintf(line, "%u,%u,%u,%u,%u,%u,%s", year, group_id_1, group_id_2, temp_species_id, 0, min_divided_year, "new species, mark group 2");
//								group_output.push_back(line);
								markedSpeciesID(group_id_2, temp_species_id, &organisms);
								temp_species_id_2 = temp_species_id;
								temp_species_id++;
							}
							//LOG(INFO)<<"end mark species id with new id.";
						} else {
							//LOG(INFO)<<"mark species id with old id.";
							//if there is not speciation, marked them with the same id
							unsigned short t_id = (temp_species_id_1==0)?temp_species_id_2:temp_species_id_1;
							t_id = (t_id==0)?temp_species_id:t_id;
							markedSpeciesID(group_id_1, t_id, &organisms);
							temp_species_id_1 = t_id;
//                            char line1[100];
//                            sprintf(line1, "%u,%u,%u,%u,%u,%u,%s", year, group_id_1, group_id_2, t_id, 0, min_divided_year, "same species, mark group 1");
//                            group_output.push_back(line1);
							markedSpeciesID(group_id_2, t_id, &organisms);
							temp_species_id_2 = t_id;
							//LOG(INFO)<<"end to mark species id with old id.";
//                            char line2[100];
//                            sprintf(line2, "%u,%u,%u,%u,%u,%u,%s", year, group_id_1, group_id_2, t_id, 0, min_divided_year, "same species, mark group 2");
//                            group_output.push_back(line2);
						}
					}
				}
				//LOG(INFO)<<"End to detect speciation.";

//				if (group_output.size()>0) {
//					char filepath[target.length() + 50];
//					sprintf(filepath, "%s/group_log/%u_group.txt", getSpeciesFolder(sp_it.first).c_str(), year);
//					CommonFun::writeFile(group_output, filepath);
//					group_output.clear();
//				}
			} else {
				for (auto y_it : sp_it.second) {
					for (auto o_it : y_it.second) {
						o_it->setGroupId(current_group_id);
					}
				}
			}
		}
		//LOG(INFO)<<"end to mark the group id, and detect the speciation.";

		//LOG(INFO)<<"Begin to rebuild the organism structure in this year";
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
					//createSpeciesFolder(new_species, false);
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

		//LOG(INFO)<<"End to rebuild the organism structure in this year";
		//LOG(INFO)<<"begin to generate group maps";
		boost::unordered_map<SpeciesObject*, SparseMap*> group_maps;
		for (auto sp_it : individual_organisms_in_current_year) {
			SpeciesObject* species = sp_it.first;
			if (group_maps.find(species)==group_maps.end()) {
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
//		for (auto it : group_maps) {
//			if (it.second!=NULL) {
//				if (it.first->isNewSpecies()||with_detail||(year==burnInYear)||(year==totalYears)){
//					//save distribution
//					std::string speciesFolder = getSpeciesFolder(it.first);
//					char tiffName[speciesFolder.length() + 28];
//					sprintf(tiffName, "%s/groupsmap/%s.tif", speciesFolder.c_str(),
//							CommonFun::fixedLength(year, 7).c_str());
//					int* array = it.second->toArray();
//					RasterController::writeGeoTIFF(tiffName, xSize, ySize, geoTrans,
//							array, (double) NODATA, GDT_Int32);
//					delete[] array;
//					tif_number++;
//				}
//
//			} else {
//				individual_organisms_in_current_year.erase(it.first);
//			}
//			it.first->setNewSpecies(false);
//		}
		saveGroupmap(year, group_maps);
		//clear group_maps;
//		std::vector<SpeciesObject*> erased_key;
//		for (auto it : group_maps) {
//			erased_key.push_back(it.first);
//		}
//		for (auto key : erased_key) {
//			if (group_maps[key] != NULL) {
//				delete group_maps[key];
//			}
//			group_maps.erase(key);
//		}
//		group_maps.clear();
//		erased_key.clear();


		all_individualOrganisms.insert(std::make_pair(year, individual_organisms_in_current_year));

		//remove the useless organism
		//LOG(INFO)<<"Remove the useless organisms. Before removing, Memory usage:"<<CommonFun::getCurrentRSS();
		for (auto sp_it : species) {
			if (year<sp_it->getDispersalSpeed()) {
				continue;
			}
			unsigned speciation_year = sp_it->getSpeciationYears();
			bool is_remove_previous_span = false;
			bool is_remove_previous_speciation = false;
			if ((int)year<((int)burnInYear - (int)speciation_year)) {
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
				//LOG(INFO)<<"Current year is "<<year<<". Remove organisms at year "<< removed_year<<".";
				boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > temp_o = all_individualOrganisms[removed_year][sp_it];
				for (auto it1 : temp_o) {
					CommonFun::clearVector(&it1.second);
				}
//				all_individualOrganisms.erase(removed_year);

//				LOG(INFO)<<"Set the parent to NULL for year "<<removed_year + sp_it->getDispersalSpeed();
				temp_o = all_individualOrganisms[removed_year + sp_it->getDispersalSpeed()][sp_it];
				for (auto it1 : temp_o) {
					for (auto it2 : it1.second) {
						it2->setParent(NULL);
					}
				}

//				LOG(INFO)<<"Done!";

			}
		}
		//LOG(INFO)<<"Remove the useless organisms. After  removing, Memory usage:"<<CommonFun::getCurrentRSS();

		//LOG(INFO)<<"Generate speciation information.";
		generateSpeciationInfo(year, false);
		CommonFun::clearVector(&current_environments);
		//LOG(INFO)<<"Save stat information.";
		unsigned long o_size = 0;
		unsigned long c_size = 0;
		unsigned long mem_size = 0;
		unsigned long species_size = 0;
//		for (auto year_it : all_individualOrganisms) {
//			species_size += year_it.second.size();
//			for (auto sp_it : year_it.second) {
//				for (auto c_it : sp_it.second) {
//					o_size += c_it.second.size();
//					for (auto o_it : c_it.second) {
//						mem_size += o_it->getMemoryUsage();
//					}
//				}
//				c_size += sp_it.second.size();
//			}
//
//		}

		sprintf(line, "%u,%lu,%lu,%lu,%lu,%lu,%lu", year, CommonFun::getCurrentRSS(), c_size, o_size, mem_size, species_size, all_individualOrganisms[year].size());
		stat_output.push_back(line);

		if ((CommonFun::getCurrentRSS()>memLimit)&&(year<100000)) {
			char filepath[target.length() + 16];
			sprintf(filepath, "%s/stat_curve.csv", target.c_str());
			CommonFun::writeFile(stat_output, filepath);
			generateSpeciationInfo(year, true);

			return 1;
		}
	}

	generateSpeciationInfo(totalYears, true);
	return 0;
}
void Scenario::generateSpeciationInfo(unsigned year, bool is_tree) {
	std::vector<SpeciesObject*> roots;
	for (auto sp_it : species) {
		if (sp_it->getAppearedYear() == 0) {
			sp_it->markNode(year);
			roots.push_back(sp_it);
		}
	}

	for (auto sp_it : roots) {
		std::string folder = getSpeciesFolder(sp_it);
		if (is_tree){
			std::string newick = folder + "/tree.new";
			CommonFun::writeFile(sp_it->getNewickTree(true, false, year),
					newick.c_str());
			std::string html = folder + "/Phylogram.html";
			CommonFun::writeFile(sp_it->getHTMLTree(year), html.c_str());
		}
//		std::string stat = folder + "/stats/" + CommonFun::fixedLength(year, 7)
//				+ "_stat.csv";
		std::string stat = folder + "/stats/stat.csv";
		CommonFun::writeFile(sp_it->getSpeciationExtinction(true, year),
				stat.c_str());

	}
}
unsigned short Scenario::getTempSpeciesID(unsigned short group_id,
		boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms) {
	for (auto c_it : (*organisms)) {
		for (auto o_it : c_it.second) {
			if (o_it->getGroupId() == group_id) {
				return o_it->getTempSpeciesId();
			}
		}
	}
	return 0;
}
void Scenario::markedSpeciesID(unsigned short group_id,
		unsigned short temp_species_id,
		boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms) {
	//here can improve the effencty a lot!
	//LOG(INFO)<<"Size of organism 1 is "<<organisms->size();
	for (auto c_it : (*organisms)) {
		//LOG(INFO)<<"Size of organism 2 is "<<c_it.second.size();
		for (auto o_it : c_it.second) {
			if (o_it->getGroupId() == group_id) {
				o_it->setTempSpeciesId(temp_species_id);
			}
		}
	}
}
unsigned Scenario::getMinDividedYear_minDistance(unsigned speciation_year,
		unsigned short group_id_1, unsigned short group_id_2,
		boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms,
		unsigned current_year) {
	unsigned nearest_divided_year = 0;
	std::vector<unsigned> group_c_1;
	std::vector<unsigned> group_c_2;
	double min_distance = 9999999;
	unsigned group_1_index = 0;
	unsigned group_2_index = 0;
	for (auto c_it : (*organisms)) {
		if (c_it.second.front()->getGroupId() == group_id_1) {
			group_c_1.push_back(c_it.first);
		} else if (c_it.second.front()->getGroupId() == group_id_2) {
			group_c_2.push_back(c_it.first);
		}
	}
	for (auto o_it_1 : group_c_1) {
		int x1 = (*organisms)[o_it_1].front()->getX();
		int y1 = (*organisms)[o_it_1].front()->getY();
		for (auto o_it_2 : group_c_2) {
			int x2 = (*organisms)[o_it_2].front()->getX();
			int y2 = (*organisms)[o_it_2].front()->getY();
			double distance = CommonFun::GreatCirleDistanceFast(x1, y1, x2, y2, poCT, geoTrans, resolution);
			if (min_distance > distance) {
				min_distance = distance;
				group_1_index = o_it_1;
				group_2_index = o_it_2;
			}
		}
	}

	std::vector<IndividualOrganism*> group_1 = (*organisms)[group_1_index];
	std::vector<IndividualOrganism*> group_2 = (*organisms)[group_2_index];

	for (auto o_it_1 : group_1) {
		for (auto o_it_2 : group_2) {
			//if ((o_it_1->getGroupId()==group_id_1)&&(o_it_2->getGroupId()==group_id_2)){
				unsigned divided_year = getDividedYear(o_it_1, o_it_2);
				nearest_divided_year =
						(divided_year > nearest_divided_year) ?
								divided_year : nearest_divided_year;
				if ((current_year - nearest_divided_year) < speciation_year) {
					return current_year - nearest_divided_year;
				}
			//}
		}
		//printf("%u/%u\n", i++, group_1.size() * group_2.size());
	}
	return current_year - nearest_divided_year;
}

unsigned Scenario::getMinDividedYear(unsigned speciation_year,
		unsigned short group_id_1, unsigned short group_id_2,
		boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms,
		unsigned current_year) {
	unsigned nearest_divided_year = 0;
	std::vector<unsigned> group_c_1;
	std::vector<unsigned> group_c_2;
	unsigned group_1_index = 0;
	unsigned group_2_index = 0;

	//save all the organisms in that two groups
	std::vector<IndividualOrganism*> group_1 = (*organisms)[group_1_index];
	std::vector<IndividualOrganism*> group_2 = (*organisms)[group_2_index];

	for (auto c_it : (*organisms)) {
		for (auto o_it : c_it.second){
			if (o_it->getGroupId() == group_id_1){
				group_1.push_back(o_it);
			}else if (o_it->getGroupId() == group_id_2){
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
			if ((current_year - nearest_divided_year) < speciation_year) {
				return current_year - nearest_divided_year;
			}
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
		boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms) {
	unsigned short x = p_unmarked_organism->getX();
	unsigned short y = p_unmarked_organism->getY();
	unsigned short p_dispersal_ability = p_unmarked_organism->getDispersalAbility();
	if (p_dispersal_ability==0){
		p_dispersal_ability = 1;
	}
	for (int i_x = (x - p_dispersal_ability);
			i_x <= (x + p_dispersal_ability); ++i_x) {
		int next_x = i_x;
		if (next_x < 0){
			next_x = xSize + next_x;
		}

		if (next_x>=xSize){
			next_x = next_x - xSize;
		}
		for (int i_y = (y - p_dispersal_ability);
				i_y <= (y + p_dispersal_ability);
				++i_y) {
			i_y = (((int) i_y) < 0) ? 0 : i_y;

			if (i_y >= (int)ySize)
				break;
//            LOG(INFO)<<"X="<<i_x<<", Y="<<i_y;
			double distance = CommonFun::GreatCirleDistanceFast((int) i_x, (int) i_y,
					(int) (x), (int) (y), poCT, geoTrans, resolution);
			//double distance = CommonFun::EuclideanDistance((int) i_x, (int) i_y,
			//		(int) (x), (int) (y));
			if (distance > p_dispersal_ability) {
//                LOG(INFO)<<"skip 1";
				continue;
			}
			if (organisms->find(i_y * xSize + next_x) == organisms->end()) {
//                LOG(INFO)<<"skip 2";
				continue;
			}
			unsigned short group_id =
					(*organisms)[i_y * xSize + next_x].front()->getGroupId();
			if (group_id != 0) {
//                LOG(INFO)<<"skip 3";
				continue;
			}
			for (auto it : (*organisms)[i_y * xSize + next_x]) {
				it->setGroupId(p_group_id);
				if ((x != next_x) || (y != i_y)) {
					markJointOrganism(p_group_id, it, organisms);
				}
			}
		}
	}
}

IndividualOrganism* Scenario::getUnmarkedOrganism(
		boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms) {
	for (auto p_it : (*organisms)) {
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
		IndividualOrganism* individualOrganism, std::string species_folder, unsigned year) {
	std::vector<CoodLocation*> new_cells;

	unsigned short p_dispersal_ability = individualOrganism->getDispersalAbility();

	//get all the cells whose E-distances are not longer than dispersal ability.
	//When number of path = 1, ignore the dispersal method parameter.
	if (individualOrganism->getNumOfPath() == -1) {
		int x = (int)individualOrganism->getX();
		int y = (int)individualOrganism->getY();
		for (int i_x = (x - p_dispersal_ability);
				i_x <= (x + p_dispersal_ability); ++i_x) {

			int next_x = i_x;
			if (next_x < 0){
				next_x = xSize + next_x;
//				std::vector<std::string> special_log;
//				char line[100];
//				sprintf(line, "round cross x detected %u", year);
//				special_log.push_back(line);
//				char filepath[target.length() + 50];
//				sprintf(filepath, "%s/round_cross_log.txt", species_folder.c_str());
//				CommonFun::writeFile(special_log, filepath);
			}

			if (next_x>=xSize){
				next_x = next_x - xSize;
//				std::vector<std::string> special_log;
//				char line[100];
//				sprintf(line, "round cross x detected %u", year);
//				special_log.push_back(line);
//				char filepath[target.length() + 50];
//				sprintf(filepath, "%s/round_cross_log.txt", species_folder.c_str());
//				CommonFun::writeFile(special_log, filepath);
			}
			for (int i_y = (y - p_dispersal_ability);
					i_y <= (y + p_dispersal_ability);
					++i_y) {
				i_y = (((int) i_y) < 0) ? 0 : i_y;
				if ((unsigned) i_y >= ySize)
					break;
				double distance = CommonFun::GreatCirleDistanceFast((int) i_x,
								(int) i_y, (int) (x), (int) (y), poCT, geoTrans, resolution);
				//double distance = CommonFun::EuclideanDistance((int) i_x,
				//		(int) i_y, (int) (x), (int) (y));
//                printf("%u, %u vs %u, %u, Distance:%f\n", i_x, i_y, x, y,
//                        distance);
				if ((distance < p_dispersal_ability)
						|| (CommonFun::AlmostEqualRelative(distance,
								(double) p_dispersal_ability))) {
					CoodLocation* v = new CoodLocation(next_x, i_y);
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
//	cleanEnvironments();
//	cleanActivedIndividualOrganisms();
//	cleanSpecies();
}

std::string Scenario::getTarget() {
	return target;
}
