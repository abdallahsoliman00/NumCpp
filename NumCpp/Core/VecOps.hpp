/* VecOps.hpp */
#pragma once

#include <cmath>
#include <type_traits>

#include "Shape.hpp"
#include "NArray.hpp"
#include "Matrix.hpp"

namespace numcpp {

// Matrix-multiplication of two matrices
template <typename dtype>
NArray<dtype> matmul(const NArray<dtype>& lmat, const NArray<dtype>& rmat) {
    if(!static_cast<bool>(Shape::get_matmul_type(lmat.get_shape(), rmat.get_shape()))) {
        throw error::ShapeError(lmat.get_shape(), rmat.get_shape(), "multiply");
    }
    std::vector<dtype> out_data = util::matmul(
        lmat.get_data(), lmat.get_shape(),
        rmat.get_data(), rmat.get_shape()
    );
    auto out_shape = Shape::get_product_shape(lmat.get_shape(), rmat.get_shape());

    return NArray(std::move(out_data), out_shape);
}


// Dot product of two vectors
template <typename dtype>
NArray<dtype> dot(const NArray<dtype>& a, const NArray<dtype>& b) {
    MatmulType type = Shape::get_matmul_type(a.get_shape(), b.get_shape());
    
    switch(type) {
        case MatmulType::Invalid:
            throw error::ShapeError(a.get_shape(), b.get_shape(), "dot");
            break;
        case MatmulType::Dot: {
            dtype sum = 0;
            for(size_t i = 0; i < a.get_total_size(); i++)
                sum += (a.get_data()[i] * b.get_data()[i]);
            return NArray(sum);
            break;
        }
        case MatmulType::MatCol:
            return matmul(a,b);
            break;
        case MatmulType::RowMat:
            return matmul(a,b);
            break;
        case MatmulType::MatMat:
            return matmul(a,b);
            break;
        default:
        throw error::ShapeError(a.get_shape(), b.get_shape(), "dot");
        break;
    }
}

template <typename dtype>
Matrix<dtype> dot(const Matrix<dtype>& a, const Matrix<dtype>& b) {
    return a * b;
}

template <typename dtype>
dtype dot(dtype a, dtype b) { return a * b; }


// Flattens the input NArrays and returns the dot-product
template <typename dtype>
dtype vdot(const NArray<dtype>& a, const NArray<dtype>& b) {
    if(!a.same_shape(b))
        throw error::ShapeError(a.get_shape(), b.get_shape(), "dot");

    dtype sum = 0;
    for(size_t i = 0; i < a.get_total_size(); i++)
        sum += (a.get_data()[i] * b.get_data()[i]);

    return sum;
}


// Returns the elementwise multiplication of two arrays of any dimanesion
template <typename T, typename U>
auto hadamard(const NArray<T>& larr, const NArray<U>& rarr)
    -> NArray<std::common_type_t<T, U>>
{
    if (larr.get_shape() != rarr.get_shape())
        throw error::ShapeError(larr.get_shape(), rarr.get_shape(), "multiply");

    using R = std::common_type_t<T, U>;
    NArray<R> out(larr.get_shape());

    for (size_t i = 0; i < larr.get_total_size(); i++)
        out.get_data()[i] = static_cast<R>(larr[i]) * static_cast<R>(rarr[i]);

    return out;
}

} // namespace numcpp
