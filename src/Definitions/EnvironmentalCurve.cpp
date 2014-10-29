/*
 * EnvironmentalCurve.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: qiaohj
 */


#include "EnvironmentalCurve.h"


EnvironmentalCurve::EnvironmentalCurve(Json::Value p_root) {
    glacial = new RasterObject(p_root.get("glacial_path", "").asCString());
    interglacial = new RasterObject(p_root.get("interglacial_path", "").asCString());
    noData = glacial->getNoData();
    part_1_years = p_root.get("part_1_years", 10000).asInt();
    part_2_years = p_root.get("part_2_years", 10000).asInt();
    plateau_1_years = p_root.get("plateau_1_years", 10000).asInt();
    plateau_2_years = p_root.get("plateau_2_years", 10000).asInt();
    burn_in_curve = p_root.get("burn_in_curve", "1").asString();
    part_1_curve = p_root.get("part_1_curve", "1").asString();
    part_2_curve = p_root.get("part_2_curve", "1").asString();
    plateau_1_curve = p_root.get("plateau_1_curve", "1").asString();
    plateau_2_curve = p_root.get("plateau_2_curve", "1").asString();
    burnInYears = p_root.get("burn_in_years", 50000).asInt();
    cycleYears = part_1_years + part_2_years + plateau_1_years + plateau_2_years;
}
int EnvironmentalCurve::getBurnInYears(){
    return burnInYears;
}
SparseMap* EnvironmentalCurve::getValues(unsigned p_year){
    float curve_value = getCurveValue(p_year);
    mapvalue* value = new mapvalue(glacial->getXSize(), glacial->getYSize(), glacial->getXSize() * glacial->getYSize());
    for (unsigned x=0; x<glacial->getXSize(); ++x){
        for (unsigned y=0; y<glacial->getYSize(); ++y){
            float v = readByXY(curve_value, p_year, x, y);
            if (!CommonFun::AlmostEqualRelative(v, noData)){
                if (CommonFun::AlmostEqualRelative(v, (float)0)){
                    v = MATRIX_ZERO;
                }
                (*value)(x, y) = (int) v;
            }
        }
    }
    return new SparseMap(value);
}
float EnvironmentalCurve::getCurveValue(unsigned p_year){
    float curve_value = 0;
    Parser prs;
    if (p_year<=burnInYears){
        curve_value = prs.parse(burn_in_curve.c_str());
    }else{
        unsigned x_pos = ((p_year-burnInYears) % cycleYears);
        std::string curve_func = "";
        float x_value = 0;

        if (x_pos<=part_1_years){
            x_value = ((float)x_pos) / ((float)part_1_years);
            curve_func = part_1_curve;
        }else if ((x_pos>part_1_years)&&(x_pos<=(part_1_years + plateau_1_years))){
            x_value = ((float)(x_pos - part_1_years)) / ((float)plateau_1_years);
            curve_func = plateau_1_curve;
        }else if ((x_pos>(part_1_years + plateau_1_years))&&(x_pos<=(part_1_years + plateau_1_years + part_2_years))){
            x_value = ((float)(x_pos - part_1_years - plateau_1_years)) / ((float)part_2_years);
            curve_func = part_2_curve;
        }else{
            x_value = ((float)(x_pos - part_1_years - plateau_1_years - part_2_years)) / ((float)plateau_2_years);
            curve_func = plateau_2_curve;
        }
        boost::replace_all(curve_func, "x", CommonFun::to_str(x_value));
//        printf("Function is %s\n", curve_func.c_str());
        curve_value = prs.parse(curve_func.c_str());

    }
    return curve_value;
}
float EnvironmentalCurve::readByXY(float p_curve_value, unsigned p_year, unsigned p_x, unsigned p_y){
    float interglacial_value = interglacial->readByXY(p_x, p_y);
    float glacial_value =  glacial->readByXY(p_x, p_y);
    float value = (interglacial_value - glacial_value) * p_curve_value + glacial_value;
    return value;
}
float EnvironmentalCurve::readByLL(float p_curve_value, unsigned p_year, double p_longitude, double p_latitude){
    if (CommonFun::AlmostEqualRelative(p_curve_value, (float)-1)){
        p_curve_value = getCurveValue(p_year);
    }
    float interglacial_value = interglacial->readByLL(p_longitude, p_latitude);
    float glacial_value =  glacial->readByLL(p_longitude, p_latitude);
    float value = (interglacial_value - glacial_value) * p_curve_value + glacial_value;
    return value;
}
EnvironmentalCurve::~EnvironmentalCurve() {
    delete glacial;
    delete interglacial;
}

