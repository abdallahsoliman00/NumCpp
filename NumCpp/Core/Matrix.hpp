/* Matrix.hpp */
#pragma once

#include "NArray.hpp"
#include "Shape.hpp"

namespace numcpp {

template <typename dtype = double>
class Matrix : public NArray<dtype> {

protected:
    using NArray<dtype>::_data_ptr;
    using NArray<dtype>::_shape;


public:
    /* ====== Constructors ====== */
    using NArray<dtype>::NArray;  // inherit constructors
    // TODO: Delete all unwanted constructors or only include the constructors needed for matrix
    // TODO: Reimplement the recursive constructor to always ensure a total of two dimensions


    /* ====== Operator Overloads ====== */
    // Multiplication Overloads
    Matrix<dtype> operator*(const Matrix<dtype>& other) const {
        if (!are_multipliable(*this, other)) {
            throw error::ShapeError(this->_shape, other._shape, "multiply");
        }
        auto out_data = util::matmul(
            this->_data_ptr.get(), this->_shape,
            other._data_ptr.get(), other._shape
        );
        auto out_shape = Shape::get_product_shape(this->_shape, other._shape);
        return Matrix(out_data, out_shape);
    }

    NArray<dtype> operator*(const NArray<dtype>& other) const override {
        if(const auto* matrix_other = dynamic_cast<const Matrix<dtype>*>(&other)) {
            return this->operator*(*matrix_other);
        }
        
        // Check if it's a valid 2D NArray that can be treated as a matrix
        if(!are_multipliable(*this, other)) {
            // Treat the NArray as a matrix for multiplication
            auto out_data = util::matmul(
                this->get_data(), this->get_shape(),
                other.get_data(), other.get_shape()
            );
            auto out_shape = Shape::get_product_shape(this->_shape, other.get_shape());
            return Matrix(std::move(out_data), out_shape);
        }
        
        throw error::ShapeError(this->_shape, other.get_shape(), "multiply");
    }


    /* ====== Helper Functions ====== */

    static bool are_multipliable(const Matrix<dtype>& lmat, const Matrix<dtype>& rmat) {
        return static_cast<bool>(Shape::get_matmul_type(lmat._shape, rmat.get_shape()));
    }

    Matrix transpose() {
        auto out_shape = _shape.transpose();
        auto out_data_ptr = this->get_data_copy_as_shared_ptr();
        util::transpose(out_data_ptr.get(), this->_shape);
        return Matrix(out_data_ptr,out_shape);
    }

};

} // namespace numcpp
