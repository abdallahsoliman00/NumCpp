/* Shape.hpp */
#pragma once

#include <iostream>
#include <vector>

namespace numcpp {

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
    // Iterator constructor
    template <typename Iter>
    Shape(Iter first, Iter last) : dimensions(first, last) {}
    // Vector constructor
    Shape(const std::vector<size_t>& dims) : dimensions(dims) {}
    // Move constructor
    Shape(std::vector<size_t>&& dims) noexcept : dimensions(std::move(dims)) {}

    void reshape(const std::initializer_list<size_t>& dims) { dimensions = std::move(dims); }

    size_t get_Ndim() const { return dimensions.size(); }

    bool same_shape(const Shape& other) const {
        return this->dimensions == other.dimensions;
    }

    size_t get_total_size() const {
        size_t result = 1;
        for(const auto& d : dimensions) result *= d;
        return result;
    }

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

}