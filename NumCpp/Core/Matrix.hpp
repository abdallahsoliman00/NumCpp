/* Matrix.hpp */
#pragma once

#include "NArray.hpp"
#include "Shape.hpp"

namespace numcpp {

template <typename dtype = double>
class Matrix : public NArray<dtype> {
protected:
    using NArray<dtype>::data_ptr;
    using NArray<dtype>::shape;

public:
    /* Constructors */
    using NArray<dtype>::NArray;  // inherit constructors
    // TODO: Delete all unwanted constructors or only include the constructors needed for matrix
    // TODO: Reimplement the recursive constructor to always ensure a total of two dimensions


    /* Helper Functions */
    bool are_multipliable(const Matrix<dtype>& other) const {
        return this->shape.are_matrix_multipliable(other.shape);
    }

    Matrix transpose() {
        auto out_shape = shape.transpose();
        auto out_data_ptr = this->get_data_copy_as_shared_ptr();
        util::transpose(out_data_ptr.get(), shape);
        return Matrix(out_data_ptr,out_shape);
    }


    /* Operator Overloads */
    // Multiplication Overloads
    Matrix<dtype> operator*(const Matrix<dtype>& other) const {
        if (!this->are_multipliable(other)) {
            throw error::ShapeError(this->shape, other.shape, "multiply");
        }
        auto out_data = util::matmul(
            this->data_ptr.get(), this->shape,
            other.data_ptr.get(), other.shape
        );
        auto out_shape = Shape::get_product_shape(this->shape, other.shape);
        return Matrix(out_data, out_shape);
    }
    NArray<dtype> operator*(const NArray<dtype>& other) const override {
        if(const auto* matrix_other = dynamic_cast<const Matrix<dtype>*>(&other)) {
            return this->operator*(*matrix_other);
        }
        
        // Check if it's a valid 2D NArray that can be treated as a matrix
        if(this->shape.are_matrix_multipliable(other.get_shape())) {
            // Treat the NArray as a matrix for multiplication
            auto out_data = util::matmul(
                this->get_data(), this->get_shape(),
                other.get_data(), other.get_shape()
            );
            auto out_shape = Shape::get_product_shape(this->shape, other.get_shape());
            return Matrix(std::move(out_data), out_shape);
        }
        
        throw error::ShapeError(this->shape, other.get_shape(), "multiply");
    }

};

} // namespace numcpp
