/* NArray.hpp */
#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include "Shape.hpp"
#include "../Utils/MathOps.hpp"
#include "../Utils/VecOps.hpp"
#include "../Utils/Errors.hpp"


namespace numcpp {

template <typename dtype = double>
class NArray {
protected:
    // TODO: Change the entore data storage system to mimic numpy better.
    // make data a shared pointer that points to the data unless a copy is called
    std::vector<dtype> data;
    Shape shape;

    // Checks if two shapes are the same
    bool same_shape(const NArray& other) const {
        return shape.same_shape(other.shape);
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

    // Returns a mask of which NArray elements are the same
    NArray<bool> elementwiseCompare(const NArray& other, std::function<bool(dtype,dtype)> comparison_func) const {
        if(!same_shape(other)) {
            throw error::ShapeError(this->shape, other.shape, "compare");
        }
        else {
            std::vector<bool> newVec(shape.get_total_size());
            for(int i = 0; i < shape.get_total_size(); i++) {
                newVec[i] = comparison_func(data[i], other.data[i]);
            }
            return NArray<bool>(newVec, shape);
        }
    }

    // Print a 1D array
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

    // Recursively prints the N-Dimensional arrays (> 2D arrays)
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
        int size = static_cast<int>(shape[0]);

        if((index >= 0) && (index < size))
            return static_cast<uint32_t>(index);
        else if((index < 0) && (index >= -size))
            return static_cast<uint32_t>(index + size);
        else
            throw std::runtime_error("Index out of range.");
    }

public:
    /* ====== 1D constructors ====== */
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
                throw error::ValueError("Jagged initializer lists are not supported.");
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
    NArray(std::vector<dtype>&& vec, Shape&& shape) : data(std::move(vec)), shape(shape) {
        if (this->shape.get_total_size() != data.size()) {
            throw error::ValueError("Cannot construct NArray because Shape and data size don't match.");
        }
    }
    NArray(const std::vector<dtype>& vec, const Shape& shape) : data(vec), shape(shape) {
        if (this->shape.get_total_size() != data.size()) {
            throw error::ValueError("Cannot construct NArray because Shape and data size don't match.");
        }
    }
    // Shape + initializer value constructor
    NArray(const Shape& shape, dtype val = 0) : data(shape.get_total_size(), val), shape(shape) {}
    NArray(Shape&& shape, dtype val = 0) noexcept : data(shape.get_total_size(), val), shape(std::move(shape)) {}



    /* ====== Operator Overloading ====== */
    // Array addition
    NArray operator+(const NArray& other) const {
        if(!same_shape(other))
            throw error::ShapeError(this->shape, other.shape, "add");
        else
            return elementWiseOp(other, &util::add<dtype>);
    }
    // Array subtraction
    NArray operator-(const NArray& other) const {
        if(!same_shape(other))
            throw error::ShapeError(this->shape, other.shape, "subtract");
        else
            return elementWiseOp(other, &util::subtract<dtype>);
    }
    // Array multiplication
    virtual NArray operator*(const NArray& other) const {
        if(!same_shape(other))
            throw error::ShapeError(this->shape, other.shape, "multiply");
        else
            return elementWiseOp(other, &util::multiply<dtype>);
    }
    // Array division
    NArray operator/(const NArray& other) const {
        if(!same_shape(other))
            throw error::ShapeError(this->shape, other.shape, "divide");
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

    /* Equality Overloads */
    NArray<bool> operator==(const NArray& other) const {
        return elementwiseCompare(other, &util::eq<dtype>);
    }
    NArray<bool> operator!=(const NArray& other) const {
        return elementwiseCompare(other, &util::neq<dtype>);
    }
    NArray<bool> operator<=(const NArray& other) const {
        return elementwiseCompare(other, &util::leq<dtype>);
    }
    NArray<bool> operator>=(const NArray& other) const {
        return elementwiseCompare(other, &util::geq<dtype>);
    }
    NArray<bool> operator<(const NArray& other) const {
        return elementwiseCompare(other, &util::less_than<dtype>);
    }
    NArray<bool> operator>(const NArray& other) const {
        return elementwiseCompare(other, &util::greater_than<dtype>);
    }

    /* Index Overload */
    NArray<dtype> operator[](const int& i) {
        auto index = get_index(i);
        NArray<dtype> out(
            std::vector<dtype>(this->data.begin() + shape[1] * index,
                               this->data.begin() + shape[1] * index + shape.get_total_size() / shape[0]),
            Shape(shape.dimensions.begin() + 1, shape.dimensions.end())
        );
        return out;
    }

    // const dtype& operator[](const int& i) const { return data[get_index(i)]; }

    /* Print Overload */
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
    // Fetches the NArray shape
    const Shape& get_shape() const { return this->shape; }

    // Returns a reference to the data
    const std::vector<dtype>& get_data() const { return this->data; }

    /* NArray functions */
    // Returns a NEW transposed matrix
    NArray transpose() {
        auto out_shape = shape.transpose();
        auto out_data = util::transpose(data, shape);
        return NArray(out_data,out_shape);
    }

    NArray flatten() { return NArray(data); }

    // TODO: ravel should have a flat shape, but not change the shape
    // of the original array. i.e. a.shape != b.shape,
    // but a.data == b.data as in they share the same memory
    NArray& ravel() {
        shape.flatten();
        return *this;
    }

};

/* Deduction guides */
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
