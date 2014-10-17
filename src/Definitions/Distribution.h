/*
 * Distribution.h
 *
 *  Created on: Oct 16, 2014
 *      Author: qiaohj
 */

using namespace std;

#include <string>
#include "Map.h"

class Distribution {
private:
    Map* distributionMap;
public:
    Distribution(Map* distributionMap);
    Distribution(Distribution* potential, Distribution* dispersal);
    virtual ~Distribution();
};

