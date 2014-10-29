/*
 * NicheBreadth.cpp
 *
 *  Created on: Oct 29, 2014
 *      Author: qiaohj
 */

#include "NicheBreadth.h"

NicheBreadth::NicheBreadth(float p_min, float p_max) {
    min = p_min;
    max = p_max;
}

NicheBreadth::~NicheBreadth() {

}
float NicheBreadth::getMin(){
    return min;
}
float NicheBreadth::getMax(){
    return max;
}

