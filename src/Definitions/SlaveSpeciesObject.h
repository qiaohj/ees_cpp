/*
 * SlaveSpeciesObject.h
 *
 *  Created on: Oct 15, 2014
 *      Author: qiaohj
 */
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <string>
#include "../JsonPaster/include/json/json.h"

using namespace std;

class SlaveSpeciesObject {
private:
	string id;
	vector<float*> nicheBreadth;
	int dispersalAbility;
	int dispersalSpeed;
	int dispersalMethod;
	int numberOfPath;
	vector<float*> initialSeeds;
	boost::numeric::ublas::mapped_matrix<bool> distribution_matrix;
public:
	SlaveSpeciesObject(Json::Value root);
	virtual ~SlaveSpeciesObject();
};

