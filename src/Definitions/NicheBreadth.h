/**
 * @file NicheBreadth.h
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


#ifndef DEFINITIONS_NICHEBREADTH_H_
#define DEFINITIONS_NICHEBREADTH_H_
/**
 * @brief A class of the niche breadth of a virtual species
 */
class NicheBreadth {
private:
    float min;
    float max;
public:
    /**
	 * @brief Constructor of NicheBreadth class
	 * @param min lower limit of the niche breadth
	 * @param max upper limit of the niche breadth
	 */
    NicheBreadth(float min, float max);
    /**
	 * @brief Destructor of NicheBreadth class
	 *
	 * release all the resources
	 */
    virtual ~NicheBreadth();
    /**
     * @brief return the upper limit of the niche breadth
     * @return
     */
    float getMax();
    /**
	 * @brief return the lower limit of the niche breadth
	 * @return
	 */
    float getMin();
};

#endif /* DEFINITIONS_NICHEBREADTH_H_ */
