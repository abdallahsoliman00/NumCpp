/* VecOps.hpp */
#pragma once

#include "Shape.hpp"

namespace numcpp {

template <typename dtype>
std::vector<dtype> hadamardProduct(
    const std::vector<dtype>& larr, const Shape& lshape,
    const std::vector<dtype>& rarr, const Shape& rshape
) {
    auto out_shape = Shape::get_product_shape(lshape, rshape);
    std::vector<dtype> out(out_shape.get_total_size());
    
    // for(int i = 0; i < )

    return out;
    }

} // namespace numcpp
