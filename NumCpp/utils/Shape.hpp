/* Shape.hpp */
#pragma once

#include <iostream>
#include <vector>

namespace nc {

struct Shape {
    std::vector<size_t> dimensions;
    // Default constructor
    Shape() : dimensions({}) {}
    // Initializer constructor
    Shape(std::initializer_list<size_t> dims) :  dimensions(dims) {}
    // Copy constructor
    Shape(const Shape& newShape) : dimensions(newShape.dimensions) {}
    // Move constructor
    Shape(Shape&& other) noexcept : dimensions(std::move(other.dimensions)) {}

    void reshape(const std::initializer_list<size_t>& dims) { dimensions = std::move(dims); }

    bool same_shape(const Shape& other) const {
        return this->dimensions == other.dimensions;
    }

    size_t get_total_size() {
        size_t result = 1;
        for(const auto& d : dimensions) result *= d;
        return result;
    }

    const size_t& operator[](size_t index) const {
        return dimensions[index];
    }

    friend std::ostream& operator<<(std::ostream& os, const Shape& shape) {
        os << '(';
        for(size_t i = 0; i < shape.dimensions.size(); i++) {
            os << shape.dimensions[i];
            if(i != shape.dimensions.size() - 1) os << ',';
        }
        os << ')';
        return os;
    } 
};

}