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
#include "../RasterIO/SparseMap.h"
#include "../RasterIO/RasterController.h"
#include "../Universal/CommonType.h"

using namespace std;

class SpeciesObject {
private:
    string id;
    vector<float*> nicheBreadth;
    unsigned dispersalAbility;
    unsigned dispersalSpeed;
    unsigned nextRunYear;
    unsigned speciationYears;
    int dispersalMethod;
    int numberOfPath;
    vector<float*> seeds;
    hashmap_single distributions;
    bool isSuitable(CellObject* cell,
            const vector<SparseMap*> environmental_values);
    SparseMap* getDispersalMap_2(SparseMap* prec_distribution);
    void writeDistribution(unsigned year, SparseMap* distribution,
            string base_folder, double* geoTrans);
public:
    SpeciesObject(Json::Value root, unsigned x_size, unsigned y_size,
            double* geoTrans);
    virtual ~SpeciesObject();
    unsigned getDispersalAbility();
    unsigned getDispersalSpeed();
    vector<SpeciesObject*> run(const unsigned current_year,
            const vector<SparseMap*> environmental_values, string p_base_folder,
            double* geoTrans);
};

#endif /* SpeciesObject_H */
