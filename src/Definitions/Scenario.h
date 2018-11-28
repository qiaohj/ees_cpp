/**
 * @file Scenario.h
 * @brief The Scenario class
 * @author Huijie Qiao
 * @version 1.0
 * @date 11/25/2018
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */



#ifndef DEFINITIONS_SCENARIO_H_
#define DEFINITIONS_SCENARIO_H_
#include <string>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include "../Universal/CommonFun.h"
#include "../JsonPaster/include/json/json.h"
#include "../Universal/CommonType.h"
#include "../RasterIO/RasterController.h"
#include "SparseMap.h"
#include "EnvironmentalHadley.h"
#include "SpeciesObject.h"
#include "IndividualOrganism.h"
#include "CoodLocation.h"
#include "../Universal/log.hpp"

/**
 * The Scenario class is to define the features of a virtual scenario in a simulation, and the virtual species in the scenario.
 */
class Scenario{
private:

	bool isFinished;
	bool isOverwrite;
    unsigned long memLimit;
    std::vector<EnvironmentalHadley*> environments;
    std::vector<SpeciesObject*> species;
    boost::unordered_map<unsigned, boost::unordered_map<unsigned, double>> distances;
    unsigned totalYears;
    unsigned minSpeciesDispersalSpeed;
    SparseMap* mask;
    std::string baseFolder;
    std::string target;
    double* geoTrans;
    unsigned xSize, ySize;
    char *prj;
    void cleanActivedIndividualOrganisms();
    void cleanEnvironments();
    void cleanSpecies();
    /**
     * @brief Create all the necessary folders for a species
     * @param p_species the species to create the folder
     * @param isRoot is a root species of the simulation?
     */
    void createSpeciesFolder(SpeciesObject* p_species, bool isRoot);
    unsigned burnInYear;
    boost::unordered_map<unsigned, boost::unordered_map<SpeciesObject*,
            boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > > > all_individualOrganisms;
    std::vector<CoodLocation*> getDispersalMap_2(
            IndividualOrganism* individualOrganism, std::string species_folder, unsigned year);
    IndividualOrganism* getUnmarkedOrganism(
            boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms);
    void markJointOrganism(unsigned short p_group_id,
            IndividualOrganism* p_unmarked_organism,
            boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms);
    unsigned getMinDividedYear(unsigned speciation_year, unsigned short group_id_1,
            unsigned short group_id_2, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms, unsigned current_year);
    unsigned getMinDividedYear_minDistance(unsigned speciation_year, unsigned short group_id_1,
            unsigned short group_id_2, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms, unsigned current_year);
    unsigned getDividedYear(IndividualOrganism* o_1, IndividualOrganism* o_2);
    void markedSpeciesID(unsigned short group_id, unsigned short temp_species_id, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms);
    unsigned short getTempSpeciesID(unsigned short group_id, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms);
    /**
     * @brief Get the folder of the given species
     * @param p_species The species to return the folder
     * @return The folder of the species in the parameter.
     */
    std::string getSpeciesFolder(SpeciesObject* p_species);
    void generateSpeciationInfo(unsigned year, bool is_tree);
    bool with_detail;
    void getExtend(int p_dispersal_ability, int x, int y,
    		boost::unordered_set<unsigned>* x_extent,
    		boost::unordered_set<unsigned>* y_extent);
    double distanceFast(int x1, int y1, int x2, int y2);
public:
    /**
     * @brief Constructor of Scenario class
     *
	 * @param p_scenario_json_path A path points to the configuration file (in JSON format) which defines the features of the scenario.
	 * @param p_scenario_id A unique ID of the scenario
	 * @param p_base_folder A base folder where the application can load the required resource
	 * @param p_target A folder to save simulation results.
	 * @param p_overwrite Overwrite the folder or not if the folder is existed.
	 * @param p_mem_limit The maximum memory allocated to the application.
	 * @param p_with_detail Output the details of the simulation (all occupy a huge hard disk space) or not. Only for debug.
     */
    Scenario(const std::string p_scenario_json_path, std::string p_scenario_id, std::string p_base_folder,
            std::string p_target, bool p_overwrite, unsigned long p_mem_limit, bool p_with_detail);

    /**
     * @brief Destructor of Scenario class
     *
     * release all the resources
     */
    virtual ~Scenario();

    /**
     * @brief Run the simulation
     */
    unsigned run();
    /**
     * @brief Detect whether the simulation is terminated.
     * @return Return the terminated reason. TRUE means the simulation is terminated because it finishes the simulation successfully.
     */
    bool isTerminated();

    /**
     * @brief Return the environmental layers based on the given time step
     * @param p_year The time step to get the environmental layers.
     * @return The environmental layers at the given time step
     */
    std::vector<SparseMap*> getEnvironmenMap(unsigned p_year);

    /**
     * @brief Return the folder to save the result of the simulation.
     * @return The folder to save the result of the simulation.
     */
    std::string getTarget();

    /**
     * @brief Save the distribution map, the population, and the speciation information to the log file
     * @param year The time step to save the result
     * @param species_group_maps The population information to save
     */
    void saveGroupmap(unsigned year, boost::unordered_map<SpeciesObject*, SparseMap*> species_group_maps);
    //bool generateEnv();
};

#endif /* DEFINITIONS_SCENARIO_H_ */
