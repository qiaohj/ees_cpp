/*
 * CellObject.h
 *
 *  Created on: Oct 19, 2014
 *      Author: qiaohj
 */

#ifndef RASTERIO_CELLOBJECT_H_
#define RASTERIO_CELLOBJECT_H_

using namespace std;
class CellObject{
private:
    unsigned x;
    unsigned y;
    int value;
public:
    CellObject(unsigned p_x, unsigned p_y, int p_value);
    ~CellObject();
    unsigned getX();
    unsigned getY();
    int getValue();
};



#endif /* RASTERIO_CELLOBJECT_H_ */
