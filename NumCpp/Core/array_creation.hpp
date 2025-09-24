/* array_creation.hpp */
#pragma once

#include "NArray.hpp"
#include <cmath>

// TODO: Check for types. Cannot create a zeros array of char
namespace numcpp {
    // TODO: Open https://numpy.org/doc/2.3/reference/routines.array-creation.html and implement functions.
    template <typename T = double>
    NArray<T> zeros(const uint32_t& size) {
        return NArray<T>(size);
    }
    template <typename T = double>
    NArray<T> zeros(const Shape& shape) {
        return NArray<T>(shape, 0.0f);
    }

    template <typename T = double>
    NArray<T> zeros_like(const NArray<T>& other) {
        return NArray<T>(other.get_shape(), 0.0f);
    }
    template <typename T = double>
    NArray<T> zeros_like(const std::vector<T>& other) {
        return NArray<T>(other.size(), 0.0f);
    }

    template <typename T = double>
    NArray<T> ones(const uint32_t& size) {
        return NArray<T>(size, 1.0f);
    }
    template <typename T = double>
    NArray<T> ones(const Shape& shape) {
        return NArray<T>(shape, 1.0f);
    }

    template <typename T = double>
    NArray<T> ones_like(const NArray<T>& other) {
        return NArray<T>(other.get_shape(), 1.0f);
    }

    NArray<double> linspace(const float& start, const float& stop, const uint32_t& count, const bool& endpoint = true) {
        float step = (stop - start)/(count - static_cast<float>(endpoint));
        std::vector<double> out(count, start);

        for(uint32_t i = 1; i < count; i++) {
            out[i] += step * i;
        }
        return NArray(out);
    }

    NArray<double> arange(const float& start, const float& stop, const float& step = 1) {
        uint32_t count = static_cast<uint32_t>(1 + (stop - start)/step);
        std::vector<double> out(count, start);

        for(uint32_t i = 1; i < count; i++) {
            out[i] += step * i;
        }
        return NArray(out);
    }

    template <typename T = double>
    NArray<T> eye(uint32_t n, uint32_t m = 0) {
        if (m == 0) m = n;

        std::vector<T> out_data(n * m, static_cast<T>(0));

        for (uint32_t i = 0; i < n && i < m; ++i) {
            out_data[i * m + i] = static_cast<T>(1); // row-major indexing
        }

        return NArray<T>(std::move(out_data), Shape({n, m}));
    }

    template <typename T = double>
    NArray<T> identity(uint32_t n) {
        return eye<T>(n);
    }
} // namespace numcpp
