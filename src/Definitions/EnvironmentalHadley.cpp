/*
 * EnvironmentalHadley.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: qiaohj
 */

#include "EnvironmentalHadley.h"

EnvironmentalHadley::EnvironmentalHadley(const std::string p_basefolder, int p_burn_in_year,
		int p_begin_year, int p_end_year, int p_step) {

	for (int y=p_begin_year + p_burn_in_year; y<=p_end_year; y -= p_step){
		std::string layer = p_basefolder + "/"
					+  CommonFun::fixedLength(y, 4) + ".tif";
		if (y>p_begin_year){
			layer = p_basefolder + "/"
					+  CommonFun::fixedLength(p_begin_year, 4) + ".tif";
		}
		RasterObject* r = new RasterObject(layer);
		layers[y] = r;
		this->burnInYears = p_burn_in_year;
	}
}
unsigned EnvironmentalHadley::getBurnInYears() {
	return burnInYears;
}
SparseMap* EnvironmentalHadley::getValues(unsigned p_year) {

	SparseMap* v = new SparseMap(layers[p_year], false);

	return v;
}

float EnvironmentalHadley::readByXY(unsigned p_year,
		unsigned p_x, unsigned p_y) {
	float value = layers[p_year]->readByXY(p_x, p_y);
	return value;
}
float EnvironmentalHadley::readByLL(unsigned p_year,
		double p_longitude, double p_latitude) {
	float value = layers[p_year]->readByLL(p_longitude, p_latitude);
	return value;
}
EnvironmentalHadley::~EnvironmentalHadley() {

}

