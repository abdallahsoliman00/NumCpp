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
    // using NArray<dtype>::NArray;  // inherit constructors
    // TODO: Delete all unwanted constructors or only include the constructors needed for matrix
    // TODO: Reimplement the recursive constructor to always ensure a total of two dimensions

    // Default constructor
    Matrix() : NArray<dtype>() {}


    // Copy constructor
    Matrix(const Matrix& mat) : NArray<dtype>(mat) {}


    // Move constructor
    Matrix(Matrix && mat) noexcept : NArray<dtype>(std::move(mat)) {}


    // Scalar constructor
    Matrix(const dtype& num) : NArray<dtype>(num) {
        this->_shape = Shape({1,1});
    }


    // Vector constructor
    Matrix(const std::vector<dtype>& data) : NArray<dtype>(data) {
        this->_shape = this->_shape.transpose();
    }

    Matrix(std::vector<dtype>&& data) noexcept : NArray<dtype>(data) {
        this->_shape = this->_shape.transpose();
    }


    // List constructor
    Matrix(std::initializer_list<dtype> list) : NArray<dtype>(list) {
        this->_shape = this->_shape.transpose();
    }


    // Data vector + shape constructor (copy)
    Matrix(const std::vector<dtype>& vec, const Shape& shape) : NArray<dtype>(vec, shape) {
        check_and_adjust_shape(this->_shape);
    }

    
    // Data vector + shape constructor (move)
    Matrix(std::vector<dtype>&& vec, const Shape& shape) : NArray<dtype>(vec, shape) {
        check_and_adjust_shape(this->_shape);
    }


    // Data + shape constructor from pointer
    Matrix(dtype* ptr, const Shape& shape) : NArray<dtype>(ptr, shape) {
        check_and_adjust_shape(this->_shape);
    }

    Matrix(dtype* ptr, Shape&& shape) : NArray<dtype>(ptr, shape) {
        check_and_adjust_shape(this->_shape);
    }


    // Constructor from other NArrays
    template <typename... Arrays>
    Matrix(const NArray<dtype>& first, const NArray<dtype>& next, const Arrays&... rest) {
        if(first.get_shape().get_Ndim() > 1) {
            throw error::ShapeError("Cannot construct a Matrix with more than 2-Dimensions.");
        }

        this->_shape = first.get_shape();
        // Get size requirements for new NArray
        size_t total_size = 0;
        int new_dims = 0;
        this->get_size_requirements(total_size, new_dims, first, next, rest...);

        // Initialise _shape and _data_ptr
        this->_shape.insert_dimension(new_dims, 0);
        this->_data_ptr = std::shared_ptr<dtype>(new dtype[total_size], std::default_delete<dtype[]>());

        // Add data
        this->add_data_to_data_ptr(this->_data_ptr, 0, first, next, rest...);
    }


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

    static bool are_multipliable(const NArray<dtype>& lmat, const NArray<dtype>& rmat) {
        return static_cast<bool>(Shape::get_matmul_type(lmat.get_shape(), rmat.get_shape()));
    }

    Matrix transpose() {
        auto out_shape = _shape.transpose();
        auto out_data_ptr = this->get_data_copy_as_shared_ptr();
        util::transpose(out_data_ptr.get(), this->_shape);
        return Matrix(out_data_ptr, out_shape);
    }

private:
    void check_and_adjust_shape(Shape& shape) {
        if(shape.get_Ndim() == 1)
            shape = Shape{1,shape[0]};
        else if(shape.get_Ndim() > 2 || shape.get_Ndim() < 1)
            throw error::ShapeError("Could not construct a Matrix with shape " + util::toString(shape) + ".");
    }

};

} // namespace numcpp
