/* VecOps.hpp */
#pragma once

#include <vector>

namespace numcpp::util {

template <typename T>
std::vector<std::vector<T>> split(std::vector<T> vin, const size_t& n_groups) {
    size_t grp_size = vin.size()/n_groups;
    std::vector<std::vector<T>> vout(n_groups);

    for(size_t i = 0; i < vin.size(); i++) {
        vout[i/grp_size].push_back(vin[i]);
    }
    return vout;
}

}