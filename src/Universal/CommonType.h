#ifndef CommonType_H
#define CommonType_H

#include <boost/unordered_map.hpp>

#include "../Definitions/SparseMap.h"

typedef boost::unordered_map <unsigned, vector<SparseMap*> > hashmap_multiply;
typedef boost::unordered_map <unsigned, SparseMap*> hashmap_single;

#endif /* CommonFun_H */
