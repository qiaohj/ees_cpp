/**
 * @file CommonType.h
 * @brief Head CommonType. A head file to implement the data types commonly used in the application
 * @author Huijie Qiao
 * @version 1.0
 * @date 11/25/2018
 * @details
 * Copyright 2014-2019 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */

#ifndef CommonType_H
#define CommonType_H

#include <boost/unordered_map.hpp>

#include "../Definitions/SparseMap.h"
/**
 * @brief a hash map to save the environmental variables per time step.
 */
typedef boost::unordered_map<unsigned, std::vector<SparseMap*> > hashmap_multiply;
/**
 * @brief a hash map to save distribution map per time step.
 */
typedef boost::unordered_map<unsigned, SparseMap*> hashmap_single;

#endif /* CommonFun_H */
