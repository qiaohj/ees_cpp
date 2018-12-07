/**
 * @file IndividualOrganism.cpp
 * @brief Class IndividualOrganism. A class to handle the behavior of an individual in the simulation
 * @author Huijie Qiao
 * @version 1.0
 * @date 11/25/2018
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
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
//void IndividualOrganism::removeChild(IndividualOrganism* child) {
//    std::vector<IndividualOrganism*>::iterator iter = children.begin();
//    while (iter != children.end()) {
//        if (*iter == child) {
//            iter = children.erase(iter);
//        } else {
//            ++iter;
//        }
//    }
//}
//void IndividualOrganism::clearChildren(){
//    for (auto child : children){
//        child->setParent(NULL);
//    }
//}
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
unsigned short IndividualOrganism::getDispersalAbility() {
    return dispersalAbility;
}
void IndividualOrganism::setRandomDispersalAbility(){

	double r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
	unsigned short dispersal_ability = 1;
	for (unsigned short i = 1; i <= species->getDispersalAbilityLength(); i++) {
		if (r <= species->getDispersalAbility()[i-1]) {
			dispersal_ability = i - 1;
			break;
		}
	}
	//LOG(INFO)<<"Dispersal ability is " <<dispersal_ability << " calculated by "<<r;
	dispersalAbility = dispersal_ability;
	//dispersalAbility = 1;
}

void IndividualOrganism::setDispersalAbility(unsigned short p_dispersal_ability) {
    dispersalAbility = p_dispersal_ability;
}
unsigned IndividualOrganism::getSpeciationYears(){
    return species->getSpeciationYears();
}
//void IndividualOrganism::addChild(IndividualOrganism* child){
//    children.push_back(child);
//}
bool IndividualOrganism::isSuitable(std::vector<SparseMap*>* p_current_environments) {
    std::vector<NicheBreadth*> nicheBreadth = species->getNicheBreadth();
    for (unsigned i = 0; i < nicheBreadth.size(); ++i) {
    	//LOG(INFO)<<"Environments:"<<i<<" Size:"<<(*p_current_environments).size()<<" Address:"<<(*p_current_environments)[i];
        int env_value = (*p_current_environments)[i]->readByXY(x, y);
        //LOG(INFO)<<env_value<<"FALSE";
        if ((env_value > nicheBreadth[i]->getMax())
                || (env_value < nicheBreadth[i]->getMin())) {
            return false;
        }
    }

    //remove the areas covered by the ice sheet randomly based on the ice sheet values.
    if (((*p_current_environments).size()-1)==nicheBreadth.size()){
		int icesheet = (*p_current_environments)[nicheBreadth.size()]->readByXY(x, y);
		double r = static_cast<double>(rand()) * 1000 / static_cast<double>(RAND_MAX);
		//LOG(INFO)<<"icesheet is "<<icesheet<<" and random value is "<<r;
		if (icesheet>=r){
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
unsigned long IndividualOrganism::getMemoryUsage(){
	unsigned long mem = 0;
	mem += sizeof(unsigned short) * 4;
	mem += sizeof(unsigned);
	mem += sizeof(SpeciesObject*);
	mem += sizeof(std::vector<IndividualOrganism*>);
//	mem += sizeof(IndividualOrganism*) * children.size();
	return mem;
}
