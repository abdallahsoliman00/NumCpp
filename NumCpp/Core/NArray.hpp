/* NArray.hpp */
#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include "Shape.hpp"
#include "../Utils/MathOps.hpp"
#include "../Utils/VecOps.hpp"


namespace numcpp {

template <typename dtype = double>
class NArray {
private:
    std::vector<dtype> vec;
    Shape shape;
    
protected:    
    bool same_shape(const NArray& other) const {
        return shape.same_shape(other.shape);
    }
    
    NArray elementWiseOp(const NArray &other, std::function<dtype(dtype, dtype)> func, const char* opName) const {
        if(!same_shape(other)) {
            std::cerr << "[ValueError]: Unable to " << opName << " Vectors. Cannot " << opName <<
                        " shapes " << shape << " and " << other.shape << '.' << std::endl;
            throw std::runtime_error("");
        }
        else {
            if(shape.get_Ndim() == 1) {
                std::vector<dtype> newVec(shape[0]);
                for(size_t i = 0; i < shape[0]; i++) {
                    newVec[i] = func(vec[i], other.vec[i]);
                }
                return NArray(std::move(newVec));
            }
            else {
                // TODO
                std::cout << "Not implemented yet." << std::endl;
                return *this;
            }
        }
    }

    NArray fullVecOp(const float& other, std::function<dtype(dtype, dtype)> func) const {
        std::vector<dtype> newVec(shape.get_total_size());
        for(int i = 0; i < shape.get_total_size(); i++) {
            newVec[i] = func(vec[i], other);
        }
        return NArray(std::move(newVec), shape);
    }

    NArray<bool> checkEquality(const NArray& other, const bool& eq = true) const {
        if(!same_shape(other)) {
            std::cerr << "[ValueError]: Unable to compare Vectors. Cannot compare shapes " 
                      << shape << " and " << other.shape << '.' << std::endl;
            throw std::runtime_error("");
        }
        else {
            std::vector<bool> newVec(shape.get_total_size());
            for(int i = 0; i < shape.get_total_size(); i++) {
                newVec[i] = eq ? (vec[i] == other.vec[i]) : (vec[i] != other.vec[i]);
            }
            return NArray<bool>(newVec, shape);
        }
    }

    static void OneDPrint(std::ostream& os, const NArray& arr) {
        os << '[';
        for(int i = 0; i < arr.shape[0]; i++) {
        if constexpr (std::is_same_v<dtype, bool>) {
            os << (arr.vec[i] ? "true" : "false");
        } else {
            os << arr.vec[i];
        }
            if(i != arr.shape[0] - 1) os << ", ";
        }
        os << ']';
    }

    static void recursivePrint(std::ostream& os, const NArray& arr) {
        // Base case
        if(arr.shape.get_Ndim() == 1) {
            OneDPrint(os, arr);
            return;
        }

        // Take the first element and split 
        size_t n_grps = arr.shape[0];
        auto groups = util::split(arr.vec, n_grps);

        Shape subshape(arr.shape.dimensions.begin() + 1, arr.shape.dimensions.end());
        
        os << '[';
        for (size_t i = 0; i < n_grps; i++) {
            recursivePrint(os, NArray(groups[i], subshape));
            if (i != n_grps - 1) os << ", ";
        }
        os << ']';

    }

    size_t get_index(const int& index) const {
        int size = static_cast<int>(vec.size());

        if((index >= 0) && (index < size))
            return static_cast<size_t>(index);
        else if((index < 0) && (index >= -size))
            return static_cast<size_t>(index + size);
        else
            throw std::runtime_error("Index out of range.");
    }

public:
    /* 1D constructors */
    // Default constructor
    NArray() : vec(), shape() {}
    // Vector constructor
    NArray(const std::vector<dtype>& vec) : vec(vec), shape({vec.size()}) {}
    // List constructor
    NArray(std::initializer_list<dtype> list) : vec(list), shape({list.size()}) {}
    // Array constructor
    NArray(dtype *array, size_t size) : vec(array, array + size), shape({size}) {}
    // Copy constructor
    NArray(const NArray& newVec) : vec(newVec.vec), shape(newVec.shape) {}
    // Move constructor
    NArray(NArray&& other) noexcept : vec(std::move(other.vec)), shape(std::move(other.shape)) {}
    // Repeat constructor
    NArray(size_t count, dtype val = 0) : vec(count, val), shape({count}) {}

    /* N-Dimensional contructors */
    // Recursive constructor
    NArray(std::initializer_list<NArray> arr) {
        // Check if empty
        if(!arr.size()) return;

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
        vec.reserve(shape.get_total_size());

        // Add values
        for (const NArray& sub : arr) {
            vec.insert(vec.end(), sub.vec.begin(), sub.vec.end());
        }
    }
    // Data + shape constructor
    NArray(std::vector<dtype> data, Shape shape) : vec(std::move(data)), shape(std::move(shape)) {
        if (this->shape.get_total_size() != vec.size()) {
            throw std::runtime_error("Shape and data size don't match. Please check data and shape.");
        }
    }
    // Empty shape constructor
    NArray(const Shape& shape) : vec(shape.get_total_size()), shape(shape) {}
    

    /* Operator overloading*/

    NArray operator+(const NArray& other) const {
        return elementWiseOp(other, &util::add<dtype>, "add");
    }
    NArray operator-(const NArray& other) const {
        return elementWiseOp(other, &util::subtract<dtype>, "subtract");
    }
    NArray operator*(const NArray& other) const {
        return elementWiseOp(other, &util::multiply<dtype>, "multiply");
    }
    NArray operator/(const NArray& other) const {
        return elementWiseOp(other, &util::divide<dtype>, "divide");
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    friend NArray operator+(T num, const NArray& arr) {
        return arr.fullVecOp(static_cast<dtype>(num), &util::add<dtype>);
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    friend NArray operator-(T num, const NArray& arr) {
        return arr.fullVecOp(static_cast<dtype>(num), &util::subtract<dtype>);
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    friend NArray operator*(T num, const NArray& arr) {
        return arr.fullVecOp(static_cast<dtype>(num), &util::multiply<dtype>);
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    friend NArray operator/(T num, const NArray& arr) {
        return arr.fullVecOp(static_cast<dtype>(num), &util::divide<dtype>);
    }

    friend NArray operator+(float num, const NArray& arr) {
        return arr.fullVecOp(num, &util::add<dtype>);
    }
    friend NArray operator-(float num, const NArray& arr) {
        return arr.fullVecOp(num, &util::subtract<dtype>);
    }
    friend NArray operator*(float num, const NArray& arr) {
        return arr.fullVecOp(num, &util::multiply<dtype>);
    }
    friend NArray operator/(float num, const NArray& arr) {
        return arr.fullVecOp(num, &util::divide<dtype>);
    }

    NArray<bool> operator==(const NArray& other) const {
        return checkEquality(other);
    }

    NArray<bool> operator!=(const NArray& other) const {
        return checkEquality(other, false);
    }

    // TODO: Make these operators properly ined the n-dimensinal arrays.
    // Use recursion to get the correct arrays?
    dtype& operator[](const int& i) { return vec[get_index(i)]; }

    const dtype& operator[](const int& i) const { return vec[get_index(i)]; }

    friend std::ostream& operator<<(std::ostream& os, const NArray& arr) {
        switch(arr.shape.get_Ndim()) {
        case 1: // 1D vector
            OneDPrint(os, arr);
            break;

        case 2: { // Martix
            os << '[';
            auto grps = util::split(arr.vec, arr.shape[0]);
            for(size_t i = 0; i < grps.size(); i++) {
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

    NArray flatten() { return NArray(vec); }

    // TODO: ravel should have a flat shape, but not change the shape
    // of the original array. i.e. a.shape != b.shape,
    // but a.vec == b.vec as in they share the same memory
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
NArray(T*, size_t) -> NArray<T>;

// Repeat constructor
template <typename T>
NArray(size_t, T) -> NArray<T>;

}

