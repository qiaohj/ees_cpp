/*
 * IndividualOrganism.h
 *
 *  Created on: Oct 24, 2014
 *      Author: qiaohj
 */

#ifndef DEFINITIONS_INDIVIDUALORGANISM_H_
#define DEFINITIONS_INDIVIDUALORGANISM_H_

#include "SpeciesObject.h"
#include "SparseMap.h"
#include "../Universal/log.hpp"

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
    //void setDispersalAbility(unsigned short p_dispersal_ability);
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
