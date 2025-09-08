/* NArray.hpp */
#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include "../Utils/Operations.hpp"
#include "../Utils/Shape.hpp"


std::ostream& operator<<(std::ostream& os, const std::vector<bool>& vec) {
    os << '[';
    for (size_t i = 0; i < vec.size(); ++i) {
        os << (vec[i] ? "true" : "false");
        if (i != vec.size() - 1) os << ", ";
    }
    os << ']';
    return os;
}

namespace numcpp {

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
            if(shape.get_Ndim() == 1) {
                std::vector<float> newVec(shape[0]);
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

    NArray fullVecOp(const float& other, std::function<float(float, float)> func) const {
        if(shape.get_Ndim() == 1) {
            std::vector<float> newVec(shape[0]);
            for(int i = 0; i < shape[0]; i++) {
                newVec[i] = func(vec[i], other);
            }
            return NArray(std::move(newVec));
        }
        else {
            // TODO
            std::cout << "Not implemented yet." << std::endl;
            return *this;
        }
    }

    std::vector<bool> checkEquality(const NArray& other, const bool& eq = true) const {
        if(!same_shape(other)) {
            std::cerr << "[ValueError]: Unable to compare Vectors. Cannot compare shapes " 
                      << shape << " and " << other.shape << '.' << std::endl;
            return std::vector<bool>(0);
        }
        else {
            if(shape.get_Ndim() == 1) {
                std::vector<bool> newVec(shape[0]);
                for(int i = 0; i < shape[0]; i++) {
                    newVec[i] = eq ? (vec[i] == other.vec[i]) : (vec[i] != other.vec[i]);
                }
                return std::move(newVec);
            }
            else {
                // TODO
                std::cout << "Not implemented yet." << std::endl;
                return std::vector<bool>(0);
            }
        }
    }

    static void recursivePrint(std::ostream& os, const std::vector<float>& data, const Shape& shape) {
        return;
    }

    size_t get_index(const int& index) const {
        int size = static_cast<int>(vec.size());

        if((index >= 0) && (index < size))
            return static_cast<size_t>(index);
        else if((index < 0) && (index >= -size))
            return static_cast<size_t>(index + size);
        else
            throw std::runtime_error("Index out of range.\n");
    }

public:
    /* 1D constructors */
    // Default constructor
    NArray() : vec(), shape() {}
    // Vector constructor
    NArray(std::vector<float> vec) : vec(vec), shape({vec.size()}) {}
    // List constructor
    NArray(std::initializer_list<float> list) : vec(list), shape({vec.size()}) {}
    // Array constructor
    NArray(float *array, size_t size) : vec(array, array + size), shape({size}) {}
    // Copy constructor
    NArray(const NArray& newVec) : vec(newVec.vec), shape(newVec.shape) {}
    // Move constructor
    NArray(NArray&& other) noexcept : vec(std::move(other.vec)), shape(std::move(other.shape)) {}
    // Repeat constructor
    NArray(size_t count, float num = 0) : vec(count, num), shape({count}) {}

    /* N-Dimensional contructor */
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

    std::vector<bool> operator!=(const NArray& other) const {
        return checkEquality(other, false);
    }

    float& operator[](const int& i) { return vec[get_index(i)]; }

    const float& operator[](const int& i) const { return vec[get_index(i)]; }

    friend std::ostream& operator<<(std::ostream& os, const NArray& arr) {
        switch(arr.shape.get_Ndim()) {
        case 1: // 1D vector
            os << '[';
            for(int i = 0; i < arr.shape[0]; i++) {
                os << arr.vec[i];
                if(i != arr.shape[0] - 1) os << ", ";
            }
            os << ']';
            break;

        case 2: // Martix
            // TODO: Complete when multidimensional arrays are complete.
            std::cout << '\n';
            break;

        default: // Anything else
            os << '[';
            for(const auto &dim : arr.shape.dimensions) {

            }
            os << ']';
            break;
        }
        return os;
} 

    /* Helper functions */
    const Shape& get_shape() const { return this->shape; }
};
}

