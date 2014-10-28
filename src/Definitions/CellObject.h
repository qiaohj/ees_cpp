/*
 * CellObject.h
 *
 *  Created on: Oct 25, 2014
 *      Author: qiaohj
 */

#ifndef DEFINITIONS_CELLOBJECT_H_
#define DEFINITIONS_CELLOBJECT_H_

#include <boost/unordered_map.hpp>
#include "IndividualOrganism.h"

class CellObject {
private:
    unsigned x;
    unsigned y;
    boost::unordered_map<unsigned, IndividualOrganism*> individualOrganisms;


    vector<unsigned*> getDispersalMap_2(IndividualOrganism* individualOrganism,
            unsigned p_xsize, unsigned p_ysize,
            vector<SparseMap*> p_current_environments);
public:
    CellObject(unsigned p_year, unsigned p_x, unsigned p_y,
            SpeciesObject* p_species);
    virtual ~CellObject();
    boost::unordered_map<unsigned, CellObject*> run(unsigned p_year,
            vector<SparseMap*> p_current_environments, unsigned p_xsize,
            unsigned p_ysize);
    void addIndividualOrganism(IndividualOrganism* individualOrganism);
    void merge(CellObject* p_cell);
    boost::unordered_map<unsigned, IndividualOrganism*> getIndividualOrganisms();
    unsigned getX();
    unsigned getY();
};

#endif /* DEFINITIONS_CELLOBJECT_H_ */
