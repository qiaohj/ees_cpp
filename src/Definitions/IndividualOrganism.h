/*
 * IndividualOrganism.h
 *
 *  Created on: Oct 24, 2014
 *      Author: qiaohj
 */

#ifndef DEFINITIONS_INDIVIDUALORGANISM_H_
#define DEFINITIONS_INDIVIDUALORGANISM_H_

#include <boost/unordered_map.hpp>
#include "SpeciesObject.h"
#include "SparseMap.h"
#include "../Universal/log.hpp"

class IndividualOrganism {
private:
    unsigned short x;
    unsigned short y;
    unsigned short groupId;
    unsigned short tempSpeciesID;
    SpeciesObject* species;
    unsigned year;
    boost::unordered::unordered_map<unsigned, unsigned> parentIds;
//    std::vector<IndividualOrganism*> children;

public:
    IndividualOrganism(unsigned p_year, SpeciesObject* p_species,
            boost::unordered::unordered_map<unsigned, unsigned> p_parent_ids, unsigned short p_x, unsigned short p_y);
    virtual ~IndividualOrganism();
    int getDispersalMethod();
    int getNumOfPath();
    unsigned getDispersalAbility();
    unsigned getSpeciationYears();
    bool isSuitable(std::vector<SparseMap*>* p_current_environments);
    SpeciesObject* getSpecies();
    unsigned getSpeciesID();
    void addParentId(unsigned year, unsigned group_id);
    boost::unordered::unordered_map<unsigned, unsigned> getParentIds();
//    void setParent(IndividualOrganism* p_parent);
//    IndividualOrganism* getParent();
    unsigned getYear();
    unsigned short getX();
    unsigned short getY();
    void setYear(unsigned p_year);
    void setGroupId(unsigned short p_group_id);
    unsigned short getGroupId();
    void setTempSpeciesId(unsigned short p_species_id);
    unsigned getTempSpeciesId();
    void setSpecies(SpeciesObject* p_species);
    void removeParentYear(unsigned parent_year);
    unsigned long getMemoryUsage();
//    void addChild(IndividualOrganism* child);
//    void clearChildren();
//    void removeChild(IndividualOrganism* child);
};

#endif /* DEFINITIONS_INDIVIDUALORGANISM_H_ */
