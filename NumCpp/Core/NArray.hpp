/* NArray.hpp */
#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <memory>

#include "Shape.hpp"
#include "../Utils/VecOps.hpp"
#include "../Utils/Errors.hpp"
#include "../Utils/Copy.hpp"


namespace numcpp {

template <typename dtype = double>
class NArray {
protected:
    /* ====== Member Variables ====== */

    std::shared_ptr<dtype> _data_ptr;
    Shape _shape;


public:
    /* ====== Helper functions ====== */

    // Fetches the NArray shape
    const Shape& get_shape() const { return this->_shape; }


    // Returns a pointer `dtype*` to the data
    dtype* get_data() const { return this->_data_ptr.get(); }


    // Returns the data as an std::vector
    std::vector<dtype> get_data_as_vector() const {
        return std::vector<dtype>(_data_ptr.get(), _data_ptr.get() + _shape.get_total_size());
    }


    // Returns a shared pointer to newly allocated heap memory with the data
    std::shared_ptr<dtype> get_data_copy_as_shared_ptr() const {
        std::shared_ptr<dtype> out(new dtype[_shape.get_total_size()], std::default_delete<dtype[]>());
        std::copy(_data_ptr.get(), _data_ptr.get() + _shape.get_total_size(), out.get());
        return out;
    }


    // Checks if two shapes are the same
    static bool same_shape(const NArray& a, const NArray& b) {
        return a._shape.same_shape(b._shape);
    }


    size_t get_total_size() const {
        return _shape.get_total_size();
    }



protected:
    /* ====== Helper Functions ====== */

    // begin iterator over the NArray's data
    dtype* begin() { return _data_ptr.get(); }

    // end iterator over the NArray's data
    dtype* end() { return _data_ptr.get() + _shape.get_total_size(); }


    // Gets the total size required to store multiple NArrays
    static void get_size_requirements(
        size_t& size, int& depth, const NArray& arr
    ) {
        size += arr.get_total_size();
        depth++;
    }

    template <typename... Arrays>
    static void get_size_requirements(
        size_t& size, int& depth, 
        const NArray& arr, const NArray& next, 
        const Arrays&... rest
    ) {
        if(same_shape(arr, next)) {
            size += arr.get_total_size();
            depth++;
            get_size_requirements(size, depth, next, rest...);
        } else {
            throw error::ValueError("Could not initialise array because subarrays do not have the same shape.");
        }
    }


    // Adds the data to the array given its pointer
    static void add_data_to_data_ptr(
        const std::shared_ptr<dtype>& data_ptr, size_t starting_pos,
        const NArray& arr
    ) {
        std::copy(
            arr._data_ptr.get(),
            arr._data_ptr.get() + arr.get_total_size(),
            data_ptr.get() + starting_pos
        );
    }

    template <typename... Arrays>
    static void add_data_to_data_ptr(
        const std::shared_ptr<dtype>& data_ptr, size_t starting_pos,
        const NArray& first, const Arrays&... rest
    ) {
        // Copy first array's elements to data_ptr
        std::copy(
            first._data_ptr.get(),
            first._data_ptr.get() + first.get_total_size(),
            data_ptr.get() + starting_pos
        );
        // Pass the rest recursively
        add_data_to_data_ptr(data_ptr, starting_pos + first.get_total_size(), rest...);
    }


    // Enables Python-like indexing with negative indexes wrapping around
    size_t get_index(const int& index) const {
        int size = static_cast<int>(_shape[0]);

        if((index >= 0) && (index < size))
            return static_cast<size_t>(index);
        else if((index < 0) && (index >= -size))
            return static_cast<size_t>(index + size);
        else
            throw std::runtime_error("Index out of range.");
    }


    // Elementwise operation of two NArrays
    template <typename T, typename Func>
    auto elementWiseOp(
        const NArray<T> &other,
        Func func
    ) const -> NArray<decltype(func(std::declval<dtype>(), std::declval<T>()))>
    {
        using U = decltype(func(std::declval<dtype>(), std::declval<T>()));
        NArray<U> out(_shape);

        for(size_t i = 0; i < _shape.get_total_size(); i++) {
            out.get_data()[i] = func(this->get_data()[i], other.get_data()[i]);
        }
        return out;
    }


    // Performs the scalar operation to the right of each element in the array
    template <typename T, typename Func>
    auto fullVecOpR(const T scalar, Func func)
        const -> NArray<decltype(func(std::declval<dtype>(), std::declval<T>()))>
    {
        using U = decltype(func(std::declval<dtype>(), std::declval<T>()));

        std::vector<U> newVec(_shape.get_total_size());
        for(size_t i = 0; i < _shape.get_total_size(); i++) {
            newVec[i] = func(get_data()[i], scalar);
        }
        return NArray<U>(std::move(newVec), _shape);
    }


    // Performs the scalar operation to the left of each element in the array
    template <typename T, typename Func>
    auto fullVecOpL(const T scalar, Func func)
        const -> NArray<decltype(func(std::declval<T>(), std::declval<dtype>()))>
    {
        using U = decltype(func(std::declval<T>(), std::declval<dtype>()));

        std::vector<dtype> newVec(_shape.get_total_size());
        for(size_t i = 0; i < _shape.get_total_size(); i++) {
            newVec[i] = func(scalar, get_data()[i]);
        }
        return NArray(std::move(newVec), _shape);
    }


    // Returns a mask of which NArray elements are the same
    template <typename T, typename Func>
    NArray<bool> elementwiseCompare(const NArray& other, Func comparison_func) const {
        if(!same_shape(*this, other)) {
            throw error::ShapeError(this->_shape, other._shape, "compare");
        }
        else {
            std::vector<bool> newVec(_shape.get_total_size());
            for(size_t i = 0; i < _shape.get_total_size(); i++) {
                newVec[i] = comparison_func(get_data()[i], other.get_data()[i]);
            }
            return NArray<bool>(std::move(newVec), _shape);
        }
    }


    // Print a 1D array
    static void OneDPrint(
        std::ostream& os, const NArray& arr, const util::PrintAttributes& attributes
    ) {
        os << '[';
        for(size_t i = 0; i < arr._shape[0]; i++) {
        if constexpr (std::is_same_v<dtype, bool>) {
            os << (arr.get_data()[i] ? " true" : " false");
        } else {
            os << util::num_to_str_from_attributes(arr.get_data()[i], attributes);
        }
            if(i < arr._shape[0] - 1) os << ' ';
        }
        os << ']';
    }


    // Recursively prints the N-Dimensional arrays (> 2D arrays)
    static void recursivePrint(
        std::ostream& os, std::vector<dtype> data, const Shape& shape,
        const util::PrintAttributes& attributes, int depth = 0
    ) {
        // Base case
        if(shape.get_Ndim() == 1) {
            OneDPrint(os, NArray(data), attributes);
            return;
        }

        // Take the first element and split 
        size_t n_grps = shape[0];
        auto groups = util::split(data, n_grps);

        Shape subshape(shape.dimensions.begin() + 1, shape.dimensions.end());
        
        os << '[';
        for (size_t i = 0; i < n_grps; i++) {
            if (i > 0) {
                os << ",";
                if (depth == 0)
                    os << "\n\n";
                else
                    os << "\n" << std::string(depth + 1, ' ');
            }
            recursivePrint(os, groups[i], subshape, attributes, depth + 1);
        }
        os << ']';
    }



public:
    /* ====== 1D constructors ====== */

    // Default constructor
    NArray() : _data_ptr(nullptr), _shape() {}


    // Scalar constructor
    NArray(const dtype& num) : _data_ptr(new dtype[1], std::default_delete<dtype[]>()), _shape({1})
        { _data_ptr.get()[0] = num; }

    
    // Iterator constructor
    template <typename Iter>
    NArray(Iter first, Iter last) :
        _data_ptr(new dtype[static_cast<size_t>(std::distance(first, last))], std::default_delete<dtype[]>()),
        _shape(static_cast<size_t>(std::distance(first, last)))
    {
        if (std::distance(first, last) < 0)
            throw std::invalid_argument("NArray: iterator range is invalid (last before first)");

        size_t i = 0;
        for (auto it = first; it != last; ++it, ++i) {
            _data_ptr.get()[i] = *it;
        }
    }


    // Vector constructor
    NArray(const std::vector<dtype>& data) :
        _data_ptr(new dtype[data.size()], std::default_delete<dtype[]>()),
        _shape({static_cast<size_t>(data.size())})
    {
        std::copy(data.begin(), data.end(), _data_ptr.get());
    }

    NArray(std::vector<dtype>&& data) :
        _data_ptr(new dtype[data.size()], std::default_delete<dtype[]>()),
        _shape({data.size()})
    {
        std::move(data.begin(), data.end(), _data_ptr.get());
    }


    // initializer_list constructor
    NArray(std::initializer_list<dtype> list) :
        _data_ptr(new dtype[list.size()], std::default_delete<dtype[]>()),
        _shape({static_cast<size_t>(list.size())})
    {
        size_t i = 0;
        for(auto item : list)
            _data_ptr.get()[i++] = item;
    }


    // Array constructor from heap array (ownership takeover)
    // Warning: Use with caution. Memory must be managed manually as std::shared_ptr won't manage it properly.
    NArray(dtype *array, const size_t& size) : _data_ptr(array), _shape({size}) {}


    // Array constructor from heap array (copy)
    NArray(copy_t, dtype *array, const size_t& size) :
        _data_ptr(new dtype[size], std::default_delete<dtype[]>()),
        _shape({size})
    {
        // Use numcpp::copy as the first argument to copy
        std::copy(array, array + size, _data_ptr.get());
    }


    // Array constructor from stack array
    template <size_t N>
    NArray(dtype (&array)[N]) : _data_ptr(new dtype[N], std::default_delete<dtype[]>()), _shape({N}) {
        for(size_t i = 0; i < N; i++)
            _data_ptr.get()[i] = array[i];
    }


    // Copy constructor
    NArray(const NArray& oldarray) :
        _data_ptr(new dtype[oldarray._shape.get_total_size()], std::default_delete<dtype[]>()),
        _shape(oldarray._shape)
    {
        std::copy(
            oldarray._data_ptr.get(),
            oldarray._data_ptr.get() + oldarray._shape.get_total_size(),
            _data_ptr.get()
        );
    }


    // Move constructor
    NArray(NArray&& other) noexcept : _data_ptr(std::move(other._data_ptr)), _shape(std::move(other._shape)) {}


    // Repeat constructor
    NArray(size_t count, dtype val) :
        _data_ptr(new dtype[count], std::default_delete<dtype[]>()),
        _shape({count})
    {
        for(size_t i = 0; i < count; i++)
            _data_ptr.get()[i] = val;
    }


    /* ====== N-Dimensional contructors ====== */

    // Recursive constructor
    NArray(std::initializer_list<NArray> arr) {
        // Check if empty
        if(!arr.size()) return; // Base case

        // Read first dimension
        _shape.dimensions.push_back(arr.size());

        // Continue adding dimensions recursively until base case is reached
        const NArray& first = *arr.begin();
        _shape.dimensions.insert(_shape.dimensions.end(),
                                first._shape.dimensions.begin(),
                                first._shape.dimensions.end());

        // Consistency check
        for (const NArray& sub : arr) {
            if (sub._shape.dimensions != first._shape.dimensions) {
                throw error::ValueError("Jagged initializer lists are not supported.");
            }
        }

        // Reserve space for values
        _data_ptr = std::shared_ptr<dtype>(new dtype[_shape.get_total_size()], std::default_delete<dtype[]>());

        // Add values
        size_t offset = 0;
        for (const NArray& sub : arr) {
            size_t sz = sub._shape.get_total_size();
            std::copy(sub._data_ptr.get(), sub._data_ptr.get() + sz, _data_ptr.get() + offset);
            offset += sz;
        }
    }


    // Data + shape constructor (move)
    NArray(std::vector<dtype>&& vec, const Shape& shape) :
        _data_ptr(new dtype[vec.size()], std::default_delete<dtype[]>()),
        _shape(shape)
    {
        if (this->_shape.get_total_size() != vec.size()) {
            throw error::ValueError("Cannot construct NArray because Shape and data size don't match.");
        }
        std::move(vec.begin(), vec.end(), _data_ptr.get());
    }


    // Data + shape constructor (copy)
    NArray(const std::vector<dtype>& vec, const Shape& shape) :
        _data_ptr(new dtype[vec.size()], std::default_delete<dtype[]>()),
        _shape(shape)
    {
        if (this->_shape.get_total_size() != vec.size()) {
            throw error::ValueError("Cannot construct NArray because Shape and data size don't match.");
        }
        
        std::copy(vec.begin(), vec.end(), _data_ptr.get());
    }


    // Data + shape constructor (from initializer_list)
    NArray(std::initializer_list<dtype> list, const Shape& shape) :
        _data_ptr(new dtype[list.size()], std::default_delete<dtype[]>()),
        _shape(shape)
    {
        if (this->_shape.get_total_size() != list.size()) {
            throw error::ValueError("Cannot construct NArray because Shape and data size don't match.");
        }

        size_t i = 0;
        for(auto item : list)
            _data_ptr.get()[i++] = item;
    }


    // Shape + initializer value constructor
    NArray(const Shape& shape, dtype val) :
        _data_ptr(new dtype[shape.get_total_size()], std::default_delete<dtype[]>()),
        _shape(shape)
    {
        for(size_t i = 0; i < _shape.get_total_size(); i++)
            _data_ptr.get()[i] = val;
    }

    NArray(Shape&& shape, dtype val) :
        _data_ptr(new dtype[shape.get_total_size()], std::default_delete<dtype[]>()),
        _shape(std::move(shape))
    {
        for(size_t i = 0; i < _shape.get_total_size(); i++)
            _data_ptr.get()[i] = val;
    }


    // Empty shape constructor
    NArray(const Shape& shape) :
        _data_ptr(new dtype[shape.get_total_size()], std::default_delete<dtype[]>()),
        _shape(shape) {}
    
    NArray(Shape&& shape) :
        _data_ptr(new dtype[shape.get_total_size()], std::default_delete<dtype[]>()),
        _shape(std::move(shape)) {}


    // Constructor from shared_ptr (used for slicing)
    NArray(std::shared_ptr<dtype> base, dtype* slice_start, Shape&& new_shape)
        : _data_ptr(base, slice_start), _shape(std::move(new_shape)) {}

    NArray(std::shared_ptr<dtype> base, dtype* slice_start, const Shape& new_shape)
        : _data_ptr(base, slice_start), _shape(new_shape) {}


    // Shared pointer + shape constructor
    NArray(std::shared_ptr<dtype> sp, const Shape& shape) : _data_ptr(sp), _shape(shape) {}
    NArray(const std::shared_ptr<dtype>& sp, Shape&& shape) : _data_ptr(sp), _shape(std::move(shape)) {}


    // Raw pointer + shape constructor
    // Warning: Use with caution. Memory must be managed manually as std::shared_ptr won't manage it properly.
    NArray(dtype* ptr, const Shape& shape) : _data_ptr(ptr), _shape(shape) {}
    NArray(dtype* ptr, Shape&& shape) : _data_ptr(ptr), _shape(std::move(shape)) {}


    // Constructor from other NArrays
    template <typename... Arrays>
    NArray(const NArray& first, const NArray& next, const Arrays&... rest) : _shape(first._shape) {
        // Get size requirements for new NArray
        size_t total_size = 0;
        int new_dims = 0;
        get_size_requirements(total_size, new_dims, first, next, rest...);

        // Initialise _shape and _data_ptr
        _shape.insert_dimension(new_dims, 0);
        _data_ptr = std::shared_ptr<dtype>(new dtype[total_size], std::default_delete<dtype[]>());

        // Add data
        add_data_to_data_ptr(_data_ptr, 0, first, next, rest...);
    }


    /* ====== Destructor ====== */
    virtual ~NArray() = default;



    /* ====== Operator Overloading ====== */

    // Array addition
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    auto operator+(const NArray<T>& other) const {
        if(!same_shape(*this, other))
            throw error::ShapeError(this->_shape, other._shape, "add");
        else
            return elementWiseOp(other, [] (auto a, auto b) { return a + b; } );
    }


    // Array subtraction
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    auto operator-(const NArray<T>& other) const {
        if(!same_shape(*this, other))
            throw error::ShapeError(this->_shape, other._shape, "subtract");
        else
            return elementWiseOp(other, [] (auto a, auto b) { return a - b; } );
    }


    // Array multiplication
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    auto operator*(const NArray<T>& other) const {
        if(!same_shape(*this, other))
            throw error::ShapeError(this->_shape, other._shape, "multiply");
        else
            return elementWiseOp(other, [] (auto a, auto b) { return a * b; } );
    }


    // Array division
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    auto operator/(const NArray<T>& other) const {
        if(!same_shape(*this, other))
            throw error::ShapeError(this->_shape, other._shape, "divide");
        else
            return elementWiseOp(other, [] (auto a, auto b) { return a / b; } );
    }


    /* Scalar Overloads */

    // Right exponent overload
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray operator^(T num) const {
        return fullVecOpR(num, [] (dtype b, T e) { return std::pow(b,e); });
    }


    // Right addition overload
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray operator+(T num) const {
        return fullVecOpR(num, [] (dtype a, T b) { return a + b; });
    }


    // Right subtraction overload
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray operator-(T num) const {
        return fullVecOpR(num, [] (dtype a, T b) { return a + b; });
    }


    // Right multiplication overload
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray operator*(T num) const {
        return fullVecOpR(num, [] (dtype a, T b) { return a * b; });
    }


    // Right division overload
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray operator/(T num) const {
        return fullVecOpR(num, [] (dtype a, T b) { return a / b; });
    }



    // Left base overload
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    friend NArray operator^(T num, const NArray& arr) {
        return arr.fullVecOpL(num, [] (T b, dtype e) { return std::pow(b,e); });
    }


    // Left addition overload
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    friend NArray operator+(T num, const NArray& arr) {
        return arr.fullVecOpL(num, [] (T a, dtype b) { return a + b; });
    }


    // Left subtraction overload
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    friend NArray operator-(T num, const NArray& arr) {
        return arr.fullVecOpL(num, [] (T a, dtype b) { return a - b; });
    }


    // Left multiplication overload
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    friend NArray operator*(T num, const NArray& arr) {
        return arr.fullVecOpL(num, [] (T a, dtype b) { return a * b; });
    }


    // Left division overload
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    friend NArray operator/(T num, const NArray& arr) {
        return arr.fullVecOpL(num, [] (T a, dtype b) { return a / b; });
    }


    /* Comparison Overloads */

    template <typename T>
    NArray<bool> operator==(const NArray<T>& other) const {
        return elementwiseCompare(other, [] (dtype a, T b) { return a == b; } );
    }

    template <typename T>
    NArray<bool> operator!=(const NArray<T>& other) const {
        return elementwiseCompare(other, [] (dtype a, T b) { return a != b; } );
    }

    template <typename T>
    NArray<bool> operator<=(const NArray<T>& other) const {
        return elementwiseCompare(other, [] (dtype a, T b) { return a <= b; });
    }

    template <typename T>
    NArray<bool> operator>=(const NArray<T>& other) const {
        return elementwiseCompare(other, [] (dtype a, T b) { return a >= b; });
    }

    template <typename T>
    NArray<bool> operator<(const NArray<T>& other) const {
        return elementwiseCompare(other, [] (dtype a, T b) { return a < b; });
    }

    template <typename T>
    NArray<bool> operator>(const NArray<T>& other) const {
        return elementwiseCompare(other, [] (dtype a, T b) { return a > b; });
    }



    /* Index Overload */
    NArray<dtype> operator[](const int& i) const {
        auto index = get_index(i);

        if (_shape.get_Ndim() == 1) {
            return NArray<dtype>(_data_ptr, _data_ptr.get() + index, Shape(1));
        } else {
            auto slice_start = _data_ptr.get() + _shape[1] * index;
            auto new_shape = Shape(_shape.dimensions.begin() + 1, _shape.dimensions.end());
            return NArray<dtype>(_data_ptr, slice_start, std::move(new_shape));
        }
    }


    /* Assignment Overload */
    void operator=(const NArray& other) {
        if(same_shape(*this, other)) {
            std::copy(
                other.get_data(),
                other.get_data() + _shape.get_total_size(),
                this->get_data()
            );
        } else {
            throw error::ValueError("Could not overwrite data because LHS and RHS of the assignment are not equal.");
        }
    }
        
    void operator=(NArray&& other) {
        if(same_shape(*this, other)) {
            std::move(
                other.get_data(),
                other.get_data() + _shape.get_total_size(),
                this->get_data()
            );
        } else {
            throw error::ValueError("Could not overwrite data because LHS and RHS of the assignment are not equal.");
        }
    }

    void operator=(std::initializer_list<dtype> list) {
        if(this->_shape.get_Ndim() == 1 && this->_shape[0] == list.size()) {
            int i = 0;
            for(dtype val : list)
                this->get_data()[i++] = val;
        } else {
            throw error::ValueError("Could not overwrite data because LHS and RHS of the assignment are not equal.");
        }
    }

    void operator=(const std::vector<dtype>& other) {
        if(this->_shape.get_Ndim() == 1 && this->_shape[0] == other.size()) {
            std::copy(
                other.begin(),
                other.end(),
                this->get_data()
            );
        } else {
            throw error::ValueError("Could not overwrite data because LHS and RHS of the assignment are not equal.");
        }
    }

    void operator=(std::vector<dtype>&& other) {
        if(this->_shape.get_Ndim() == 1 && this->_shape[0] == other.size()) {
            std::move(
                other.begin(),
                other.end(),
                this->get_data()
            );
        } else {
            throw error::ValueError("Could not overwrite data because LHS and RHS of the assignment are not equal.");
        }
    }

    void operator=(dtype val) {
        if(this->_shape == Shape{1}) {
            this->get_data()[0] = val;
        }
    }


    /* Print Overload */
    friend std::ostream& operator<<(std::ostream& os, const NArray& arr) {
        // Fetch print attributes
        util::PrintAttributes attributes;
        if(std::is_arithmetic_v<dtype>)
            attributes = util::GetPrintAttributes(arr.get_data(), arr.get_total_size());

        switch(arr._shape.get_Ndim()) {
        case 1: // 1D vector
            if(arr._shape[0] == 1) os << *(arr.get_data());
            else OneDPrint(os, arr, attributes);
            break;

        case 2: { // Martix
            os << '[';
            auto grps = util::split(arr.get_data_as_vector(), arr._shape[0]);
            for(size_t i = 0; i < grps.size(); i++) {
                OneDPrint(os, NArray(grps[i]), attributes);
                if(i != grps.size() - 1) os << "\n ";
            }
            os << ']';
            break;
        }

        default: // Anything else
            recursivePrint(os, arr.get_data_as_vector(), arr._shape, attributes);
            break;
        }
        return os;
    }



    /* ====== Conversion Operators ====== */

    explicit operator int() const {
        if (_shape.get_total_size() == 1)
            return static_cast<int>(get_data()[0]);
        else
            throw error::ConversionError(_shape, "int");
    }

    explicit operator long long() const {
        if (_shape.get_total_size() == 1)
            return static_cast<long long>(get_data()[0]);
        else
            throw error::ConversionError(_shape, "long long");
    }

    explicit operator double() const {
        if (_shape.get_total_size() == 1)
            return static_cast<double>(get_data()[0]);
        else
            throw error::ConversionError(_shape, "double");
    }

    explicit operator bool() const {
        return (_shape.get_total_size() != 0);
    }



    /* ===== NArray functions ====== */

    // Returns a NEW transposed matrix
    NArray transpose() const {
        auto out_shape = _shape.transpose();
        std::shared_ptr<dtype> out_data_ptr(new dtype[_shape.get_total_size()], std::default_delete<dtype[]>());
        util::transpose(out_data_ptr.get(), _data_ptr.get(), _shape);
        return NArray(out_data_ptr, out_shape);
    }

    // Returns a NEW transposed matrix (wrapper for NArray::transpose())
    NArray T() const { return transpose(); }


    // Returns a new flat vector
    NArray flatten() const { return NArray(get_data_copy_as_shared_ptr(), _shape.flatten()); }


    // Returns a flat view of the array
    NArray ravel() { return NArray(_data_ptr, _shape.flatten()); }


    // Returns a deep copy of the NArray
    NArray deepcopy() { return NArray(*this); }


    // Returns a shallow copy of the NArray
    NArray copy() { return NArray(_data_ptr, _shape); }

};


/* ====== Deduction guides ======= */

// Scalar constructor
template < typename T>
NArray(T) -> NArray<T>;

// Vector constructor
template <typename T>
NArray(std::vector<T>) -> NArray<T>;

// List constructor
template <typename T>
NArray(std::initializer_list<T>) -> NArray<T>;

// Array constructor
template <typename T>
NArray(T*, size_t) -> NArray<T>;

// Array constructor (copy)
template <typename T>
NArray(copy_t, T*, size_t) -> NArray<T>;

// Repeat constructor
template <typename T>
NArray(size_t, T) -> NArray<T>;

// Data + Shape constructor
template <typename T>
NArray(std::vector<T>, Shape) -> NArray<T>;

// Shape + initializer value constructor
template < typename T>
NArray(Shape, T) -> NArray<T>;

// Constructor from shared_ptr (used for slicing)
template < typename T>
NArray(std::shared_ptr<T>, T*, Shape) -> NArray<T>;

// Shared pointer + shape constructor
template <typename T>
NArray(std::shared_ptr<T>, Shape) -> NArray<T>;

} // namespace numcpp
