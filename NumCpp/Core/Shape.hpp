/* Shape.hpp */
#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

namespace numcpp {

struct Shape {
    std::vector<uint32_t> dimensions;

    // Default constructor
    Shape() : dimensions({}) {}
    // Initializer constructor
    Shape(std::initializer_list<uint32_t> dims) :  dimensions(dims) {}
    // Copy constructor
    Shape(const Shape& newShape) : dimensions(newShape.dimensions) {}
    // Move constructor
    Shape(Shape&& other) noexcept : dimensions(std::move(other.dimensions)) {}
    // Iterator constructor
    template <typename Iter>
    Shape(Iter first, Iter last) : dimensions(first, last) {}
    // Vector constructor
    Shape(const std::vector<uint32_t>& dims) : dimensions(dims) {}
    // Move constructor
    Shape(std::vector<uint32_t>&& dims) noexcept : dimensions(std::move(dims)) {}

    // Changes the shape
    void reshape(const std::initializer_list<uint32_t>& dims) { dimensions = std::move(dims); }

    // Flattens the shape
    void flatten() { dimensions = {get_total_size()};}

    // Returns the total number of dimensions is a shape
    uint32_t get_Ndim() const { return dimensions.size(); }

    // Checks if two shapes are identical
    bool same_shape(const Shape& other) const {
        return this->dimensions == other.dimensions;
    }

    // Checks if two shapes can be multiplied
    bool are_multipliable(const Shape& other) const {
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
    uint32_t get_total_size() const {
        uint32_t result = 1;
        for(const auto& d : dimensions) result *= d;
        return result;
    }

    // Transpose a vector or matrix;
    void transpose() {
        switch(get_Ndim()) {
            case 1:
                reshape({1,dimensions[0]});
                break;
            case 2:
                std::reverse(dimensions.begin(), dimensions.end());
                break;
            default:
                throw std::runtime_error("Cannot transpose arrays with more than 2 dimensions.");
                break;
        }
    }

    // Index operator
    const uint32_t& operator[](int index) const {
        return dimensions[get_index(index)];
    }

    friend std::ostream& operator<<(std::ostream& os, const Shape& shape) {
        os << '(';
        for(uint32_t i = 0; i < shape.dimensions.size(); i++) {
            os << shape.dimensions[i];
            if((i != shape.dimensions.size() - 1) || (0 == i)) os << ',';
        }
        os << ')';
        return os;
    }

private:
    uint32_t get_index(const int& index) const {
        int size = static_cast<int>(dimensions.size());

        if((index >= 0) && (index < size))
            return static_cast<uint32_t>(index);
        else if((index < 0) && (index >= -size))
            return static_cast<uint32_t>(index + size);
        else
            throw std::runtime_error("Index out of range.\n");
    }
};

} // namespace numcpp