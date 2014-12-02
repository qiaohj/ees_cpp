/*
 * IndividualOrganism.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: qiaohj
 */

#include "IndividualOrganism.h"

IndividualOrganism::IndividualOrganism(unsigned p_year,
        SpeciesObject* p_species, IndividualOrganism* p_parent, unsigned short p_x, unsigned short p_y) {
    species = p_species;
    year = p_year;
    parent = p_parent;
    x = p_x;
    y = p_y;
    groupId = 0;
    tempSpeciesID = 0;
    if (parent!=NULL){
        parent->addChild(this);
    }
}
void IndividualOrganism::setGroupId(unsigned short p_group_id){
    groupId = p_group_id;
}
unsigned short IndividualOrganism::getGroupId(){
    return groupId;
}
void IndividualOrganism::setTempSpeciesId(unsigned short p_species_id){
    tempSpeciesID = p_species_id;
}
unsigned IndividualOrganism::getTempSpeciesId(){
    return tempSpeciesID;
}
void IndividualOrganism::setSpecies(SpeciesObject* p_species){
    species = p_species;
}
void IndividualOrganism::setParent(IndividualOrganism* p_parent) {
    parent = p_parent;
}
IndividualOrganism* IndividualOrganism::getParent() {
    return parent;
}
void IndividualOrganism::removeChild(IndividualOrganism* child) {
    std::vector<IndividualOrganism*>::iterator iter = children.begin();
    while (iter != children.end()) {
        if (*iter == child) {
            iter = children.erase(iter);
        } else {
            ++iter;
        }
    }
}
void IndividualOrganism::clearChildren(){
    for (auto child : children){
        child->setParent(NULL);
    }
}
IndividualOrganism::~IndividualOrganism() {

}
unsigned IndividualOrganism::getNextRunYear() {
    return year + species->getDispersalSpeed();
}
int IndividualOrganism::getDispersalMethod() {
    return species->getDispersalMethod();
}
int IndividualOrganism::getNumOfPath() {
    return species->getNumOfPath();
}
unsigned IndividualOrganism::getDispersalAbility() {
    return species->getDispersalAbility();
}
unsigned IndividualOrganism::getSpeciationYears(){
    return species->getSpeciationYears();
}
void IndividualOrganism::addChild(IndividualOrganism* child){
    children.push_back(child);
}
bool IndividualOrganism::isSuitable(std::vector<SparseMap*>* p_current_environments) {
    std::vector<NicheBreadth*> nicheBreadth = species->getNicheBreadth();
    for (unsigned i = 0; i < nicheBreadth.size(); ++i) {
        int env_value = (*p_current_environments)[i]->readByXY(x, y);
        if (env_value==0){
            return false;
        }
        if (env_value == MATRIX_ZERO) {
            env_value = 0;
        }
        if ((env_value > nicheBreadth[i]->getMax())
                || (env_value < nicheBreadth[i]->getMin())) {
            return false;
        }
    }
    return true;
}
SpeciesObject* IndividualOrganism::getSpecies() {
    return species;
}
unsigned IndividualOrganism::getSpeciesID() {
    return species->getID();
}
unsigned IndividualOrganism::getYear(){
    return year;
}
unsigned short IndividualOrganism::getX(){
    return x;
}
unsigned short IndividualOrganism::getY(){
    return y;
}
void IndividualOrganism::setYear(unsigned p_year){
    year = p_year;
}
