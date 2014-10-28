/*
 * CellObject.h
 *
 *  Created on: Oct 19, 2014
 *      Author: qiaohj
 */

#ifndef RASTERIO_CELLOBJECT_H_
#define RASTERIO_CELLOBJECT_H_

#include <vector>
#include "IndividualOrganism.h"

using namespace std;
class CellObject{
private:
    unsigned x;
    unsigned y;
    int value;
    vector<IndividualOrganism> individualOrganisms;
public:
    CellObject(unsigned p_x, unsigned p_y, int p_value);
    ~CellObject();
    unsigned getX();
    unsigned getY();
    int getValue();
    void addIndividualOrganisms(IndividualOrganism p_individual_organism);
    static void clearCellObject(vector<CellObject*> p_v);
};



#endif /* RASTERIO_CELLOBJECT_H_ */
