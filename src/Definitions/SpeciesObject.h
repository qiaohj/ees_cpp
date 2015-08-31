/*
 * SpeciesObject.h
 *
 *  Created on: Oct 25, 2014
 *      Author: qiaohj
 */

#ifndef DEFINITIONS_SPECIESOBJECT_H_
#define DEFINITIONS_SPECIESOBJECT_H_

#include <string>
#include <vector>
#include "../JsonPaster/include/json/json.h"
#include "SparseMap.h"
#include "NicheBreadth.h"
#include "GeoLocation.h"

class SpeciesObject {
private:
    bool newSpecies;
    unsigned id;
    std::vector<NicheBreadth*> nicheBreadth;
    float* dispersalAbility;
    unsigned short dispersalAbilityLength;
    unsigned dispersalSpeed;
    unsigned speciationYears;
    int dispersalMethod;
    int numberOfPath;
    unsigned appearedYear;
    unsigned disappearedYear;
    float speciesExtinctionThreshold;
    float groupExtinctionThreshold;
    std::vector<GeoLocation*> seeds;
    SpeciesObject* parent;
    std::vector<SpeciesObject*> children;
    unsigned number_of_clade_extinction;
    unsigned number_of_speciation;
    unsigned number_of_species_extinction;
    //0: unknown, 1: extincted, 2: unextincted 3: parent_extincted
    unsigned clade_extinction_status;
public:
    SpeciesObject(const std::string json_path);
    SpeciesObject(unsigned p_id, SpeciesObject* p_parent, unsigned p_year);
    virtual ~SpeciesObject();
    float* getDispersalAbility();
    unsigned getDispersalSpeed();
    unsigned short getDispersalAbilityLength();
    float getSpeciesExtinctionThreshold();
    float getGroupExtinctionThreshold();
    int getDispersalMethod();
    std::vector<GeoLocation*> getSeeds();
    unsigned getID();
    int getNumOfPath();
    std::vector<NicheBreadth*> getNicheBreadth();
    unsigned getSpeciationYears();
    SpeciesObject* getParent();
    void setDisappearedYear(unsigned p_disappeared_year);
    unsigned getDisappearedYear();
    unsigned getAppearedYear();
    std::vector<SpeciesObject*> getChildren();
    void addChild(SpeciesObject* child);
    std::string getNewickTree(bool isroot, bool iscolor, unsigned p_year);
    void markNode(unsigned total_years);
    void setCladeExtinctionStatus(unsigned status);
    void markParentClade();
    bool isAllLeafExtinction(unsigned total_years);
    unsigned getNumberOfCladeExtinction();
    unsigned getNumberOfSpeciation();
    unsigned getNumberOfSpeciesExtinction();
    std::vector<std::string> getHTMLTree(unsigned p_year);
    std::string getSpeciationExtinction(bool isroot, unsigned total_years);
    bool isNewSpecies();
    void setNewSpecies(bool p);
    std::string getIDWithParentID();
};

#endif /* DEFINITIONS_SPECIESOBJECT_H_ */
