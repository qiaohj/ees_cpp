/**
 * @file Scenario.h
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



#ifndef DEFINITIONS_SCENARIO_H_
#define DEFINITIONS_SCENARIO_H_
#include <string>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <sqlite3.h>
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
 * @brief to define the features of a virtual scenario in a simulation, and the virtual species in the scenario.
 */
class Scenario{
private:
	/// @brief a sqlite db connection to save the results.
	sqlite3 *db;
	/// @brief Whether the simulation runs to the end, or be terminated before the end-time-step because of an exception (such as out of memory, full hard disk, or another exception)
	bool isFinished;
	/// @brief If the target folder exists, overwrite it or skip it.
	bool isOverwrite;
	/// @brief The maximum memory allocated to the application
    unsigned long memLimit;
    /// @brief If save the results to a sqlite database. Suggested to set it to true
    bool isSQLite;
    /// @brief The environmental variables used in the simulation.
    std::vector<EnvironmentalHadley*> environments;
    /// @brief The virtual species in the simulation, including the initial species, and new species after the speciation events.
    std::vector<SpeciesObject*> species;

    //useless
    boost::unordered_map<unsigned, boost::unordered_map<unsigned, double>> distances;

    /// @brief The last time step
    unsigned totalYears;

    /// @brief The time span per time step
    unsigned minSpeciesDispersalSpeed;

    /// @brief The mask file of the simulation
    SparseMap* mask;

    /// @brief The base folder of the simulation
    std::string baseFolder;

    /// @brief The target folder where the log files is saved.
    std::string target;

    /// @brief The GeoTransform matrix of the raster layers, can be extract from the mask or any environmental layers.
    double* geoTrans;

    /// @brief X and Y size of the mask layer
    unsigned xSize, ySize;

    /// @brief Projection string of the mask layer
    char *prj;

    /**
     * @brief Remove all the individual objects and release the resources.
     */
    void cleanActivedIndividualOrganisms();
    /**
	 * @brief Remove all the environmental variable objects and release the resources.
	 */
    void cleanEnvironments();

    /**
	 * @brief Remove all the species object and release the resources.
	 */
    void cleanSpecies();

    /**
     * @brief Create all the necessary folders for a species
     * @param p_species the species to create the folder
     * @param isRoot is a root species of the simulation?
     */

    void createSpeciesFolder(SpeciesObject* p_species, bool isRoot);

    /// @brief Burn-in year of the simulation
    unsigned burnInYear;

    /// @brief A hash map to save all the living individual objects in the simulation.
    boost::unordered_map<unsigned, boost::unordered_map<SpeciesObject*,
            boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > > > all_individualOrganisms;

    /**
     * @brief Get the potential distribution of a given individual in the next time step.
     * @param individualOrganism The given individual object
     * @param species_folder The folder to save the log file
     * @param year Time step
     * @return Return the potential distribution in pixel.
     */
    std::vector<CoodLocation*> getDispersalMap_2(
            IndividualOrganism* individualOrganism, std::string species_folder, unsigned year);

    /**
     * @brief Get the first individual object without a population ID from all the individuals.
     * @param organisms All the individuals
     * @return The first individual object
     */
    IndividualOrganism* getUnmarkedOrganism(
            boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms);

    /**
     * @brief Allocate a population ID to the individuals which connect with the given individual.
     * @param p_group_id A group ID to be allocated.
     * @param p_unmarked_organism The given individual object
     * @param organisms All the individuals
     */
    void markJointOrganism(unsigned short p_group_id,
            IndividualOrganism* p_unmarked_organism,
            boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms);

    /**
     * @brief Get the minimal separating time length of two populations
     * @param speciation_year Speciation year
     * @param group_id_1 Population 1
     * @param group_id_2 Population 2
     * @param organisms All the individuals
     * @param current_year Current time step.
     */
    unsigned getMinDividedYear(unsigned speciation_year, unsigned short group_id_1,
            unsigned short group_id_2, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms, unsigned current_year);

    /**
     * @brief An enhanced function of 'markJointOrganism'. The simulation would detect all the individuals in population 1 and 2, but the the individual-pair which distance smaller than a threshold. It is faster but will cause some bugs. Unused now.
     * @param speciation_year
     * @param group_id_1
     * @param group_id_2
     * @param organisms
     * @param current_year
     */
    unsigned getMinDividedYear_minDistance(unsigned speciation_year, unsigned short group_id_1,
            unsigned short group_id_2, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms, unsigned current_year);

    /**
     * Get the separating time length of two individuals.
     * @param o_1 individual 1
     * @param o_2 individual 2
     */
    unsigned getDividedYear(IndividualOrganism* o_1, IndividualOrganism* o_2);

    /**
     * @brief Give a new species ID to all the individuals in a population when a speciation event happens.
     * @param group_id The population ID
     * @param temp_species_id species ID
     * @param organisms All the individuals
     */
    void markedSpeciesID(unsigned short group_id, unsigned short temp_species_id, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms);

    /**
     * @brief Get a temporary species ID based on the parent species ID and population ID
     * @param group_id The population ID
     * @param organisms All the individuals
     */
    unsigned short getTempSpeciesID(unsigned short group_id, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms);
    /**
     * @brief Get the folder of the given species
     * @param p_species The species to return the folder
     * @return The folder of the species in the parameter.
     */
    std::string getSpeciesFolder(SpeciesObject* p_species);
    /**
     * @brief Generate the speciation information of the simulation, including the speciation tree (in both NEXUS and HTML format), number of speciation and extinction, etc.
     * @param year time step
     * @param is_tree Whether generating the speciation tree.
     */
    void generateSpeciationInfo(unsigned year, bool is_tree);

    /// @brief Whether outputing the details of the simulation, for debug only.
    bool with_detail;

    /**
     * @brief Get the range of potential distributed pixels of an individual in the next time step based on the dispersal ability.
     * @param p_dispersal_ability Dispersal ability
     * @param x The x of the individual
     * @param y The y of the individual
     * @param x_extent The x(s) that the individual can dispersal in the next time step.
     * @param y_extent The y(s) that the individual can dispersal in the next time step.
     */
    void getExtend(int p_dispersal_ability, int x, int y,
    		boost::unordered_set<unsigned>* x_extent,
    		boost::unordered_set<unsigned>* y_extent);

    /**
     * @brief Calculate the Euclidean Distance of two points.
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     * @return
     */
    double distanceFast(int x1, int y1, int x2, int y2);

    /**
     * @brief create a db used to save the logs
     * @param path
     */
    void createDB(const char* path);
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
	 * @param p_isSQLite save the results to a sqlite database.
     */
    Scenario(const std::string p_scenario_json_path, std::string p_scenario_id, std::string p_base_folder,
            std::string p_target, bool p_overwrite, unsigned long p_mem_limit, bool p_with_detail, bool p_isSQLite);

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

    void saveGroupmap_file(unsigned year, boost::unordered_map<SpeciesObject*, SparseMap*> species_group_maps);

    void saveGroupmap_db(unsigned year, boost::unordered_map<SpeciesObject*, SparseMap*> species_group_maps);
    //bool generateEnv();
};

#endif /* DEFINITIONS_SCENARIO_H_ */
