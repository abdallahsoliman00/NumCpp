/* NArray.hpp */
#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <memory>

#include "Shape.hpp"
#include "../Utils/MathOps.hpp"
#include "../Utils/VecOps.hpp"
#include "../Utils/Errors.hpp"
#include "../Utils/Copy.hpp"


namespace numcpp {

template <typename dtype = double>
class NArray {
protected:

    std::shared_ptr<dtype> data_ptr;
    Shape shape;

    // Checks if two shapes are the same
    bool same_shape(const NArray& other) const {
        return shape.same_shape(other.shape);
    }
    
    // Elementwise operation of two NArrays
    NArray elementWiseOp(const NArray &other, std::function<dtype(dtype, dtype)> func) const {
        NArray<dtype> out(this->shape);
        for(size_t i = 0; i < shape.get_total_size(); i++) {
            out.get_data()[i] = func(this->get_data()[i], other.get_data()[i]);
        }
        return out;
    }

    // Right scalar operation to NArrays
    NArray fullVecOpR(const dtype& scalar, std::function<dtype(dtype, dtype)> func) const {
        std::vector<dtype> newVec(shape.get_total_size());
        for(size_t i = 0; i < shape.get_total_size(); i++) {
            newVec[i] = func(get_data()[i], scalar);
        }
        return NArray(std::move(newVec), shape);
    }

    // Left scalar operation to NArrays
    NArray fullVecOpL(const dtype& scalar, std::function<dtype(dtype, dtype)> func) const {
        std::vector<dtype> newVec(shape.get_total_size());
        for(size_t i = 0; i < shape.get_total_size(); i++) {
            newVec[i] = func(scalar, get_data()[i]);
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
            for(size_t i = 0; i < shape.get_total_size(); i++) {
                newVec[i] = comparison_func(get_data()[i], other.get_data()[i]);
            }
            return NArray<bool>(newVec, shape);
        }
    }

    // Print a 1D array
    static void OneDPrint(std::ostream& os, const NArray& arr) {
        os << '[';
        for(size_t i = 0; i < arr.shape[0]; i++) {
        if constexpr (std::is_same_v<dtype, bool>) {
            os << (arr.get_data()[i] ? "true" : "false");
        } else {
            os << arr.get_data()[i];
        }
            if(i != arr.shape[0] - 1) os << ", ";
        }
        os << ']';
    }

    // Recursively prints the N-Dimensional arrays (> 2D arrays)
    static void recursivePrint(std::ostream& os, std::vector<dtype> data, const Shape& sh, int depth = 0) {
        // Base case
        if(sh.get_Ndim() == 1) {
            OneDPrint(os, NArray(data));
            return;
        }

        // Take the first element and split 
        size_t n_grps = sh[0];
        auto groups = util::split(data, n_grps);

        Shape subshape(sh.dimensions.begin() + 1, sh.dimensions.end());
        
        os << '[';
        for (size_t i = 0; i < n_grps; i++) {
            recursivePrint(os, groups[i], subshape, depth + 1);
            if(i != n_grps - 1) {
                if(depth % subshape[0] == 0)
                    os << ",\n\n";
                else
                    os << ",\n" << std::string(n_grps, ' ');
            }
        }
        os << ']';
    }

    size_t get_index(const int& index) const {
        int size = static_cast<int>(shape[0]);

        if((index >= 0) && (index < size))
            return static_cast<size_t>(index);
        else if((index < 0) && (index >= -size))
            return static_cast<size_t>(index + size);
        else
            throw std::runtime_error("Index out of range.");
    }

public:
    /* ====== 1D constructors ====== */
    // Default constructor
    NArray() : data_ptr(nullptr), shape() {}


    // Scalar constructor
    NArray(const dtype& scalar) : data_ptr(new dtype[1], std::default_delete<dtype[]>()), shape({1}) { data_ptr[0] = scalar; }

    
    // Iterator constructor
    template <typename Iter>
    NArray(Iter first, Iter last)
        : data_ptr(new dtype[std::distance(first, last)], std::default_delete<dtype[]>()),
        shape({std::distance(first, last)})
    {
        size_t i = 0;
        for (auto it = first; it != last; ++it, ++i) {
            data_ptr.get()[i] = *it;
        }
    }


    // Vector constructor
    NArray(const std::vector<dtype>& data) :
        data_ptr(new dtype[data.size()], std::default_delete<dtype[]>()),
        shape({static_cast<size_t>(data.size())})
    {
        for(size_t i = 0; i < data.size(); i++)
            data_ptr.get()[i] = data[i];
    }
    NArray(std::vector<dtype>&& data) :
        data_ptr(new dtype[data.size()], std::default_delete<dtype[]>()),
        shape({data.size()})
    {
        std::move(data.begin(), data.end(), data_ptr.get());
    }


    // List constructor
    NArray(std::initializer_list<dtype> list) :
        data_ptr(new dtype[list.size()], std::default_delete<dtype[]>()),
        shape({static_cast<size_t>(list.size())})
    {
        size_t i = 0;
        for(auto item : list)
            data_ptr.get()[i++] = item;
    }


    // Array constructor from heap array (ownership takeover)
    NArray(dtype *array, const size_t& size) : data_ptr(array), shape({size}) {}


    // Array constructor from heap array (copy)
    NArray(copy_t, dtype *array, const size_t& size) :
        data_ptr(new dtype[size], std::default_delete<dtype[]>()),
        shape({size})
    {
        // Use numcpp::copy as the first argument to copy
        std::copy(array, array + size, data_ptr.get());
    }


    // Array constructor from stack array
    template <size_t N>
    NArray(dtype (&array)[N]) : data_ptr(new dtype[N], std::default_delete<dtype[]>()), shape({N}) {
        for(size_t i = 0; i < N; i++)
            data_ptr[i] = array[i];
    }


    // Copy constructor
    NArray(const NArray& oldarray) :
        data_ptr(new dtype[oldarray.shape.get_total_size()], std::default_delete<dtype[]>()),
        shape(oldarray.shape)
    {
        std::copy(
            oldarray.data_ptr.get(),
            oldarray.data_ptr.get() + oldarray.shape.get_total_size(),
            data_ptr.get()
        );
    }


    // Move constructor
    NArray(NArray&& other) noexcept : data_ptr(std::move(other.data_ptr)), shape(std::move(other.shape)) {}


    // Repeat constructor
    NArray(size_t count, dtype val = 0) :
        data_ptr(new dtype[count], std::default_delete<dtype[]>()),
        shape({count})
    {
        for(size_t i = 0; i < count; i++)
            data_ptr[i] = val;
    }


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
        data_ptr = std::shared_ptr<dtype>(new dtype[shape.get_total_size()], std::default_delete<dtype[]>());

        // Add values
        size_t offset = 0;
        for (const NArray& sub : arr) {
            size_t sz = sub.shape.get_total_size();
            std::copy(sub.data_ptr.get(), sub.data_ptr.get() + sz, data_ptr.get() + offset);
            offset += sz;
        }
    }


    // Data + shape constructor (move)
    NArray(std::vector<dtype>&& vec, const Shape& sh) :
        data_ptr(new dtype[vec.size()], std::default_delete<dtype[]>()),
        shape(sh)
    {
        std::move(vec.begin(), vec.end(), data_ptr.get());

        if (this->shape.get_total_size() != vec.size()) {
            throw error::ValueError("Cannot construct NArray because Shape and data size don't match.");
        }
    }


    // Data + shape constructor (copy)
    NArray(const std::vector<dtype>& vec, const Shape& sh) :
        data_ptr(new dtype[vec.size()], std::default_delete<dtype[]>()),
        shape(sh)
    {
        for(size_t i = 0; i < vec.size(); i++)
            data_ptr.get()[i] = vec[i];
        
        if (this->shape.get_total_size() != vec.size()) {
            throw error::ValueError("Cannot construct NArray because Shape and data size don't match.");
        }
    }


    // Shape + initializer value constructor
    NArray(const Shape& shape, dtype val = 0) :
        data_ptr(new dtype[shape.get_total_size()], std::default_delete<dtype[]>()),
        shape(shape)
    {
        for(size_t i = 0; i < shape.get_total_size(); i++)
            data_ptr.get()[i] = val;
    }


    // Constructor from shared_ptr (used for slicing)
    NArray(std::shared_ptr<dtype> base, dtype* slice_start, Shape new_shape)
        : data_ptr(base, slice_start), shape(std::move(new_shape)) {}


    // Shared pointer + shape constructor
    NArray(std::shared_ptr<dtype> sp, const Shape& sh) : data_ptr(sp), shape(sh) {}
    NArray(std::shared_ptr<dtype>&& sp, const Shape& sh) : data_ptr(std::move(sp)), shape(sh) {}



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
    NArray<dtype> operator[](const int& i) const {
        auto index = get_index(i);
        auto slice_start = data_ptr.get() + shape[1] * index;
        auto new_shape = Shape(shape.dimensions.begin() + 1, shape.dimensions.end());

        return NArray<dtype>(data_ptr, slice_start, new_shape);
    }


    /* Print Overload */
    friend std::ostream& operator<<(std::ostream& os, const NArray& arr) {
        switch(arr.shape.get_Ndim()) {
        case 1: // 1D vector
            OneDPrint(os, arr);
            break;

        case 2: { // Martix
            os << '[';
            auto grps = util::split(arr.get_data_as_vector(), arr.shape[0]);
            for(size_t i = 0; i < grps.size(); i++) {
                OneDPrint(os, NArray(grps[i]));
                if(i != grps.size() - 1) os << ",\n ";
            }
            os << ']';
            break;
        }

        default: // Anything else
            recursivePrint(os, arr.get_data_as_vector(), arr.shape);
            break;
        }
        return os;
    }

    /* Helper functions */
    // Fetches the NArray shape
    const Shape& get_shape() const { return this->shape; }

    // Returns a pointer to the data
    dtype* get_data() const { return this->data_ptr.get(); }

    // Returns the data as an std::vector
    std::vector<dtype> get_data_as_vector() const {
        return std::vector<dtype>(data_ptr.get(), data_ptr.get() + shape.get_total_size());
    }

    // Returns a shared pointer to newly allocated heap memory with the data
    std::shared_ptr<dtype> get_data_copy_as_shared_ptr() const {
        std::shared_ptr<dtype> out(new dtype[shape.get_total_size()], std::default_delete<dtype[]>());
        std::copy(data_ptr.get(), data_ptr.get() + shape.get_total_size(), out.get());
        return out;
    }

    /* NArray functions */
    // Returns a NEW transposed matrix
    NArray transpose() const {
        auto out_shape = shape.transpose();
        std::shared_ptr<dtype> out_data_ptr(new dtype[shape.get_total_size()], std::default_delete<dtype[]>());
        util::transpose(out_data_ptr.get(), data_ptr.get(), shape);
        return NArray(out_data_ptr, out_shape);
    }

    // Returns a new flat vector
    NArray flatten() const { return NArray(get_data_copy_as_shared_ptr(), shape.flatten()); }

    // Returns a flat view of the array
    NArray ravel() { return NArray(data_ptr, shape.flatten()); }

    // Returns a copy of the array
    NArray copy() { return NArray(*this); }

};

// TODO: Add deduction guides for the new constructors
/* Deduction guides */
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

// Data + Shape constructor
template <typename T>
NArray(std::vector<T>, Shape) -> NArray<T>;

} // namespace numcpp
