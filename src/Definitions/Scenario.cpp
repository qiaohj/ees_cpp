/**
 * @file Scenario.cpp
 * @brief Class Scenario. A class to define the features of a virtual scenario in a simulation, and the virtual species in the scenario.
 * @author Huijie Qiao
 * @version 1.0
 * @date 11/25/2018
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */


#include "Scenario.h"

Scenario::Scenario(const std::string p_scenario_json_path, std::string p_scenario_id,
		std::string p_base_folder, std::string p_target, bool p_overwrite,
		unsigned long p_mem_limit, bool p_with_detail, bool p_isSQLite) {

	//initialize the required parameters for the simualtion
	isSQLite = p_isSQLite;
	with_detail = p_with_detail;
	Json::Value root_Scenario = CommonFun::readJson(p_scenario_json_path.c_str());
	memLimit = p_mem_limit;
	baseFolder = p_base_folder;
	target = p_target + "/" + p_scenario_id;

	LOG(INFO)<<"Save result to " <<target;

	isFinished = boost::filesystem::exists(target);
	isOverwrite = p_overwrite;

	/*-------------------
	 * If the target folder exists and the is_overwrite parameter is false, skip the simulation,
	 * or overwrite the existing result with the new simulation.
	 -------------------------*/

	if ((isFinished)&&(!p_overwrite)){
		return;
	}

	//Create the necessary folders.
	CommonFun::createFolder(target.c_str());
	if (isSQLite){
		CommonFun::deleteFile((target + "/log.db").c_str());
		createDB((target + "/log.db").c_str());
	}else{
		CommonFun::createFolder((target + "/Map_Folder").c_str());
	}
	//Load the required parameters of the scenario from the JSON file.
	totalYears = root_Scenario.get("total_years", 120000).asInt();
	RasterObject* mask_raster = new RasterObject(
			root_Scenario.get("mask", "").asString());
	geoTrans = new double[6];
	memcpy(geoTrans, mask_raster->getGeoTransform(), 6 * sizeof(*geoTrans));
	prj = new char[strlen(mask_raster->getProjectionRef()) + 1];
	strcpy(prj, mask_raster->getProjectionRef());
	mask = new SparseMap(mask_raster, true);
	xSize = mask_raster->getXSize();
	ySize = mask_raster->getYSize();
	minSpeciesDispersalSpeed = totalYears;
	Json::Value species_json_array = root_Scenario["species"];
	//Load the species parameters from the JSON file(s).
	for (unsigned index = 0; index < species_json_array.size(); ++index) {
		std::string species_json_path = baseFolder
				+ std::string("/Species_Configurations/")
				+ species_json_array[index].asString() + std::string(".json");
		SpeciesObject* species = new SpeciesObject(species_json_path.c_str());
		this->species.push_back(species);
		createSpeciesFolder(species, true);
		std::vector<GeoLocation*> seeds = species->getSeeds();
		boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > t_o;

		std::vector<std::string> output;
		char line[100];
		sprintf(line, "%s,%s", "long", "lat");
		output.push_back(line);
		/* -----------------
		 * Create the individual organism(s) based on the seeds in the species' configuration.
		 * All the individual organism(s) has(have) the same parameters inherited from the species and the different localities.
		 * Now there is only on seed per species.
		 * Don't use the function of the multiple species and multiple seeds per species now,
		 * because it hasn't be tested strictly.
		 *-------------------------*/
		for (unsigned i = 0; i < seeds.size(); ++i) {
			char line[100];
			sprintf(line, "%f,%f", seeds[i]->getLongitude(), seeds[i]->getLatitude());
			output.push_back(line);
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
		/*---------------------
		 *
		 * get the minimal dispersal speed from all the species in the simulation.
		 * It is unused parameter now, because there is only one species per scenario.
		 * It might be used later.
		 *
		 -----------------------*/

		minSpeciesDispersalSpeed =
				(species->getDispersalSpeed() < minSpeciesDispersalSpeed) ?
						species->getDispersalSpeed() : minSpeciesDispersalSpeed;
		char filepath2[target.length() + 15];
		sprintf(filepath2, "%s/seeds.csv", target.c_str());
		CommonFun::writeFile(output, filepath2);
		output.clear();
	}

	//Load the environments layers for the simulation.
	LOG(INFO)<<"Load environments";
	Json::Value environment_json_array = root_Scenario["environments"];
	environments.reserve(environment_json_array.size());
	burnInYear = root_Scenario.get("burn_in_year", 0).asInt();
	for (unsigned index = 0; index < environment_json_array.size(); ++index) {
		LOG(INFO)<<"Load environments of "<<index;
		std::string environment_folder_path = environment_json_array[index].asString();
		/// @todo Here is a hard code (120000, 0 and 100) to be solved.
		EnvironmentalHadley* layer = new EnvironmentalHadley(environment_folder_path, geoTrans, burnInYear, 120000, 0, 100);
		environments.push_back(layer);
	}


	delete mask_raster;
	LOG(INFO)<<"Finished";
}

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

/*-----------------
 * Create all the necessary folders for a species
 */
void Scenario::createSpeciesFolder(SpeciesObject* p_species, bool isRoot) {
	std::string speciesFolder = getSpeciesFolder(p_species);
	char groupsmapFolder[speciesFolder.length() + 6 + 10];
	sprintf(groupsmapFolder, "%s/groupsmap", speciesFolder.c_str());
	if (isRoot){
		char statsFolder[speciesFolder.length() + 6 + 10];
		sprintf(statsFolder, "%s/stats", speciesFolder.c_str());
		CommonFun::createFolder(statsFolder);
	}

}

/*-------------------------
 * Save the population information for a specific time step to a single file
 *-----------------------*/
void Scenario::saveGroupmap_file(unsigned year, boost::unordered_map<SpeciesObject*, SparseMap*> species_group_maps){
	//LOG(INFO)<<"Save result to file";
	if (species_group_maps.size()==0){
		return;
	}
	std::vector<std::string> output;
	char line[50];
	// Note: The old version has only 5 columns without lon and lat columns.
	sprintf(line, "year,x,y,lon,lat,group,sp_id");
	output.push_back(line);
	for (auto sp_it : species_group_maps){
		SpeciesObject* sp = sp_it.first;
		SparseMap* map = sp_it.second;
		if (map){
			for (unsigned x = 0; x<map->getXSize();x++){
				for (unsigned y=0;y<map->getYSize();y++){
					int v = map->readByXY(x, y);
					if (v>0){
						std::string id = sp->getIDWithParentID();
						char line[id.size() + 50];
						double lon, lat;
						CommonFun::XY2LL(geoTrans, x, y, &lon, &lat);
						sprintf(line, "%u,%u,%u,%f,%f,%d,%s", year, x, y, lon, lat, v,id.c_str());
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

/*-------------------------
 * Save the population information for a specific time step to a db
 *-----------------------*/
void Scenario::saveGroupmap_db(unsigned year, boost::unordered_map<SpeciesObject*, SparseMap*> species_group_maps){
	//LOG(INFO)<<"Save result to db";
	if (species_group_maps.size()==0){
		return;
	}
	std::vector<std::string> output;
	char line[100];
	// Note: The old version has only 5 columns without lon and lat columns.
	sprintf(line, "insert into map (YEAR, X, Y, LON, LAT, group_id, sp_id ) values ");
	output.push_back(line);
	int i = 0;
	for (auto sp_it : species_group_maps){
		SpeciesObject* sp = sp_it.first;
		SparseMap* map = sp_it.second;
		if (map){
			for (unsigned x = 0; x<map->getXSize();x++){
				for (unsigned y=0;y<map->getYSize();y++){
					int v = map->readByXY(x, y);
					if (v>0){
						std::string id = sp->getIDWithParentID();
						char line[id.size() + 100];
						double lon, lat;
						CommonFun::XY2LL(geoTrans, x, y, &lon, &lat);
						if (i==0){
							sprintf(line, " (%u,%u,%u,%f,%f,%d,%s) ", year, x, y, lon, lat, v, id.c_str());
						}else{
							sprintf(line, " ,(%u,%u,%u,%f,%f,%d,%s) ", year, x, y, lon, lat, v, id.c_str());
						}
						i = 1;
						output.push_back(line);
					}
				}
			}
		}
	}
	output.push_back(";");
	if (output.size()>0){
		CommonFun::executeSQL(output, db);
		output.clear();
	}
}
void Scenario::createDB(const char* path) {

	int rc;
	rc = sqlite3_open(path, &db);
	if (rc) {
		LOG(INFO)<<"Can't open database: "<<sqlite3_errmsg(db);
		exit(0);
	} else {
		LOG(INFO)<<"Opened database successfully";
	}
	char *zErrMsg = 0;
	//Create a table to save the log // year,x,y,lon,lat,group,sp_id
	char *sql = "CREATE TABLE map("  \
	         "YEAR		INT				NOT NULL," \
	         "X			INT				NOT NULL," \
			 "Y			INT				NOT NULL," \
	         "LON		REAL			NOT NULL," \
			 "LAT		REAL			NOT NULL," \
			 "group_id	INT				NOT NULL," \
	         "sp_id		CHAR(50)				);";
	CommonFun::executeSQL(sql, db);

}
/*-------------------------
 * Save the population information for a specific time step
 *-----------------------*/
void Scenario::saveGroupmap(unsigned year, boost::unordered_map<SpeciesObject*, SparseMap*> species_group_maps){
	if (species_group_maps.size()==0){
		return;
	}
	if (isSQLite){
		saveGroupmap_db(year, species_group_maps);
	}else{
		saveGroupmap_file(year, species_group_maps);
	}
}
/*---------------------------
 * Run a simulation on a scenario with the species in the scenario.
 *---------------------*/
unsigned Scenario::run() {
	boost::unordered_map<SpeciesObject*, SparseMap*> species_group_maps;
	std::vector<std::string> stat_output;

	for (unsigned year = minSpeciesDispersalSpeed; year <= totalYears; year +=
			minSpeciesDispersalSpeed) {
		LOG(INFO)<<"Current year:"<<year << " @ " << target <<" Memory usage:"<<CommonFun::getCurrentRSS();

		boost::unordered_map<SpeciesObject*, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > >
			individual_organisms_in_current_year;
		std::vector<SparseMap*> current_environments = getEnvironmenMap(year);


		char line[100];

		//Create the active individual organisms via cloning the individual organisms from the previous time step.
		boost::unordered_map<SpeciesObject*, boost::unordered_map<unsigned, IndividualOrganism*> > actived_individualOrganisms;
		for (auto sp_it : all_individualOrganisms[year - minSpeciesDispersalSpeed]) {

			SpeciesObject* sp = sp_it.first;
			for (auto c_it : sp_it.second) {
				if (c_it.second.size()>0) {
					actived_individualOrganisms[sp][c_it.first] = c_it.second.front();
				}
			}
		}

		int organism_count = 0;

		//Handle the active individual organisms one by one.
		//LOG(INFO)<<"start to simulate organism by species. Count of species is " << actived_individualOrganisms.size();
		for (auto s_it : actived_individualOrganisms) {
			//If it is the beginning of the simulation, generate a suitable layer for the species;
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

				//LOG(INFO)<<"2:"<<prj;
				RasterController::writeGeoTIFF(tiffName, xSize, ySize, geoTrans,
										array, (double) NODATA, GDT_Int32, prj);
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
							//LOG(INFO)<<"CALL getDispersalMap_2";
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
			organism_count += new_individual_organisms.size();
			new_individual_organisms.clear();
            //LOG(INFO)<<"end to simulate organism by organism.";
		}

        //LOG(INFO)<<"end to simulate organism by species. Count of species is " << actived_individualOrganisms.size() << ". Count of all organisms is " << organism_count;
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
						//LOG(INFO)<<"Remove it because of unsuitable";
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
					organism_count--;
				}
				individual_organisms_in_current_year[sp_it.first][key].clear();
				std::vector<IndividualOrganism*>().swap(individual_organisms_in_current_year[sp_it.first][key]);
				individual_organisms_in_current_year[sp_it.first].erase(key);
			}
		}
		//LOG(INFO)<<"after remove unsuitable, Count of all organisms is " << organism_count;


		boost::unordered_map<SpeciesObject*, std::vector<unsigned>> erased_keys2;
		//Remove the species which distribution is smaller than X for Y time steps
		for (auto sp_it : individual_organisms_in_current_year) {
			SpeciesObject* species = sp_it.first;
			/*LOG(INFO)<<"Group map size"<<sp_it.second.size()<<" CurrentSpeciesExtinctionTimeSteps"<<
					species->getCurrentSpeciesExtinctionTimeSteps()<<"/"<<species->getSpeciesExtinctionTimeSteps()<<
					" MaxSpeciesDistribution:"<<species->getMaxSpeciesDistribution()<<
					" SpeciesExtinctionThreaholdPercentage:"<<species->getSpeciesExtinctionThreaholdPercentage();
					*/
			if ((sp_it.second.size()>0)
					&&((species->getCurrentSpeciesExtinctionTimeSteps()<species->getSpeciesExtinctionTimeSteps()))
					&&(sp_it.second.size()>=(species->getMaxSpeciesDistribution() * species->getSpeciesExtinctionThreaholdPercentage()))) {

				species->setMaxSpeciesDistribution((sp_it.second.size()>species->getMaxSpeciesDistribution())?sp_it.second.size():species->getMaxSpeciesDistribution());
				if ((sp_it.second.size()<=species->getSpeciesExtinctionThreshold())
						&&(year>=1000)){
					species->addCurrentSpeciesExtinctionTimeSteps();
				} else {
					species->setCurrentSpeciesExtinctionTimeSteps(0);
				}
				//LOG(INFO)<<"1";
			} else {
				std::vector<unsigned> erased_key;
				for (auto it : sp_it.second) {
					if (it.second.size()>0) {
						erased_key.push_back(it.first);
					}
				}
				erased_keys2[sp_it.first] = erased_key;
				//LOG(INFO)<<"2";
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
			individual_organisms_in_current_year.erase(sp_it.first);
		}

		//LOG(INFO)<<"end to remove unsuitable organisms.";

		//mark the group id for every organisms in this year, seperated by species id;
		//LOG(INFO)<<"Begin to mark the group id, and detect the speciation.";
		for (auto sp_it : individual_organisms_in_current_year) {
			//printf("Species ID:%u\n", sp_it.first->getID());
			boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > organisms = sp_it.second;
			SpeciesObject* species = sp_it.first;
			unsigned short current_group_id = 1;
			if (year>=(burnInYear)) {
			//if (year>=(burnInYear + species->getSpeciationYears())) {
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
				//sp_it.first->setDisappearedYear(year);
				group_maps[sp_it.first] = NULL;
			}

		}

		saveGroupmap(year, group_maps);


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

		sprintf(line, "%u,%lu,%lu,%lu,%lu,%lu,%lu", year, CommonFun::getCurrentRSS(),
				c_size, o_size, mem_size, species_size, all_individualOrganisms[year].size());
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
	/// @todo Here can improve the efficiency a lot! We need a new algorithm!
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
double Scenario::distanceFast(int x1, int y1, int x2, int y2){
	/// @todo Now we are calculating the Euclidean distance. But the Great Cirle Distance is more reasonable. And to support the transferring from one side to another side of the map, we need to improve this function later.
	return CommonFun::EuclideanDistance(x1, y1, x2, y2);
	/*if (CommonFun::AlmostEqualRelative(e_distance, 1.0)){
		return e_distance;
	}
	int index_1 = x1 + y1 * xSize;
	int index_2 = x2 + y2 * xSize;
	//bool is_slow = false;
	if (distances.find(index_1) == distances.end()){
		double distance = CommonFun::GreatCirleDistanceFast(x1, y1, x2, y2, poCT, geoTrans, resolution);
		distances[index_1][index_2] = distance;
		distances[index_2][index_1] = distance;

		//is_slow = true;
	}else if (distances[index_1].find(index_2) == distances[index_1].end()){
		double distance = CommonFun::GreatCirleDistanceFast(x1, y1, x2, y2, poCT, geoTrans, resolution);
		distances[index_1][index_2] = distance;
		distances[index_2][index_1] = distance;

		//is_slow = true;
	}

	return distances[index_1][index_2];*/
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
		IndividualOrganism *org1 = (*organisms)[o_it_1].front();
		int x1 = org1->getX();
		int y1 = org1->getY();
		for (auto o_it_2 : group_c_2) {
			IndividualOrganism *org2 = (*organisms)[o_it_2].front();
			int x2 = org2->getX();
			int y2 = org2->getY();
			int dispersal_ability = (org2->getDispersalAbility()>org1->getDispersalAbility())?org2->getDispersalAbility():org1->getDispersalAbility();
			double distance = distanceFast(x1, y1, x2, y2);
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
void Scenario::getExtend(int p_dispersal_ability, int x, int y,
		boost::unordered_set<unsigned>* x_extent,
		boost::unordered_set<unsigned>* y_extent){
	/// @todo Now the simulation hasn't support an individual to move from one side of the map to another side directly. In fact, a species can do it because our earth is a shpere. Will support it in the future.
	double distance = 0;
	int i_x = x;
	(*x_extent).insert(x);
	int tried = 0;
	while (distance <= p_dispersal_ability){
		i_x -= 1;
		if (i_x<0){
			i_x = xSize + i_x;
		}
		tried++;
		/// @todo Here is a hard code, need to change 180 to the edge of the map later.
		if (tried>180){
			break;
		}
		if (mask->readByXY(i_x, y)==NODATA){
			//LOG(INFO)<<"Skip "<< i_x;
			continue;
		}
		distance = distanceFast((int) i_x, (int) y,
									(int) (x), (int) (y));

		//LOG(INFO)<<"1. X="<<x<<" i_x="<<i_x<<" distance:"<<distance;
		if (CommonFun::AlmostEqualRelative(distance, -1.0)) continue;
		//distance = distance - 1;

		if (distance>p_dispersal_ability){
			break;
		}
		//LOG(INFO)<<"PUT "<< i_x;
		(*x_extent).insert(i_x);
	}

	distance = 0;
	i_x = x;
	tried = 0;
	while (distance <= p_dispersal_ability){
		i_x += 1;
		if (i_x >= xSize){
			i_x = xSize - i_x;
		}
		tried++;
		/// @todo Here is a hard code, need to change 180 to the edge of the map later.
		if (tried>180){
			break;
		}
		if (mask->readByXY(i_x, y)==NODATA){
			continue;
		}
		distance = distanceFast((int) i_x, (int) y,
									(int) (x), (int) (y));
		//LOG(INFO)<<"2. X="<<x<<" i_x="<<i_x<<" distance:"<<distance;
		if (CommonFun::AlmostEqualRelative(distance, -1.0)) continue;
		if (distance>p_dispersal_ability){
			break;
		}

		//LOG(INFO)<<"PUT "<< i_x;
		(*x_extent).insert(i_x);
	}


	for (int i_y = y - p_dispersal_ability; i_y <= y + p_dispersal_ability; ++i_y) {
		i_y = (i_y < 0) ? 0 : i_y;
		if (i_y > ySize){
			break;
		}
		distance = distanceFast((int) x, (int) i_y,
									(int) (x), (int) (y));

		if (CommonFun::AlmostEqualRelative(distance, -1.0)) continue;

		if (distance > p_dispersal_ability) continue;

		(*y_extent).insert(i_y);
	}

	//LOG(INFO)<<"p_dispersal_ability:"<<p_dispersal_ability<<" x_extent:"<<(*x_extent).size()<<" y_extent:"<<(*y_extent).size();
}
void Scenario::markJointOrganism(unsigned short p_group_id,
		IndividualOrganism* p_unmarked_organism,
		boost::unordered_map<unsigned,
		std::vector<IndividualOrganism*> >* organisms) {
	unsigned short x = p_unmarked_organism->getX();
	unsigned short y = p_unmarked_organism->getY();
	unsigned short p_dispersal_ability = p_unmarked_organism->getDispersalAbility();
	if (p_dispersal_ability==0){
		p_dispersal_ability = 1;
	}
	boost::unordered_set<unsigned> x_extent;
	boost::unordered_set<unsigned> y_extent;

	for (int i_x = (x - p_dispersal_ability); i_x <= (x + p_dispersal_ability);
			++i_x) {
		int next_x = i_x;
		if (next_x < 0) {
			continue;
		}

		if (next_x >= xSize) {
			continue;
		}
		for (int i_y = (y - p_dispersal_ability);
				i_y <= (y + p_dispersal_ability); ++i_y) {
			if (i_y < 0)
				continue;

			if (i_y >= (int) ySize)
				continue;
			double distance = distanceFast((int) i_x, (int) i_y, (int) (x),
					(int) (y));
			if (CommonFun::AlmostEqualRelative(distance, -1.0)) {
				continue;
			}
			//LOG(INFO)<<"X="<<i_x<<", Y="<<i_y<<", x="<<x<<",y="<<y<<",distance="<<distance<<",disper_abi="<<p_dispersal_ability;
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
	//LOG(INFO)<<"environments.size()"<<environments.size();
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
	//LOG(INFO)<<"p_dispersal_ability:"<<p_dispersal_ability;
	//get all the cells whose E-distances are not longer than dispersal ability.
	//When number of path = 1, ignore the dispersal method parameter.
	//if (year<1000){
	//	if (p_dispersal_ability==0){
	//		p_dispersal_ability = 1;
	//	}
	//}
	if (individualOrganism->getNumOfPath() == -1) {
		int x = (int) individualOrganism->getX();
		int y = (int) individualOrganism->getY();

		boost::unordered_set<unsigned> x_extent;
		boost::unordered_set<unsigned> y_extent;
		//getExtend(p_dispersal_ability, x, y, &x_extent, &y_extent);
		//LOG(INFO)<<"p_dispersal_ability:"<<p_dispersal_ability<<" x_extent:"<<x_extent.size()<<" y_extent:"<<y_extent.size();
		for (int i_x = (x - p_dispersal_ability);
				i_x <= (x + p_dispersal_ability); ++i_x) {
			int next_x = i_x;
			if (next_x < 0) {
				continue;
			}

			if (next_x >= xSize) {
				continue;
			}
			for (int i_y = (y - p_dispersal_ability);
					i_y <= (y + p_dispersal_ability); ++i_y) {
				if (i_y < 0)
					continue;

				if (i_y >= (int) ySize)
					continue;

				//LOG(INFO)<<p_dispersal_ability;
				//LOG(INFO)<<"calculate the distance";
				double distance = distanceFast((int) i_x, (int) i_y, (int) (x),
						(int) (y));
				//LOG(INFO)<<"X="<<x<<" i_x="<<i_x<<" Y="<<y<<" i_y="<<i_y<<" disntance="<<distance<<" dispersal ability="<<p_dispersal_ability;
				//LOG(INFO)<<"end to calculate the distance";
				//double distance = CommonFun::EuclideanDistance((int) i_x,
				//		(int) i_y, (int) (x), (int) (y));
				//printf("%u, %u vs %u, %u, Distance:%f, Distance2: %f, disperial ability:%u\n", i_x, i_y, x, y,
				//        distance, distance2, p_dispersal_ability);
				if ((distance <= p_dispersal_ability)
						|| (CommonFun::AlmostEqualRelative(distance,
								(double) p_dispersal_ability))) {
					//LOG(INFO)<<"TRUE";
					CoodLocation* v = new CoodLocation(next_x, i_y);
					new_cells.push_back(v);
				} else {
					//LOG(INFO)<<"FALSE";
				}
			}
		}
	}
	return new_cells;
}

Scenario::~Scenario() {
	delete[] geoTrans;
	delete mask;
	CommonFun::executeSQL("CREATE INDEX idx_year ON map (year)", db);
	sqlite3_close(db);
//	cleanEnvironments();
//	cleanActivedIndividualOrganisms();
//	cleanSpecies();
}

std::string Scenario::getTarget() {
	return target;
}
