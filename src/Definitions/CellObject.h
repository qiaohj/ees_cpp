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
#include "CoodLocation.h"

class CellObject {
private:
    unsigned x;
    unsigned y;
    std::vector<IndividualOrganism*> individualOrganisms;


    std::vector<CoodLocation*> getDispersalMap_2(IndividualOrganism* individualOrganism,
            unsigned p_xsize, unsigned p_ysize,
            std::vector<SparseMap*> p_current_environments);
public:
    CellObject(unsigned p_x, unsigned p_y);
    virtual ~CellObject();
    boost::unordered_map<unsigned, CellObject*> run(unsigned p_year,
            std::vector<SparseMap*> p_current_environments, unsigned p_xsize,
            unsigned p_ysize);
    void addIndividualOrganism(IndividualOrganism* individualOrganism);
    void merge(CellObject* p_cell);
    std::vector<IndividualOrganism*> getIndividualOrganisms();
    unsigned getX();
    unsigned getY();
    void removeUnsuitable(std::vector<SparseMap*> p_current_environments);
};

#endif /* DEFINITIONS_CELLOBJECT_H_ */
