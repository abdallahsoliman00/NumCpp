/* Shape.hpp */
#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

namespace numcpp {

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

    // Checks if two shapes can be multiplied
    bool are_matrix_multipliable(const Shape& other) const {
        if(this->get_Ndim() > 2 || other.get_Ndim() > 2)
            return false;
        else
            return (this->dimensions[get_index(-1)] == other.dimensions[other.get_index(-2)]);
    }

    // Returns the shape of the product
    static Shape get_product_shape(const Shape& lshape, const Shape& rshape) {
        return Shape({lshape[0], rshape[-1]});
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

private:
    size_t get_index(const int& index) const {
        int size = static_cast<int>(dimensions.size());

        if((index >= 0) && (index < size))
            return static_cast<size_t>(index);
        else if((index < 0) && (index >= -size))
            return static_cast<size_t>(index + size);
        else
            throw std::runtime_error("Index out of range.\n");
    }
};

} // namespace numcpp