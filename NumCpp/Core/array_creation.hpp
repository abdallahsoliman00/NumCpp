/* array_creation.hpp */
#pragma once

#include "NArray.hpp"
#include <cmath>


namespace numcpp {
    // TODO: Open https://numpy.org/doc/2.3/reference/routines.array-creation.html and implement functions.
    NArray<double> zeros(const size_t& size) {
        return NArray<double>(size);
    }

    NArray<double> ones(const size_t& size) {
        return NArray<double>(size, 1.0f);
    }

    NArray<double> linspace(const float& start, const float& stop, const size_t& count, const bool& endpoint = true) {
        float step = (stop - start)/(count - static_cast<float>(endpoint));
        std::vector<double> out(count, start);

        for(size_t i = 1; i < count; i++) {
            out[i] += step * i;
        }
        return NArray(out);
    }

    NArray<double> arange(const float& start, const float& stop, const float& step = 1) {
        size_t count = static_cast<size_t>(1 + (stop - start)/step);
        std::vector<double> out(count, start);

        for(size_t i = 1; i < count; i++) {
            out[i] += step * i;
        }
        return NArray(out);
    }
}
