/*
 * NicheBreadth.h
 *
 *  Created on: Oct 29, 2014
 *      Author: qiaohj
 */

#ifndef DEFINITIONS_NICHEBREADTH_H_
#define DEFINITIONS_NICHEBREADTH_H_

class NicheBreadth {
private:
    float min;
    float max;
public:
    NicheBreadth(float min, float max);
    virtual ~NicheBreadth();
    float getMax();
    float getMin();
};

#endif /* DEFINITIONS_NICHEBREADTH_H_ */
