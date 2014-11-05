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
#include "../Universal/CommonFun.h"
#include "../JsonPaster/include/json/json.h"
#include "../Universal/CommonType.h"
#include "../RasterIO/RasterController.h"
#include "SparseMap.h"
#include "EnvironmentalCurve.h"
#include "SpeciesObject.h"
#include "IndividualOrganism.h"
#include "CoodLocation.h"
#include "../Universal/log.hpp"

class Scenario {
private:
    std::vector<EnvironmentalCurve*> environments;
    std::vector<SpeciesObject*> species;
    unsigned totalYears;
    unsigned minSpeciesDispersalSpeed;
    SparseMap* mask;
    hashmap_multiply environment_maps;
    std::string baseFolder;
    std::string target;
    double* geoTrans;
    unsigned xSize, ySize;
    void cleanActivedIndividualOrganisms();
    void cleanEnvironments();
    void cleanSpecies();
    void createSpeciesFolder(unsigned p_species_id);
    unsigned burnInYear;
    boost::unordered_map<unsigned, boost::unordered_map<unsigned, IndividualOrganism*>* > actived_individualOrganisms;
    boost::unordered_map<unsigned, std::vector<IndividualOrganism*>* > all_individualOrganisms;
    std::vector<CoodLocation*> getDispersalMap_2(
            IndividualOrganism* individualOrganism, std::vector<SparseMap*> p_current_environments);
public:
    Scenario(Json::Value p_root, std::string p_base_folder, std::string p_target);
    virtual ~Scenario();
    void run();
    float* getEnvironmentValue(unsigned p_year, double p_longitude, double p_latitude);
    std::vector<SparseMap*> getEnvironmenMap(unsigned p_year);
};

#endif /* DEFINITIONS_SCENARIO_H_ */
