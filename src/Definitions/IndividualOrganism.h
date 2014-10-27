/*
 * IndividualOrganism.h
 *
 *  Created on: Oct 24, 2014
 *      Author: qiaohj
 */

#ifndef DEFINITIONS_INDIVIDUALORGANISM_H_
#define DEFINITIONS_INDIVIDUALORGANISM_H_

class IndividualOrganism {
private:
    unsigned speciedId;
    unsigned year;

public:
    IndividualOrganism(unsigned p_specied_id, unsigned p_year);
    virtual ~IndividualOrganism();
};

#endif /* DEFINITIONS_INDIVIDUALORGANISM_H_ */
