/* Matrix.hpp */
#pragma once

#include "NArray.hpp"
#include "Shape.hpp"

namespace numcpp {

template <typename dtype = double>
class Matrix : public NArray<dtype> {
protected:
    using NArray<dtype>::data;
    using NArray<dtype>::shape;

public:
    using NArray<dtype>::NArray;  // inherit constructors
    // TODO: Delete all unwanted constructors or only include the constructors needed for matrix
    // TODO: Reimplement the recursive constructor to always ensure a total of two dimensions

    bool are_multipliable(const Matrix<dtype>& other) const {
        return this->shape.are_multipliable(other.shape);
    }

    // hide + delete base Hadamard when LHS is Matrix
    NArray<dtype> operator*(const NArray<dtype>&) const = delete;
    
    Matrix<dtype> operator*(const Matrix<dtype>& other) const {
        if (!this->shape.same_shape(other.shape)) {
            error::ShapeError(this->shape, other.shape, "multiply");
        }
        auto out_data = util::matmul(
            this->data, this->shape,
            other.data, other.shape
        );
        auto out_shape = Shape::get_product_shape(this->shape, other.shape);

        return Matrix(out_data, out_shape);
    }
    
};

} // namespace numcpp
