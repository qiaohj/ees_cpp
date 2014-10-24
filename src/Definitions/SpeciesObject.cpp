/*
 * SpeciesObject.cpp
 *
 *  Created on: Oct 15, 2014
 *      Author: qiaohj
 */

#include "SpeciesObject.h"

SpeciesObject::SpeciesObject(Json::Value root, unsigned x_size, unsigned y_size,
        double* geoTrans) {
    nextRunYear = 0;
    id = root.get("id", "").asString();
    dispersalAbility = root.get("dispersal_ability", 1).asInt();
    dispersalSpeed = root.get("dispersal_speed", 1000).asInt();
    dispersalMethod = root.get("dispersal_method", 1).asInt();
    numberOfPath = root.get("number_of_path", -1).asInt();
    speciationYears = root.get("speciation_years", 10000).asInt();

    Json::Value niche_breadth_array = root["niche_breadth"];
//    nicheBreadth.reserve(niche_breadth_array.size());
    for (unsigned index = 0; index < niche_breadth_array.size(); ++index) {
        Json::Value niche_breadth_json = niche_breadth_array[index];
        float* niche_breadth = new float[2];
        for (unsigned i = 0; i < niche_breadth_json.size(); ++i) {
            niche_breadth[i] = niche_breadth_json[i].asFloat();
        }
        nicheBreadth.push_back(niche_breadth);
    }

    Json::Value initial_seeds_array = root["initial_seeds"];
//    seeds.reserve(initial_seeds_array.size());
    distributions[0] = new SparseMap(x_size, y_size);
    for (unsigned index = 0; index < initial_seeds_array.size(); ++index) {
        Json::Value initial_seeds_json = initial_seeds_array[index];
        float* initial_seed = new float[2];
        for (unsigned i = 0; i < initial_seeds_json.size(); ++i) {
            initial_seed[i] = initial_seeds_json[i].asFloat();
        }
        unsigned x, y;
        CommonFun::LL2XY(geoTrans, initial_seed[0], initial_seed[1], &x, &y);
        distributions[0]->setValue(x, y, 1);
        seeds.push_back(initial_seed);
    }

}

SpeciesObject::~SpeciesObject() {
    for (vector<float*>::iterator it = nicheBreadth.begin();
            it != nicheBreadth.end(); ++it) {
        delete[] *it;
    }

    for (vector<float*>::iterator it = seeds.begin(); it != seeds.end(); ++it) {
        delete[] *it;
    }
    vector<float*>().swap(nicheBreadth);
    vector<float*>().swap(seeds);
}
vector<SpeciesObject*> SpeciesObject::run(const unsigned current_year,
        const vector<SparseMap*> environmental_values, string p_target, double* geoTrans) {
    vector<SpeciesObject*> new_species;
    if (current_year >= nextRunYear) {
        SparseMap* prev_distribution = distributions[nextRunYear];
        SparseMap* dispersal_map = NULL;
        switch (dispersalMethod) {
        //only the new individual organisms can move
        case 1:
            ;
            break;
            //all the individual organisms can move
        case 2:
            dispersal_map = getDispersalMap_2(prev_distribution);
            break;
        default:
            ;
        }
        //get the suitable area
        vector<CellObject*> cells = dispersal_map->getValues();
        SparseMap* new_distribution = new SparseMap(dispersal_map->getXSize(),
                dispersal_map->getYSize());
        for (unsigned i = 0; i < cells.size(); ++i) {
            if (isSuitable(cells[i], environmental_values)){
                new_distribution->setValue(cells[i]->getX(), cells[i]->getY(), 1);
            }
        }

        nextRunYear += dispersalSpeed;
        writeDistribution(nextRunYear, new_distribution, p_target + "/" + id + "/dispersal", geoTrans);
        distributions[nextRunYear] = new_distribution;
    }
    new_species.push_back(this);
    return new_species;
}
void SpeciesObject::writeDistribution(unsigned year, SparseMap* distribution,
        string p_target, double* geoTrans) {
    CommonFun::createFolder(p_target.c_str());
    char tiffName[p_target.length() + 12];
    sprintf(tiffName, "%s/%s.tif", p_target.c_str(),
            CommonFun::fixedLength(year, 7).c_str());
    RasterController::writeGeoTIFF(tiffName, distribution->getXSize(),
            distribution->getYSize(), geoTrans, distribution->toArray(),
            (double) NODATA, GDT_Int32);

}
bool SpeciesObject::isSuitable(CellObject* cell,
        const vector<SparseMap*> environmental_values) {
    for (unsigned i = 0; i < nicheBreadth.size(); ++i) {
        int env_value = environmental_values[i]->readByXY(cell->getX(),
                cell->getY());
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
//get all the cells whose E-distances are not longer than dispersal ability.
SparseMap* SpeciesObject::getDispersalMap_2(SparseMap* prec_distribution) {
    vector<CellObject*> cells = prec_distribution->getValues();
    SparseMap* new_dispersal = new SparseMap(prec_distribution->getXSize(),
            prec_distribution->getYSize());
    for (unsigned i = 0; i < cells.size(); ++i) {
        for (unsigned x = (cells[i]->getX() - dispersalAbility);
                x <= (cells[i]->getX() + dispersalAbility); ++x) {
            x = (((int) x) < 0) ? 0 : x;
            if ((unsigned) x >= new_dispersal->getXSize())
                break;
            for (unsigned y = (cells[i]->getY() - dispersalAbility);
                    y <= (cells[i]->getY() + dispersalAbility); ++y) {
                y = (((int) y) < 0) ? 0 : y;
                if ((unsigned) y >= new_dispersal->getYSize())
                    break;

                double distance = CommonFun::EuclideanDistance((int)x,
                        (int)y, (int)(cells[i]->getX()), (int)(cells[i]->getY()));
//                printf("Distance:%f\n", distance);
                if (distance <= dispersalAbility) {
                    new_dispersal->setValue(x, y, 1);
                }
            }
        }
    }
    return new_dispersal;
}
unsigned SpeciesObject::getDispersalAbility() {
    return dispersalAbility;
}
unsigned SpeciesObject::getDispersalSpeed() {
    return dispersalSpeed;
}
string SpeciesObject::getID(){
    return id;
}
