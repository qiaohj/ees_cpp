/**
 * @file SpeciesObject.h
 * @brief Class SpeciesObject. A class to handle the attributes and behaviors of a virtual species
 * @author Huijie Qiao
 * @version 1.0
 * @date 11/25/2018
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */


#ifndef DEFINITIONS_SPECIESOBJECT_H_
#define DEFINITIONS_SPECIESOBJECT_H_

#include <string>
#include <vector>
#include "../JsonPaster/include/json/json.h"
#include "SparseMap.h"
#include "NicheBreadth.h"
#include "GeoLocation.h"
/**
 * @brief A class to handle the attributes and behaviors of a virtual species
 */
class SpeciesObject {
private:
    bool newSpecies;
    unsigned id;
    std::vector<NicheBreadth*> nicheBreadth;
    double* dispersalAbility;
    unsigned short dispersalAbilityLength;
    unsigned dispersalSpeed;
    unsigned speciationYears;
    int dispersalMethod;
    int numberOfPath;
    unsigned appearedYear;
    unsigned disappearedYear;
    unsigned speciesExtinctionThreshold;
    unsigned groupExtinctionThreshold;
    unsigned speciesExtinctionTimeSteps;
    unsigned currentSpeciesExtinctionTimeSteps;
    double speciesExtinctionThreaholdPercentage;
    unsigned maxSpeciesDistribution;
    std::vector<GeoLocation*> seeds;
    SpeciesObject* parent;
    std::vector<SpeciesObject*> children;
    unsigned number_of_clade_extinction;
    unsigned number_of_speciation;
    unsigned number_of_species_extinction;
    /// @brief 0: unknown, 1: extincted, 2: unextincted 3: parent_extincted
    unsigned clade_extinction_status;
public:
    /**
     * @brief Constructor of SpeciesObject class No.1 (Create a species object based on the configuration in JSON format)
     * @param json_path the path to configuration file in JSON format
     */
    SpeciesObject(const std::string json_path);
    /**
     * @brief Constructor of SpeciesObject class No.2 (Create a species object based on the parent species)
     * @param p_id species ID
     * @param p_parent the parent species object
     * @param p_year time step
     */
    SpeciesObject(unsigned p_id, SpeciesObject* p_parent, unsigned p_year);
    /**
	 * @brief Destructor of SpeciesObject class
	 *
	 * release all the resources
	 */
    virtual ~SpeciesObject();
    /**
     * @brief return the dispersal ability array
     * @return
     */
    double* getDispersalAbility();
    /**
	 * @brief return the dispersal speed
	 * @return
	 */
    unsigned getDispersalSpeed();
    /**
	 * @brief return the length of the dispersal ability array
	 * @return
	 */
    unsigned short getDispersalAbilityLength();
    /**
	 * @brief return the species extinction threshold. If the area of the distribution (number of the pixels occupied by the species) is smaller than the threshold for some time steps (defined with SpeciesExtinctionTimeSteps), the species will go extinct.
	 * @return
	 */
    unsigned getSpeciesExtinctionThreshold();
    /**
	 * @brief return the population extinction threshold. If the area of the distribution (number of the pixels occupied by a population) is smaller than the threshold, the population will go extinct.
	 * @return
	 */
    unsigned getGroupExtinctionThreshold();
    /**
	 * @brief The maximum time steps for a species to go extinct when its distribution is smaller than a threshold defined with SpeciesExtinctionThreshold or SpeciesExtinctionThreaholdPercentage
	 * @return
	 */
    unsigned getSpeciesExtinctionTimeSteps();
    /**
     * @brief get the length of time (number of time steps or duration) of a species which its distribution is smaller than the threshold defined with SpeciesExtinctionThreshold or SpeciesExtinctionThreaholdPercentage
     */
    unsigned getCurrentSpeciesExtinctionTimeSteps();
    /**
     * @brief get the maximum distribution (number of the pixels) of the species during the past simulation. This parameter is used to calculate the ratio of the current distribution with the maximum distribution. If the ratio is smaller than a threshold (SpeciesExtinctionThreaholdPercentage), it will be counted as an extinction.
     */
    unsigned getMaxSpeciesDistribution();
    /**
     * @brief get the maximum distribution (number of the pixels) of the species
     * @param distribution
     */
    void setMaxSpeciesDistribution(unsigned distribution);
    /**
     * @brief get the 'ratio' extinction threshold. The ratio if the current distribution/maximum distribution.
     * @return
     */
    double getSpeciesExtinctionThreaholdPercentage();
    /**
     * @brief A counter to save the current extinction time steps. If the distribution of the species touches the threshold of extinction, invoke this function to add the counter.
     */
    void addCurrentSpeciesExtinctionTimeSteps();
    /**
     * @brief set the counter of the current extinction time steps.
     * @param p_currentSpeciesExtinctionTimeSteps
     */
    void setCurrentSpeciesExtinctionTimeSteps(unsigned p_currentSpeciesExtinctionTimeSteps);
    /**
     * @brief return the dispersal method. 1: move to X directions. X is defined with NumOfPath. 2: move to all the potential directions.
     * @return
     */
    int getDispersalMethod();
    /**
     * @brief get the seed of the species.
     * @return
     */
    std::vector<GeoLocation*> getSeeds();
    /**
	 * @brief get the species ID
	 * @return
	 */
    unsigned getID();
    /**
	 * @brief get the potential directions for an individual to move.
	 * @return
	 */
    int getNumOfPath();
    /**
     * @brief return the niche breadths of the species
     * @return
     */
    std::vector<NicheBreadth*> getNicheBreadth();
    /**
     * @brief return the speciation year
     */
    unsigned getSpeciationYears();
    /**
     * @brief return the parent species of current species
     * @return
     */
    SpeciesObject* getParent();
    /**
     * @brief set the time step when the species goes extinct.
     * @param p_disappeared_year
     */
    void setDisappearedYear(unsigned p_disappeared_year);
    /**
     * @brief get the time step when the species goes extinct.
     */
    unsigned getDisappearedYear();
    /**
	 * @brief get the time step that the species appears.
	 */
    unsigned getAppearedYear();
    /**
     * @brief get all the children species
     * @return
     */
    std::vector<SpeciesObject*> getChildren();
    /**
     * @brief add a child species.
     * @param child
     */
    void addChild(SpeciesObject* child);
    /**
     * @brief get the tree of the species with its children species
     * @param isroot whether this species is a root species (the first species in the simulation)
     * @param iscolor generating a colorful tree or not.
     * @param p_year the first time step to generate the tree
     * @return return a speciation tree in NEXUS format.
     */
    std::string getNewickTree(bool isroot, bool iscolor, unsigned p_year);
    /**
     * @brief count the number of speciation/species extinction/clade extinction in this species and its children.
     * @param total_years
     */
    void markNode(unsigned total_years);
    /**
     * @brief get the clade extinction status of this species. 0: not extinct 1: extinct
     * @param status 0: unknown, 1: extincted, 2: unextincted 3: parent_extincted
     */
    void setCladeExtinctionStatus(unsigned status);
    /**
     * @brief set the parent clade status. 0: unknown, 1: extincted, 2: unextincted 3: parent_extincted
     */
    void markParentClade();
    /**
     * @brief detect all the extinct status of all the children of this species.
     * @param total_years if the last appear year of the species is total_years, it means the species doesn't go extinct, or it goes extinct.
     * @return
     */
    bool isAllLeafExtinction(unsigned total_years);
    /**
     * @brief return the number of clade extinction
     */
    unsigned getNumberOfCladeExtinction();
    /**
	 * @brief return the number of speciation
	 */
    unsigned getNumberOfSpeciation();
    /**
	 * @brief return the number of species extinction
	 */
    unsigned getNumberOfSpeciesExtinction();
    /**
     * @brief get the tree of the species with its children species in HTML format
     * @param p_year
     * @return
     */
    std::vector<std::string> getHTMLTree(unsigned p_year);
    /**
     * @brief get the string with CSV format which saves the number of speciation/clade extinction/species extinction.
     * @param isroot
     * @param total_years
     * @return
     */
    std::string getSpeciationExtinction(bool isroot, unsigned total_years);
    /**
     * @brief is a new species or the raw species from the configuration (JSON format)
     * @return TRUE or FALSE
     */
    bool isNewSpecies();
    /**
     * @brief set the species status.
     * @param p TRUE or FALSE
     */
    void setNewSpecies(bool p);
    /**
     * @brief get the species ID with its parent ID
     * @return
     */
    std::string getIDWithParentID();
};

#endif /* DEFINITIONS_SPECIESOBJECT_H_ */
