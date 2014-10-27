/*
 * CellObject.cpp
 *
 *  Created on: Oct 19, 2014
 *      Author: qiaohj
 */

#include "CellObject.h"

CellObject::CellObject(unsigned p_x, unsigned p_y, int p_value){
    x = p_x;
    y = p_y;
    value = p_value;
}
void CellObject::clearCellObject(vector<CellObject*> p_v){
    for (vector<CellObject*>::iterator it = p_v.begin();
        it != p_v.end(); ++it) {
        delete *it;
    }
    vector<CellObject*>().swap(p_v);
}
CellObject::~CellObject(){
}
unsigned CellObject::getX(){
    return x;
}
unsigned CellObject::getY(){
    return y;
}
int CellObject::getValue(){
    return value;
}
void CellObject::addIndividualOrganisms(IndividualOrganism p_individual_organism){
    individualOrganisms.push_back(p_individual_organism);
}



