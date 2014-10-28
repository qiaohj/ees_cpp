/*
 * IndividualOrganism.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: qiaohj
 */

#include "IndividualOrganism.h"

IndividualOrganism::IndividualOrganism(unsigned p_year, unsigned p_count,
        SpeciesObject* p_species) {
    species = p_species;
    year = p_year;
    count = p_count;
    nextRunYear = 0;
}

IndividualOrganism::~IndividualOrganism() {
    // TODO Auto-generated destructor stub
}
unsigned IndividualOrganism::getNextRunYear() {
    return nextRunYear;
}
void IndividualOrganism::addNextRunYear() {
    nextRunYear += species->getDispersalSpeed();
}
void IndividualOrganism::setNextRunYear(unsigned p_next_run_year){
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
bool IndividualOrganism::isSuitable(unsigned p_x, unsigned p_y,
        vector<SparseMap*> p_current_environments) {
    vector<float*> nicheBreadth = species->getNicheBreadth();
    for (unsigned i = 0; i < nicheBreadth.size(); ++i) {
        int env_value = p_current_environments[i]->readByXY(p_x, p_y);
        if (env_value == MATRIX_ZERO) {
            env_value = 0;
        }
        if ((env_value > nicheBreadth[i][1])
                || (env_value < nicheBreadth[i][0])) {
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
void IndividualOrganism::addCount() {
    count++;
}
