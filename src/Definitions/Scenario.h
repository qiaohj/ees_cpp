/*
 * Scenario.h
 *
 *  Created on: Oct 25, 2014
 *      Author: qiaohj
 */

#ifndef DEFINITIONS_SCENARIO_H_
#define DEFINITIONS_SCENARIO_H_
#include <string>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include "../Universal/CommonFun.h"
#include "../JsonPaster/include/json/json.h"
#include "../Universal/CommonType.h"
#include "../RasterIO/RasterController.h"
#include "SparseMap.h"
#include "EnvironmentalHadley.h"
#include "SpeciesObject.h"
#include "IndividualOrganism.h"
#include "CoodLocation.h"
#include "../Universal/log.hpp"

class Scenario {
private:
	bool isFinished;
    unsigned tifLimit;
    unsigned long memLimit;
    std::vector<EnvironmentalHadley*> environments;
    std::vector<SpeciesObject*> species;
    unsigned totalYears;
    unsigned minSpeciesDispersalSpeed;
    SparseMap* mask;
//    hashmap_multiply environment_maps;
    std::string baseFolder;
    std::string target;
    double* geoTrans;
    unsigned xSize, ySize;
    void cleanActivedIndividualOrganisms();
    void cleanEnvironments();
    void cleanSpecies();
    void createSpeciesFolder(SpeciesObject* p_species);
    unsigned burnInYear;
    boost::unordered_map<unsigned, boost::unordered_map<SpeciesObject*,
            boost::unordered_map<unsigned, std::vector<IndividualOrganism*> > > > all_individualOrganisms;
    std::vector<CoodLocation*> getDispersalMap_2(
            IndividualOrganism* individualOrganism);
    IndividualOrganism* getUnmarkedOrganism(
            boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms);
    void markJointOrganism(unsigned short p_group_id,
            IndividualOrganism* p_unmarked_organism,
            boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms);
    unsigned getMinDividedYear(unsigned speciation_year, unsigned short group_id_1,
            unsigned short group_id_2, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms);
    unsigned getDividedYear(IndividualOrganism* o_1, IndividualOrganism* o_2);
    void markedSpeciesID(unsigned short group_id, unsigned short temp_species_id, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms);
    unsigned short getTempSpeciesID(unsigned short group_id, boost::unordered_map<unsigned, std::vector<IndividualOrganism*> >* organisms);
    std::string getSpeciesFolder(SpeciesObject* p_species);
    void generateSpeciationInfo(unsigned year, bool is_tree);
public:
    Scenario(const std::string p_scenario_json_path, std::string p_scenario_id, std::string p_base_folder,
            std::string p_target, unsigned p_tif_limit, unsigned long p_mem_limit);
    virtual ~Scenario();
    unsigned run();
    bool isFinish();
    float* getEnvironmentValue(unsigned p_year, double p_longitude,
            double p_latitude);
    std::vector<SparseMap*> getEnvironmenMap(unsigned p_year);
    std::string getTarget();
    //bool generateEnv();
};

#endif /* DEFINITIONS_SCENARIO_H_ */
