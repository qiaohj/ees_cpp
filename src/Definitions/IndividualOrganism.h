/**
 * @file IndividualOrganism.h
 * @brief Class IndividualOrganism. A class to handle the behavior of an individual in the simulation.
 * @author Huijie Qiao
 * @version 1.0
 * @date 11/25/2018
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#ifndef DEFINITIONS_INDIVIDUALORGANISM_H_
#define DEFINITIONS_INDIVIDUALORGANISM_H_

#include "SpeciesObject.h"
#include "SparseMap.h"
#include "../Universal/log.hpp"
/**
 * @brief A class to handle the behavior of an individual in the simulation.
 */
class IndividualOrganism {
private:
    unsigned short x;
    unsigned short y;
    unsigned short groupId;
    unsigned short tempSpeciesID;
    unsigned short dispersalAbility;
    SpeciesObject* species;
    unsigned year;
    IndividualOrganism* parent;
//    std::vector<IndividualOrganism*> children;

public:
    IndividualOrganism(unsigned p_year, SpeciesObject* p_species,
            IndividualOrganism* p_parent, unsigned short p_x, unsigned short p_y);
    virtual ~IndividualOrganism();
    unsigned getNextRunYear();
    int getDispersalMethod();
    int getNumOfPath();
    unsigned getSpeciationYears();
    unsigned short getDispersalAbility();
    void setDispersalAbility(unsigned short p_dispersal_ability);
    bool isSuitable(std::vector<SparseMap*>* p_current_environments);
    SpeciesObject* getSpecies();
    unsigned getSpeciesID();
    void setParent(IndividualOrganism* p_parent);
    void setRandomDispersalAbility();
    IndividualOrganism* getParent();
    unsigned getYear();
    unsigned short getX();
    unsigned short getY();
    void setYear(unsigned p_year);
    void setGroupId(unsigned short p_group_id);
    unsigned short getGroupId();
    void setTempSpeciesId(unsigned short p_species_id);
    unsigned getTempSpeciesId();
    void setSpecies(SpeciesObject* p_species);
//    void addChild(IndividualOrganism* child);
//    void clearChildren();
//    void removeChild(IndividualOrganism* child);
    unsigned long getMemoryUsage();
};

#endif /* DEFINITIONS_INDIVIDUALORGANISM_H_ */
