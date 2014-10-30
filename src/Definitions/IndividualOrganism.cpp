/*
 * IndividualOrganism.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: qiaohj
 */

#include "IndividualOrganism.h"

IndividualOrganism::IndividualOrganism(unsigned p_year,
        SpeciesObject* p_species, IndividualOrganism* p_parent) {
    species = p_species;
    year = p_year;
    if (p_parent==NULL){
        nextRunYear = 0;
    }else{
        nextRunYear = p_parent->getNextRunYear() + p_species->getDispersalSpeed();
    }
    parent = p_parent;
    active = true;
}
void IndividualOrganism::setParent(IndividualOrganism* p_parent) {
    parent = p_parent;
}
IndividualOrganism* IndividualOrganism::getParent() {
    return parent;
}
IndividualOrganism::~IndividualOrganism() {
}
unsigned IndividualOrganism::getNextRunYear() {
    return nextRunYear;
}
void IndividualOrganism::addNextRunYear() {
    nextRunYear += species->getDispersalSpeed();
}
void IndividualOrganism::setNextRunYear(unsigned p_next_run_year) {
    nextRunYear = p_next_run_year;
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
bool IndividualOrganism::isSuitable(unsigned p_x, unsigned p_y,
        std::vector<SparseMap*> p_current_environments) {
    std::vector<NicheBreadth*> nicheBreadth = species->getNicheBreadth();
    for (unsigned i = 0; i < nicheBreadth.size(); ++i) {
        int env_value = p_current_environments[i]->readByXY(p_x, p_y);
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
void IndividualOrganism::setActive(bool p_active){
    active = p_active;
}
bool IndividualOrganism::isActive(){
    return active;
}
unsigned IndividualOrganism::getYear(){
    return year;
}
