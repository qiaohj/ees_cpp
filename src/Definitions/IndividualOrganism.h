/*
 * IndividualOrganism.h
 *
 *  Created on: Oct 24, 2014
 *      Author: qiaohj
 */

#ifndef DEFINITIONS_INDIVIDUALORGANISM_H_
#define DEFINITIONS_INDIVIDUALORGANISM_H_

#include "SpeciesObject.h"

class IndividualOrganism {
private:
    SpeciesObject* species;
    unsigned year;
    unsigned count;
    unsigned nextRunYear;

public:
    IndividualOrganism(unsigned p_year, unsigned p_count,
            SpeciesObject* p_species);
    virtual ~IndividualOrganism();
    unsigned getNextRunYear();
    void addNextRunYear();
    void setNextRunYear(unsigned p_next_run_year);
    int getDispersalMethod();
    int getNumOfPath();
    unsigned getDispersalAbility();
    bool isSuitable(unsigned p_x, unsigned p_y,
            vector<SparseMap*> p_current_environments);
    SpeciesObject* getSpecies();
    unsigned getSpeciesID();
    void addCount();
};

#endif /* DEFINITIONS_INDIVIDUALORGANISM_H_ */
