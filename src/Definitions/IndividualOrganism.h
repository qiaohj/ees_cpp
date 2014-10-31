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
    unsigned x;
    unsigned y;
    SpeciesObject* species;
    unsigned year;
    unsigned nextRunYear;
    IndividualOrganism* parent;

public:
    IndividualOrganism(unsigned p_year, SpeciesObject* p_species,
            IndividualOrganism* p_parent, unsigned p_x, unsigned p_y);
    virtual ~IndividualOrganism();
    unsigned getNextRunYear();
    void addNextRunYear();
    void setNextRunYear(unsigned p_next_run_year);
    int getDispersalMethod();
    int getNumOfPath();
    unsigned getDispersalAbility();
    unsigned getSpeciationYears();
    bool isSuitable(std::vector<SparseMap*> p_current_environments);
    SpeciesObject* getSpecies();
    unsigned getSpeciesID();
    void setParent(IndividualOrganism* p_parent);
    IndividualOrganism* getParent();
    unsigned getYear();
    unsigned getX();
    unsigned getY();
};

#endif /* DEFINITIONS_INDIVIDUALORGANISM_H_ */
