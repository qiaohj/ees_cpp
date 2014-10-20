/*
 * EnvironmentalCurve.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: qiaohj
 */


#include "EnvironmentalCurve.h"


EnvironmentalCurve::EnvironmentalCurve(Json::Value root) {
    glacial = new RasterObject(root.get("glacial_path", "").asCString());
    interglacial = new RasterObject(root.get("interglacial_path", "").asCString());
    part_1_years = root.get("part_1_years", 10000).asInt();
    part_2_years = root.get("part_2_years", 10000).asInt();
    plateau_1_years = root.get("plateau_1_years", 10000).asInt();
    plateau_2_years = root.get("plateau_2_years", 10000).asInt();
    burn_in_curve = root.get("burn_in_curve", "1").asString();
    part_1_curve = root.get("part_1_curve", "1").asString();
    part_2_curve = root.get("part_2_curve", "1").asString();
    plateau_1_curve = root.get("plateau_1_curve", "1").asString();
    plateau_2_curve = root.get("plateau_2_curve", "1").asString();
    burnInYears = root.get("burn_in_years", 50000).asInt();
    cycleYears = part_1_years + part_2_years + plateau_1_years + plateau_2_years;
}
int EnvironmentalCurve::getBurnInYears(){
    return burnInYears;
}
float EnvironmentalCurve::getValue(unsigned year, double longitude, double latitude){
    float curve_value = 0;
    Parser prs;
    if (year<=burnInYears){
        curve_value = prs.parse(burn_in_curve.c_str());
    }else{
        unsigned x_pos = ((year-burnInYears) % cycleYears);
        string curve_func = "";
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
        boost::replace_all(curve_func, "x", to_str(x_value));
//        printf("Function is %s\n", curve_func.c_str());
        curve_value = prs.parse(curve_func.c_str());

    }
    float interglacial_value = interglacial->readByLL(longitude, latitude);
    float glacial_value =  glacial->readByLL(longitude, latitude);
    float value = (interglacial_value - glacial_value) * curve_value + glacial_value;
    return (float) value;
}
EnvironmentalCurve::~EnvironmentalCurve() {
    delete glacial;
    delete interglacial;
}

