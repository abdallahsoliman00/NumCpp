/* array_creation.hpp */
#pragma once

#include "NArray.hpp"
#include <cmath>


namespace numcpp {
    // TODO: Open https://numpy.org/doc/2.3/reference/routines.array-creation.html and implement functions.
    NArray<double> zeros(const uint32_t& size) {
        return NArray<double>(size);
    }
    NArray<double> zeros(const Shape& shape) {
        return NArray<double>(shape, 0.0f);
    }

    NArray<double> ones(const uint32_t& size) {
        return NArray<double>(size, 1.0f);
    }
    NArray<double> ones(const Shape& shape) {
        return NArray<double>(shape, 1.0f);
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
} // namespace numcpp
