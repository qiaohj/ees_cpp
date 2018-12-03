/**
 * @file NicheBreadth.cpp
 * @brief Class NicheBreadth. A class of the niche breadth of a virtual species
 * @author Huijie Qiao
 * @version 1.0
 * @date 11/25/2018
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
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

