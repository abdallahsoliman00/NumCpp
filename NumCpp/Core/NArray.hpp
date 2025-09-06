/* NArray.hpp */
#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include "../utils/operations.hpp"


std::ostream& operator<<(std::ostream& os, const std::vector<bool>& vec) {
    os << '[';
    for (size_t i = 0; i < vec.size(); ++i) {
        os << (vec[i] ? "true" : "false");
        if (i != vec.size() - 1) os << ", ";
    }
    os << ']';
    return os;
}

namespace nc {

struct Shape {
    std::vector<size_t> dimensions;

    Shape(std::initializer_list<size_t> dims) :  dimensions(dims) {}
    
    Shape() : dimensions({}) {}

    void reshape(std::initializer_list<size_t> dims) { dimensions = dims; }

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


class NArray {
private:
    std::vector<float> vec;
    Shape shape;
    
protected:    
    bool same_shape(const NArray& other) const {
        return shape.same_shape(other.shape);
    }
    NArray elementWiseOp(const NArray &other, std::function<float(float, float)> func, const char* opName) const {
        if(!same_shape(other)) {
            std::cerr << "[ValueError]: Unable to " << opName << " Vectors. Cannot " << opName <<
                        " shapes " << shape << " and " << other.shape << '.' << std::endl;
            return *this;
        }
        else {
            if(shape[0] == 1) {
                std::vector<float> newVec(shape[1]);
                for(int i = 0; i < shape[1]; i++) {
                    newVec[i] = func(vec[i], other.vec[i]);
                }
                return NArray(newVec);
            }
            else {
                // TODO
                std::cout << "Not implemented yet." << std::endl;
                return *this;
            }
        }
    }

    NArray fullVecOp(const float& other, std::function<float(float, float)> func) const {
        if(shape[0] == 1) {
            std::vector<float> newVec(shape[1]);
            for(int i = 0; i < shape[1]; i++) {
                newVec[i] = func(vec[i], other);
            }
            return NArray(newVec);
        }
        else {
            // TODO
            std::cout << "Not implemented yet." << std::endl;
            return *this;
        }
    }

    std::vector<bool> checkEquality(const NArray& other) const {
        if(!same_shape(other)) {
            std::cerr << "[ValueError]: Unable to compare Vectors. Cannot compare shapes " 
                      << shape << " and " << other.shape << '.' << std::endl;
            return std::vector<bool>(0);
        }
        else {
            if(shape[0] == 1) {
                std::vector<bool> newVec(shape[1]);
                for(int i = 0; i < shape[1]; i++) {
                    newVec[i] = (vec[i] == other.vec[i]);
                }
                return newVec;
            }
            else {
                // TODO
                std::cout << "Not implemented yet." << std::endl;
                return std::vector<bool>(0);
            }
        }
    }

public:
    /* 1D constructors */
    // Default constructor
    NArray() : vec(), shape({1,0}) {}
    // Vector constructor
    NArray(std::vector<float> vec) : vec(vec), shape({1, vec.size()}) {}
    // List constructor
    NArray(std::initializer_list<float> list) : vec(list), shape({1,vec.size()}) {}
    // Array constructor
    NArray(float *array, size_t size) : vec(array, array + size), shape({1, size}) {}
    // Copy constructor
    NArray(const NArray& newVec) : vec(newVec.vec), shape(newVec.shape) {}
    // Move constructor
    NArray(NArray&& other) noexcept : vec(std::move(other.vec)), shape(std::move(other.shape)) {}
    // Repeat constructor
    NArray(size_t count, float num = 0) : vec(count, num), shape({1, count}) {}

    /* N-Dimensional contructor */
    NArray(std::initializer_list<NArray> arr) {
        // Check if empty
        if(!arr.size()) return;

        // Read first dimension
        shape.dimensions.push_back(arr.size());

        // Continue adding dimensions recursively until base case is reached
        const NArray& first = *arr.begin();
        
        if(first.shape[0] == 1) shape.dimensions.push_back(first.shape[1]);
        else {
        shape.dimensions.insert(shape.dimensions.end(),
                        first.shape.dimensions.begin(),
                        first.shape.dimensions.end());
        }

        // Consistency check
        for (const NArray& sub : arr) {
            if (sub.shape[1] != first.shape[1]) {
                throw std::runtime_error("Jagged initializer lists are not supported");
            }
        }

        // Reserve space for values
        vec.reserve(shape.get_total_size());

        // Add values
        for (const NArray& sub : arr) {
            vec.insert(vec.end(), sub.vec.begin(), sub.vec.end());
        }
    }
    

    /* Operator overloading*/

    NArray operator+(const NArray& other) const {
        return elementWiseOp(other, &util::add<float>, "add");
    }
    NArray operator-(const NArray& other) const {
        return elementWiseOp(other, &util::subtract<float>, "subtract");
    }
    NArray operator*(const NArray& other) const {
        return elementWiseOp(other, &util::multiply<float>, "multiply");
    }
    NArray operator/(const NArray& other) const {
        return elementWiseOp(other, &util::divide<float>, "divide");
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    friend NArray operator+(T num, const NArray& arr) {
        return arr.fullVecOp(static_cast<float>(num), &util::add<float>);
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    friend NArray operator-(T num, const NArray& arr) {
        return arr.fullVecOp(static_cast<float>(num), &util::subtract<float>);
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    friend NArray operator*(T num, const NArray& arr) {
        return arr.fullVecOp(static_cast<float>(num), &util::multiply<float>);
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    friend NArray operator/(T num, const NArray& arr) {
        return arr.fullVecOp(static_cast<float>(num), &util::divide<float>);
    }

    friend NArray operator+(float num, const NArray& arr) {
        return arr.fullVecOp(num, &util::add<float>);
    }
    friend NArray operator-(float num, const NArray& arr) {
        return arr.fullVecOp(num, &util::subtract<float>);
    }
    friend NArray operator*(float num, const NArray& arr) {
        return arr.fullVecOp(num, &util::multiply<float>);
    }
    friend NArray operator/(float num, const NArray& arr) {
        return arr.fullVecOp(num, &util::divide<float>);
    }

    std::vector<bool> operator==(const NArray& other) const {
        return checkEquality(other);
    }

    float& operator[](size_t i) { return vec.at(i); }

    const float& operator[](size_t i) const { return vec.at(i); }

    friend std::ostream& operator<<(std::ostream& os, const NArray& nVec) {
        if(nVec.shape[0] == 1) {
            os << '[';
            for(int i = 0; i < nVec.shape[1]; i++) {
                os << nVec.vec[i];
                if(i != nVec.shape[1] - 1) os << ", ";
            }
            os << ']';
        } 
        else {
            // TODO: Complete when multidimensional arrays are complete.
            // for(int i = 0; i < nVec.size[0]; i++) {
            //     for(int j = 0; j < nVec.size[1]; j++) {
            //         os << nVec.vec[i][j] << ',';
            //     }
            // }
            os << "Still incomplete.\n";
        }
        return os;
    } 

    /* Helper functions */
    Shape get_shape() { return this->shape; }
};
}

