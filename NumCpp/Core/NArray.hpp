/* NArray.hpp */
#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include "Shape.hpp"
#include "VecOps.hpp"
#include "../Utils/MathOps.hpp"
#include "../Utils/VecOps.hpp"
#include "../Utils/Errors.hpp"


namespace numcpp {

template <typename dtype = double>
class NArray {
private:
    std::vector<dtype> data;
    Shape shape;
    
protected:
    // Checks if two shapes are the same
    bool same_shape(const NArray& other) const {
        return shape.same_shape(other.shape);
    }
    // Checks if *this * other is possible
    // If elementwise, returns 1, if matrix multipliable returns 2.
    uint16_t are_multipliable(const NArray& other) const {
        if(this->same_shape(other)) return 1;
        else if(shape.are_multipliable(other.shape)) return 2;
        else return 0;
    }
    
    // Elementwise operation of two NArrays
    NArray elementWiseOp(const NArray &other, std::function<dtype(dtype, dtype)> func) const {
        NArray<dtype> out(this->shape);
        for(uint32_t i = 0; i < shape.get_total_size(); i++) {
            out.data[i] = func(data[i], other.data[i]);
        }
        return out;
    }

    // Right scalar operation to NArrays
    NArray fullVecOpR(const dtype& scalar, std::function<dtype(dtype, dtype)> func) const {
        std::vector<dtype> newVec(shape.get_total_size());
        for(int i = 0; i < shape.get_total_size(); i++) {
            newVec[i] = func(data[i], scalar);
        }
        return NArray(std::move(newVec), shape);
    }

    // Left scalar operation to NArrays
    NArray fullVecOpL(const dtype& scalar, std::function<dtype(dtype, dtype)> func) const {
        std::vector<dtype> newVec(shape.get_total_size());
        for(int i = 0; i < shape.get_total_size(); i++) {
            newVec[i] = func(scalar, data[i]);
        }
        return NArray(std::move(newVec), shape);
    }

    NArray<bool> checkEquality(const NArray& other, const bool& eq = true) const {
        if(!same_shape(other)) {
            error::ShapeError(this->shape, other.shape, "compare")
        }
        else {
            std::vector<bool> newVec(shape.get_total_size());
            for(int i = 0; i < shape.get_total_size(); i++) {
                newVec[i] = eq ? (data[i] == other.data[i]) : (data[i] != other.data[i]);
            }
            return NArray<bool>(newVec, shape);
        }
    }

    static void OneDPrint(std::ostream& os, const NArray& arr) {
        os << '[';
        for(int i = 0; i < arr.shape[0]; i++) {
        if constexpr (std::is_same_v<dtype, bool>) {
            os << (arr.data[i] ? "true" : "false");
        } else {
            os << arr.data[i];
        }
            if(i != arr.shape[0] - 1) os << ", ";
        }
        os << ']';
    }

    static void recursivePrint(std::ostream& os, const NArray& arr, int depth = 0) {
        // Base case
        if(arr.shape.get_Ndim() == 1) {
            OneDPrint(os, arr);
            return;
        }

        // Take the first element and split 
        uint32_t n_grps = arr.shape[0];
        auto groups = util::split(arr.data, n_grps);

        Shape subshape(arr.shape.dimensions.begin() + 1, arr.shape.dimensions.end());
        
        os << '[';
        for (uint32_t i = 0; i < n_grps; i++) {
            recursivePrint(os, NArray(groups[i], subshape), depth + 1);
            if(i != n_grps - 1) {
                if(depth % subshape[0] == 0)
                    os << ",\n\n";
                else
                    os << ",\n" << std::string(n_grps, ' ');
            }
        }
        os << ']';
    }

    uint32_t get_index(const int& index) const {
        int size = static_cast<int>(data.size());

        if((index >= 0) && (index < size))
            return static_cast<uint32_t>(index);
        else if((index < 0) && (index >= -size))
            return static_cast<uint32_t>(index + size);
        else
            throw std::runtime_error("Index out of range.");
    }

public:
    /* 1D constructors */
    // Default constructor
    NArray() : data(), shape() {}
    // Vector constructor
    NArray(const std::vector<dtype>& data) : data(data), shape({static_cast<uint32_t>(data.size())}) {}
    // List constructor
    NArray(std::initializer_list<dtype> list) : data(list), shape({static_cast<uint32_t>(list.size())}) {}
    // Array constructor
    NArray(dtype *array, uint32_t size) : data(array, array + size), shape({size}) {}
    // Copy constructor
    NArray(const NArray& newVec) : data(newVec.data), shape(newVec.shape) {}
    // Move constructor
    NArray(NArray&& other) noexcept : data(std::move(other.data)), shape(std::move(other.shape)) {}
    // Repeat constructor
    NArray(uint32_t count, dtype val = 0) : data(count, val), shape({count}) {}

    /* N-Dimensional contructors */
    // Recursive constructor
    NArray(std::initializer_list<NArray> arr) {
        // Check if empty
        if(!arr.size()) return; // Base case

        // Read first dimension
        shape.dimensions.push_back(arr.size());

        // Continue adding dimensions recursively until base case is reached
        const NArray& first = *arr.begin();
        shape.dimensions.insert(shape.dimensions.end(),
                                first.shape.dimensions.begin(),
                                first.shape.dimensions.end());

        // Consistency check
        for (const NArray& sub : arr) {
            if (sub.shape.dimensions != first.shape.dimensions) {
                throw std::runtime_error("Jagged initializer lists are not supported.");
            }
        }

        // Reserve space for values
        data.reserve(shape.get_total_size());

        // Add values
        for (const NArray& sub : arr) {
            data.insert(data.end(), sub.data.begin(), sub.data.end());
        }
    }
    // Data + shape constructor
    NArray(std::vector<dtype> vec, Shape shape) : data(std::move(vec)), shape(std::move(shape)) {
        if (this->shape.get_total_size() != data.size()) {
            throw std::runtime_error("Cannot construct NArray because Shape and data size don't match.");
        }
    }
    // Shape + initializer constructor
    NArray(const Shape& shape, dtype val = 0) : data(shape.get_total_size(), val), shape(shape) {}
    NArray(Shape&& shape, dtype val = 0) noexcept : data(shape.get_total_size(), val), shape(std::move(shape)) {}



    /* Operator overloading*/
    // TODO: These need reworking for ndarrays: check size
    // Array addition
    NArray operator+(const NArray& other) const {
        if(!same_shape(other))
            error::ShapeError(this->shape, other.shape, "add");
        else
            return elementWiseOp(other, &util::add<dtype>);
    }
    // Array subtraction
    NArray operator-(const NArray& other) const {
        if(!same_shape(other))
            error::ShapeError(this->shape, other.shape, "subtract");
        else
            return elementWiseOp(other, &util::subtract<dtype>);
    }
    // Array multiplication
    NArray operator*(const NArray& other) const {
        switch(this->are_multipliable(other)) {
            case 0:
                error::ShapeError(this->shape, other.shape, "multiply");
            case 1:
                return elementWiseOp(other, &util::multiply<dtype>);
            case 2:
                return hadamardProduct(this->data, this->shape, other.data, other.shape);
        }
    }
    // Array division
    NArray operator/(const NArray& other) const {
        if(!same_shape(other))
            error::ShapeError(this->shape, other.shape, "divide");
        else
            return elementWiseOp(other, &util::divide<dtype>);
    }

    /* Scalar Overloads */
    // Right addition overload
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray operator+(T num) const {
        return fullVecOpR(static_cast<dtype>(num), &util::add<dtype>);
    }
    // Right subtraction overload
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray operator-(T num) const {
        return fullVecOpR(static_cast<dtype>(num), &util::subtract<dtype>);
    }
    // Right multiplication overload
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray operator*(T num) const {
        return fullVecOpR(static_cast<dtype>(num), &util::multiply<dtype>);
    }
    // Right division overload
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray operator/(T num) const {
        return fullVecOpR(static_cast<dtype>(num), &util::divide<dtype>);
    }

    // Left addition overload
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    friend NArray operator+(T num, const NArray& arr) {
        return arr.fullVecOpL(static_cast<dtype>(num), &util::add<dtype>);
    }
    // Left subtraction overload
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    friend NArray operator-(T num, const NArray& arr) {
        return arr.fullVecOpL(static_cast<dtype>(num), &util::subtract<dtype>);
    }
    // Left multiplication overload
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    friend NArray operator*(T num, const NArray& arr) {
        return arr.fullVecOpL(static_cast<dtype>(num), &util::multiply<dtype>);
    }
    // Left division overload
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    friend NArray operator/(T num, const NArray& arr) {
        return arr.fullVecOpL(static_cast<dtype>(num), &util::divide<dtype>);
    }


    NArray<bool> operator==(const NArray& other) const {
        return checkEquality(other);
    }

    NArray<bool> operator!=(const NArray& other) const {
        return checkEquality(other, false);
    }

    // TODO: Make these operators properly index the n-dimensinal arrays.
    // Use recursion to get the correct arrays?
    dtype& operator[](const int& i) { return data[get_index(i)]; }

    const dtype& operator[](const int& i) const { return data[get_index(i)]; }

    friend std::ostream& operator<<(std::ostream& os, const NArray& arr) {
        switch(arr.shape.get_Ndim()) {
        case 1: // 1D vector
            OneDPrint(os, arr);
            break;

        case 2: { // Martix
            os << '[';
            auto grps = util::split(arr.data, arr.shape[0]);
            for(uint32_t i = 0; i < grps.size(); i++) {
                OneDPrint(os, NArray(grps[i]));
                if(i != grps.size() - 1) os << ",\n ";
            }
            os << ']';
            break;
        }

        default: // Anything else
            recursivePrint(os, arr);
            break;
        }
        return os;
} 

    /* Helper functions */

    const Shape& get_shape() const { return this->shape; }

    // Returns a reference to the data
    std::vector<dtype>& get_data() { return data; }

    /* NArray functions */

    NArray flatten() { return NArray(data); }

    // TODO: ravel should have a flat shape, but not change the shape
    // of the original array. i.e. a.shape != b.shape,
    // but a.data == b.data as in they share the same memory
    NArray& ravel() {
        shape.flatten();
        return *this;
    }

};

// Deduction guides
// Vector constructor
template <typename T>
NArray(std::vector<T>) -> NArray<T>;

// List constructor
template <typename T>
NArray(std::initializer_list<T>) -> NArray<T>;

// Array constructor
template <typename T>
NArray(T*, uint32_t) -> NArray<T>;

// Repeat constructor
template <typename T>
NArray(uint32_t, T) -> NArray<T>;

// Data + Shape constructor
template <typename T>
NArray(std::vector<T>, Shape) -> NArray<T>;

} // namespace numcpp

