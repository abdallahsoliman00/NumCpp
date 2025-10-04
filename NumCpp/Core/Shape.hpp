/* Shape.hpp */
#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

#include "../Utils/Errors.hpp"

namespace numcpp {

enum class MatmulType {
    Invalid = 0,
    Dot,
    RowMat,
    MatCol,
    MatMat
}; // enum class MatmulType


struct Shape {
    std::vector<size_t> dimensions;

    // Default constructor
    Shape() : dimensions({}) {}
    // Scalar constructor
    Shape(unsigned int num) : dimensions({num}) {}
    // Initializer constructor
    Shape(std::initializer_list<size_t> dims) :  dimensions(dims) {}
    // Copy constructor
    Shape(const Shape& newShape) : dimensions(newShape.dimensions) {}
    // Move constructor
    Shape(Shape&& other) noexcept : dimensions(std::move(other.dimensions)) {}
    // Iterator constructor
    template <typename Iter>
    Shape(Iter first, Iter last) : dimensions(first, last) {}
    // Vector constructor
    Shape(const std::vector<size_t>& dims) : dimensions(dims) {}
    // Move constructor
    Shape(std::vector<size_t>&& dims) noexcept : dimensions(std::move(dims)) {}

    // Changes the shape
    void reshape(const std::initializer_list<size_t>& dims) { dimensions = std::move(dims); }

    // Inserts a dimension at a specified position
    void insert_dimension(size_t dimension, int position) {
        dimensions.insert(dimensions.begin() + get_index(position), dimension);
    }

    // Returns a new flat shape
    Shape flatten() const { return Shape({get_total_size()}); }

    // Returns the total number of dimensions is a shape
    size_t get_Ndim() const { return dimensions.size(); }

    // Checks if two shapes are identical
    bool same_shape(const Shape& other) const {
        return this->dimensions == other.dimensions;
    }

    // Equality operator
    bool operator==(const Shape& other) const {
        return same_shape(other);
    }

    // Inequality operator
    bool operator!=(const Shape& other) const {
        return !same_shape(other);
    }

    // Returns the shape of the product
    static Shape get_product_shape(const Shape& lshape, const Shape& rshape) {
        MatmulType type = get_matmul_type(lshape, rshape);
        switch (type) {
        case MatmulType::Invalid:
            throw error::ShapeError("Invlalid shapes for matrix multiplication.");
            break;
        case MatmulType::Dot:
            return Shape(1);
            break;
        case MatmulType::RowMat:
            return Shape(rshape[1]);
            break;
        case MatmulType::MatCol:
            return Shape(lshape[0]);
            break;
        case MatmulType::MatMat:
            return Shape({lshape[0], rshape[1]});
        default:
            throw error::ShapeError("Invlalid shapes for matrix multiplication.");
            break;
        }
    }

    // Returns the total size required to store the array
    size_t get_total_size() const {
        size_t result = 1;
        for(const auto& d : dimensions) result *= d;
        return result;
    }

    // Transpose a vector or matrix;
    Shape transpose() const {
        switch(get_Ndim()) {
            case 1:
                return Shape({1,dimensions[0]});
                break;
            case 2:
                return Shape({dimensions[1],dimensions[0]});
                break;
            default:
                throw std::runtime_error("Cannot transpose arrays with more than 2 dimensions.");
                break;
        }
    }

    // Index operator
    const size_t& operator[](int index) const {
        return dimensions[get_index(index)];
    }

    friend std::ostream& operator<<(std::ostream& os, const Shape& shape) {
        os << '(';
        for(size_t i = 0; i < shape.dimensions.size(); i++) {
            os << shape.dimensions[i];
            if((i != shape.dimensions.size() - 1) || (0 == i)) os << ',';
        }
        os << ')';
        return os;
    }

    static MatmulType get_matmul_type(const Shape& a, const Shape& b) {
        const int ndim1 = a.get_Ndim();
        const int ndim2 = b.get_Ndim();

        if (ndim1 > 2 || ndim2 > 2)
            return MatmulType::Invalid;

        if (ndim1 == 1 && ndim2 == 1)
            return (a.dimensions[0] == b.dimensions[0])
                ? MatmulType::Dot : MatmulType::Invalid;

        if (ndim1 == 1 && ndim2 == 2)
            return (a.dimensions[0] == b.dimensions[0])
                ? MatmulType::RowMat : MatmulType::Invalid;

        if (ndim1 == 2 && ndim2 == 1)
            return (a.dimensions[1] == b.dimensions[0])
                ? MatmulType::MatCol : MatmulType::Invalid;

        if (ndim1 == 2 && ndim2 == 2)
            return (a.dimensions[1] == b.dimensions[0])
                ? MatmulType::MatMat : MatmulType::Invalid;

        return MatmulType::Invalid;
    }

private:
    size_t get_index(int index) const {
        int size = static_cast<int>(dimensions.size());

        if((index >= 0) && (index < size))
            return static_cast<size_t>(index);
        else if((index < 0) && (index >= -size))
            return static_cast<size_t>(index + size);
        else
            throw std::runtime_error("Shape index out of range.\n");
    }
};  // struct Shape


} // namespace numcpp