/* VecOps.hpp */
#pragma once

#include <cmath>

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
    if(larr.get_shape().get_Ndim() == 1) {
        if(!larr.same_shape(rarr))
            error::ShapeError(larr.get_shape(), rarr.get_shape(), "dot");
        else {
            dtype sum = 0;
            for(size_t i = 0; i < larr.get_total_size(); i++)
                sum += (larr.get_data()[i] * rarr.get_data()[i]);
            return NArray(sum);
        }
    } else if(larr.get_shape().get_Ndim() == 2){
        return matmul(larr, rarr);
    } else error::ShapeError(larr.get_shape(), rarr.get_shape(), "dot");
}

template <typename dtype>
Matrix<dtype> dot(const Matrix<dtype>& larr, const Matrix<dtype>& rarr) {
    return larr * rarr;
}

template <typename dtype>
dtype dot(dtype a, dtype b) { return a * b; }

template <typename dtype>
dtype vdot(const NArray<dtype>& a, const NArray<dtype>& b) {
    if(!a.same_shape(b))
        error::ShapeError(larr.get_shape(), rarr.get_shape(), "dot");

    dtype sum = 0;
    for(size_t i = 0; i < a.get_total_size(); i++)
        sum += (a.get_data()[i] * b.get_data()[i]);

    return sum;
}

template <typename dtype>
NArray<dtype> pow(const NArray<dtype>& arr, const int& exponent) {
    NArray<dtype> out(arr);
    dtype* data = out.get_data();
    for(int i = 0; i < arr.get_shape().get_total_size(); i++) {
        data[i] = std::pow(data[i], exponent);
    }
    return out;
}

template <typename dtype>
NArray<dtype> hadamard(const NArray<dtype>& larr, const NArray<dtype>& rarr) {
    if(larr.get_shape() != rarr.get_shape())
        throw error::ShapeError(larr.get_shape(), rarr.get_shape(), "multiply");
    else {
        std::vector<dtype> out_vec = util::elementwiseOP(
            larr.get_data(), rarr.get_data(),
            larr.get_shape().get_total_size(), &util::multiply<dtype>
        );
        return NArray<dtype>(std::move(out_vec), larr.get_shape());
    }
}

} // namespace numcpp
