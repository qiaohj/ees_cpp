/*
 * SpeciesObject.h
 *
 *  Created on: Oct 15, 2014
 *      Author: qiaohj
 */

#ifndef SpeciesObject_H
#define SpeciesObject_H

#include <string>
#include "../JsonPaster/include/json/json.h"
#include "../Definitions/SparseMap.h"
#include "../RasterIO/RasterController.h"
#include "../Universal/CommonType.h"

using namespace std;

class SpeciesObject {
private:
    unsigned id;
    vector<float*> nicheBreadth;
    unsigned dispersalAbility;
    unsigned dispersalSpeed;
    unsigned nextRunYear;
    unsigned speciationYears;
    int dispersalMethod;
    int numberOfPath;
    vector<float*> seeds;
    hashmap_single distributions;
    bool isSuitable(CellObject* p_cell,
            const vector<SparseMap*> p_environmental_values);
    SparseMap* getDispersalMap_2(SparseMap* p_prec_distribution);
    void writeDistribution(unsigned year, SparseMap* p_distribution,
            string p_target, double* p_geoTrans);
public:
    SpeciesObject(Json::Value p_root, unsigned p_x_size, unsigned p_y_size,
            double* p_geo_trans);
    virtual ~SpeciesObject();
    unsigned getDispersalAbility();
    unsigned getDispersalSpeed();
    vector<SpeciesObject*> run(const unsigned p_current_year,
            const vector<SparseMap*> p_environmental_values, string p_target,
            double* p_geo_trans);
    unsigned getID();
};

#endif /* SpeciesObject_H */
