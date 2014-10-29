/*
 * CellObject.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: qiaohj
 */

#include "CellObject.h"

CellObject::CellObject(unsigned p_x, unsigned p_y) {
    x = p_x;
    y = p_y;
}
boost::unordered_map<unsigned, CellObject*> CellObject::run(unsigned p_year,
        std::vector<SparseMap*> p_current_environments, unsigned p_xsize,
        unsigned p_ysize) {
    //create a new group to save the new cells occupied by the new organisms move from this cell
    boost::unordered_map<unsigned, CellObject*> new_cells;
    for (unsigned i = 0; i < individualOrganisms.size(); ++i) {
        IndividualOrganism* individualOrganism = individualOrganisms[i];
        //if current year no smaller than individual organism's next run year, then move this organism.
        if (p_year >= individualOrganism->getNextRunYear()) {
            std::vector<CoodLocation*> next_cells;
            switch (individualOrganism->getDispersalMethod()) {
            //only the new individual organisms can move
            case 1:
                ;
                break;
                //all the individual organisms can move
            case 2:
                next_cells = getDispersalMap_2(individualOrganism, p_xsize,
                        p_ysize, p_current_environments);
                break;
            default:
                ;
            }
            for (auto it : next_cells) {
                unsigned index = it->getY() * p_xsize + it->getX();
                //create a new organism
                IndividualOrganism* new_individualOrganism =
                        new IndividualOrganism(p_year,
                                individualOrganism->getSpecies(),
                                individualOrganism);
                index = it->getY() * p_xsize + it->getX();
                if (new_cells.find(index) == new_cells.end()) {
                    CellObject* new_cell = new CellObject(it->getX(), it->getY());
                    new_cells[index] = new_cell;
                }
                new_cells[index]->addIndividualOrganism(
                        new_individualOrganism);
            }
            individualOrganism->addNextRunYear();
        }
    }
    return new_cells;
}
void CellObject::addIndividualOrganism(IndividualOrganism* individualOrganism) {
    individualOrganisms.push_back(individualOrganism);
}

std::vector<CoodLocation*> CellObject::getDispersalMap_2(
        IndividualOrganism* individualOrganism, unsigned p_xsize,
        unsigned p_ysize, std::vector<SparseMap*> p_current_environments) {
    std::vector<CoodLocation*> new_cells;

    //unfinished part

    //get all the cells whose E-distances are not longer than dispersal ability.
    //When number of path = 1, ignore the dispersal method parameter.
    if (individualOrganism->getNumOfPath() == -1) {
        for (unsigned i_x = (x - individualOrganism->getDispersalAbility());
                i_x <= (x + individualOrganism->getDispersalAbility()); ++i_x) {
            i_x = (((int) i_x) < 0) ? 0 : i_x;
            if ((unsigned) i_x >= p_xsize)
                break;
            for (unsigned i_y = (y - individualOrganism->getDispersalAbility());
                    i_y <= (y + individualOrganism->getDispersalAbility());
                    ++i_y) {
                i_y = (((int) i_y) < 0) ? 0 : i_y;
                if ((unsigned) i_y >= p_ysize)
                    break;

                double distance = CommonFun::EuclideanDistance((int) i_x,
                        (int) i_y, (int) (x), (int) (y));
                //printf("Distance:%f\n", distance);
                if ((distance < individualOrganism->getDispersalAbility())
                        || (CommonFun::AlmostEqualRelative(distance,
                                (double) individualOrganism->getDispersalAbility()))) {
                    if (individualOrganism->isSuitable(i_x, i_y,
                            p_current_environments)) {
                        CoodLocation* v = new CoodLocation( i_x, i_y );
                        new_cells.push_back(v);
                    }
                }
            }
        }
    }
    return new_cells;
}
std::vector<IndividualOrganism*> CellObject::getIndividualOrganisms() {
    return individualOrganisms;
}
void CellObject::merge(CellObject* p_cell) {
    std::vector<IndividualOrganism*> new_individualOrganisms =
            p_cell->getIndividualOrganisms();
    for (unsigned i = 0; i < new_individualOrganisms.size(); ++i) {
        addIndividualOrganism(new_individualOrganisms[i]);
    }
}
void CellObject::removeUnsuitable(
        std::vector<SparseMap*> p_current_environments) {
    for (int i = individualOrganisms.size() - 1; i >= 0; --i) {
        if (!individualOrganisms[i]->isSuitable(x, y, p_current_environments)) {
            individualOrganisms.erase(individualOrganisms.begin() + i);
        }
    }
}
unsigned CellObject::getX() {
    return x;
}
unsigned CellObject::getY() {
    return y;
}
CellObject::~CellObject() {
    // TODO Auto-generated destructor stub
}

