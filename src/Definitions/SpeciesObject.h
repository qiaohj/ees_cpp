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
    //0: unknown, 1: extincted, 2: unextincted 3: parent_extincted
    unsigned clade_extinction_status;
public:
    SpeciesObject(const std::string json_path);
    SpeciesObject(unsigned p_id, SpeciesObject* p_parent, unsigned p_year);
    virtual ~SpeciesObject();
    double* getDispersalAbility();
    unsigned getDispersalSpeed();
    unsigned short getDispersalAbilityLength();
    unsigned getSpeciesExtinctionThreshold();
    unsigned getGroupExtinctionThreshold();
    unsigned getSpeciesExtinctionTimeSteps();
    unsigned getCurrentSpeciesExtinctionTimeSteps();
    unsigned getMaxSpeciesDistribution();
    void setMaxSpeciesDistribution(unsigned distribution);
    double getSpeciesExtinctionThreaholdPercentage();
    void addCurrentSpeciesExtinctionTimeSteps();
    void setCurrentSpeciesExtinctionTimeSteps(unsigned p_currentSpeciesExtinctionTimeSteps);
    int getDispersalMethod();
    std::vector<GeoLocation*> getSeeds();
    unsigned getID();
    int getNumOfPath();
    std::vector<NicheBreadth*> getNicheBreadth();
    unsigned getSpeciationYears();
    SpeciesObject* getParent();
    void setDisappearedYear(unsigned p_disappeared_year);
    unsigned getDisappearedYear();
    unsigned getAppearedYear();
    std::vector<SpeciesObject*> getChildren();
    void addChild(SpeciesObject* child);
    std::string getNewickTree(bool isroot, bool iscolor, unsigned p_year);
    void markNode(unsigned total_years);
    void setCladeExtinctionStatus(unsigned status);
    void markParentClade();
    bool isAllLeafExtinction(unsigned total_years);
    unsigned getNumberOfCladeExtinction();
    unsigned getNumberOfSpeciation();
    unsigned getNumberOfSpeciesExtinction();
    std::vector<std::string> getHTMLTree(unsigned p_year);
    std::string getSpeciationExtinction(bool isroot, unsigned total_years);
    bool isNewSpecies();
    void setNewSpecies(bool p);
    std::string getIDWithParentID();
};

#endif /* DEFINITIONS_SPECIESOBJECT_H_ */
