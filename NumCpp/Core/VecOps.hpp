/* VecOps.hpp */
#pragma once

#include "Shape.hpp"
#include "NArray.hpp"

namespace numcpp {

template <typename dtype>
NArray<dtype> matmul(const NArray<dtype>& lmat, const NArray<dtype>& rmat) {
    if (!(lmat.shape.are_multipliable(rmat.shape))) {
        error::ShapeError(lmat.shape, rmat.shape, "multiply");
    }
    auto out_data = util::matmul(
        lmat.data, lmat.shape,
        rmat.data, rmat.shape
    );
    auto out_shape = Shape::get_product_shape(this->shape, other.shape);

    return NArray(out_data, out_shape);
}

} // namespace numcpp
