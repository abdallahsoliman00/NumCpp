/* VecOps.hpp */
#pragma once

#include "Shape.hpp"
#include "NArray.hpp"
#include "Matrix.hpp"

namespace numcpp {

template <typename dtype>
NArray<dtype> matmul(const NArray<dtype>& lmat, const NArray<dtype>& rmat) {
    if (!(lmat.get_shape().are_matrix_multipliable(rmat.get_shape()))) {
        error::ShapeError(lmat.get_shape(), rmat.get_shape(), "multiply");
    }
    auto out_data = util::matmul(
        lmat.get_data(), lmat.get_shape(),
        rmat.get_data(), rmat.get_shape()
    );
    auto out_shape = Shape::get_product_shape(lmat.get_shape(), rmat.get_shape());

    return NArray(out_data, out_shape);
}

template <typename dtype>
NArray<dtype> dot(const NArray<dtype>& larr, const NArray<dtype>& rarr) {
    return larr * rarr;
}
template <typename dtype>
Matrix<dtype> dot(const Matrix<dtype>& larr, const Matrix<dtype>& rarr) {
    return larr * rarr;
}

template <typename dtype>
NArray<dtype> hadamard(const NArray<dtype>& larr, const NArray<dtype>& rarr) {
    if(larr.get_shape() != rarr.get_shape())
        throw error::ShapeError(larr.get_shape(), rarr.get_shape(), "multiply");
    else {
        std::vector<dtype> out_vec = util::elementwiseOP(larr.get_data(), rarr.get_data(), &util::multiply<dtype>);
        return NArray<dtype>(out_vec, larr.get_shape());
    }
}

} // namespace numcpp
