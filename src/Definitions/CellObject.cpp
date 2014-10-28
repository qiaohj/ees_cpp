/*
 * CellObject.cpp
 *
 *  Created on: Oct 25, 2014
 *      Author: qiaohj
 */

#include "CellObject.h"

CellObject::CellObject(unsigned p_year, unsigned p_x, unsigned p_y, SpeciesObject* p_species) {
    x = p_x;
    y = p_y;
    IndividualOrganism* individualOrganism = new IndividualOrganism(p_year, 1, p_species);
    individualOrganisms[individualOrganism->getSpeciesID()] = individualOrganism;
}
boost::unordered_map<unsigned, CellObject*> CellObject::run(unsigned p_year,
        vector<SparseMap*> p_current_environments, unsigned p_xsize,
        unsigned p_ysize) {
    boost::unordered_map<unsigned, CellObject*> new_cells;
    for (unsigned i = 0; i < individualOrganisms.size(); ++i) {
        IndividualOrganism* individualOrganism = individualOrganisms[i];
        //if current year no smaller than individual organism's next run year, then move this organism.
        if (p_year >= individualOrganism->getNextRunYear()) {
            vector<unsigned*> next_cells;
            switch (individualOrganism->getDispersalMethod()) {
            //only the new individual organisms can move
            case 1:
                ;
                break;
                //all the individual organisms can move
            case 2:
                next_cells = getDispersalMap_2(individualOrganism, p_xsize, p_ysize, p_current_environments);
                break;
            default:
                ;
            }
            for (unsigned i=0; i<next_cells.size(); ++i){
                if (new_cells.find(next_cells[i][1] * p_xsize + next_cells[i][0])==new_cells.end()){
                    CellObject* new_cell = new CellObject(p_year, next_cells[i][0], next_cells[i][1], individualOrganism->getSpecies());
                    new_cells[next_cells[i][1] * p_xsize + next_cells[i][0]] = new_cell;
                }else{
                    IndividualOrganism* new_individualOrganism = new IndividualOrganism(p_year, 1, individualOrganism->getSpecies());
                    new_individualOrganism->addNextRunYear();
                    new_cells[next_cells[i][1] * p_xsize + next_cells[i][0]]->addIndividualOrganism(new_individualOrganism);
                }

            }
        }
    }
    return new_cells;
}
void CellObject::addIndividualOrganism(IndividualOrganism* individualOrganism){
    if (individualOrganisms.find(individualOrganism->getSpeciesID())==individualOrganisms.end()){
        individualOrganisms[individualOrganism->getSpeciesID()] = individualOrganism;
    }else{
        individualOrganisms[individualOrganism->getSpeciesID()]->addCount();
        individualOrganisms[individualOrganism->getSpeciesID()]->setNextRunYear(individualOrganism->getNextRunYear());
    }
}

vector<unsigned*> CellObject::getDispersalMap_2(
        IndividualOrganism* individualOrganism, unsigned p_xsize,
        unsigned p_ysize, vector<SparseMap*> p_current_environments) {
    vector<unsigned*> new_cells;
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
                //                printf("Distance:%f\n", distance);
                if ((distance < individualOrganism->getDispersalAbility())
                        || (CommonFun::AlmostEqualRelative(distance,
                                individualOrganism->getDispersalAbility()))) {
                    if (individualOrganism->isSuitable(i_x, i_y, p_current_environments)){
                        unsigned v[2] = { i_x, i_y };
                        new_cells.push_back(v);
                    }
                }
            }
        }
    }
    return new_cells;
}
boost::unordered_map<unsigned, IndividualOrganism*> CellObject::getIndividualOrganisms(){
    return individualOrganisms;
}
void CellObject::merge(CellObject* p_cell){
    boost::unordered_map<unsigned, IndividualOrganism*> new_individualOrganisms = p_cell->getIndividualOrganisms();
    for (auto it : new_individualOrganisms){
        addIndividualOrganism(it.second);
    }
}
CellObject::unsigned getX(){
    return x;
}
CellObject::unsigned getY(){
    return y;
}
CellObject::~CellObject() {
    // TODO Auto-generated destructor stub
}

