/*
 * EnvironmentalHadley.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: qiaohj
 */

#include "EnvironmentalHadley.h"

EnvironmentalHadley::EnvironmentalHadley(const std::string p_basefolder, double* p_geoTrans, unsigned p_burn_in_year,
		unsigned p_begin_year, unsigned p_end_year, unsigned p_step) {
	geoTrans = p_geoTrans;
	for (unsigned y=p_begin_year + p_burn_in_year; y>=p_end_year; y -= p_step){
		unsigned year = y / 100;
		std::string layer = p_basefolder + "/"
					+  CommonFun::fixedLength(year, 4) + ".tif";
		if (y>p_begin_year){
			layer = p_basefolder + "/"
					+  CommonFun::fixedLength(p_begin_year / 100, 4) + ".tif";
		}

		//RasterObject* r = new RasterObject(layer);
		unsigned key = p_begin_year + p_burn_in_year - y;
		RasterObject* r = new RasterObject(layer);
		SparseMap* v = new SparseMap(r, false);
		//LOG(INFO)<<"Initial environments information from "<<layer<<" to key "<<key;
		layers[key] = v;
		delete r;
		if (y==0){
			break;
		}
	}
	this->burnInYears = p_burn_in_year;
}
unsigned EnvironmentalHadley::getBurnInYears() {
	return burnInYears;
}
SparseMap* EnvironmentalHadley::getValues(unsigned p_year) {
//	LOG(INFO)<<"get:"<<p_year;
//	LOG(INFO)<<" from "<<layers[p_year]->getFilename();

	return layers[p_year];
}

float EnvironmentalHadley::readByXY(unsigned p_year,
		unsigned p_x, unsigned p_y) {

	float value = layers[p_year]->readByXY(p_x, p_y);
	return value;
}
float EnvironmentalHadley::readByLL(unsigned p_year,
		double p_longitude, double p_latitude) {
	float value = layers[p_year]->readByLL(this->geoTrans, p_longitude, p_latitude);
	return value;
}
EnvironmentalHadley::~EnvironmentalHadley() {

}

