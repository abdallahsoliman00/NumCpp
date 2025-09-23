/* VecOps.hpp */
#pragma once

#include <vector>
#include "../Core/Shape.hpp"

namespace numcpp::util {

template <typename dtype>
std::vector<dtype> matmul(
    const std::vector<dtype>& larr, const Shape& lshape,
    const std::vector<dtype>& rarr, const Shape& rshape
) {
    auto out_shape = Shape::get_product_shape(lshape, rshape);
    std::vector<dtype> out(out_shape.get_total_size(), static_cast<dtype>(0));
    
    int m = lshape[0];
    int k = lshape[1];
    int n = rshape[1];

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            dtype sum = 0;
            for (int t = 0; t < k; t++) {
                sum += larr[i * k + t] * rarr[t * n + j];
            }
            out[i * n + j] = sum;
        }
    }

    return out;
}


template <typename dtype>
std::vector<dtype> transpose(
    const std::vector<dtype>& arr, const Shape& shape
) {
    std::vector<dtype> out(arr.size());
    
    int rows = shape[0];
    int cols = shape[1];

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            out[j * rows + i] = arr[i * cols + j];
        }
    }
    return out;
}


template <typename T>
std::vector<std::vector<T>> split(std::vector<T> vin, const size_t& n_groups) {
    size_t grp_size = vin.size()/n_groups;
    std::vector<std::vector<T>> vout(n_groups);

    for(size_t i = 0; i < vin.size(); i++) {
        vout[i/grp_size].push_back(vin[i]);
    }
    return vout;
}


template <typename T>
std::vector<T> elementwiseOP(
    const std::vector<T>& larr,
    const std::vector<T>& rarr,
    std::function<T(T, T)> func
) {
    std::vector<T> out(larr.size());
    for(uint32_t i = 0; i < larr.size(); i++) {
        out[i] = func(larr[i], rarr[i]);
    }
    return out;
}

} // namespace numcpp::util