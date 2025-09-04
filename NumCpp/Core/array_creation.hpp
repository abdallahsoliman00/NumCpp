#include "NArray.hpp"
#include <cmath>


namespace nc {
    // TODO: Open https://numpy.org/doc/2.3/reference/routines.array-creation.html and implement functions.
    NArray zeros(const size_t& size) {
        return NArray(size);
    }

    NArray ones(const size_t& size) {
        return NArray(size, 1.0f);
    }

    NArray linspace(const float& start, const float& stop, const size_t& count, const bool& endpoint = true) {
        float step = (stop - start)/(count - static_cast<float>(endpoint));
        std::vector<float> out(count, start);

        for(size_t i = 1; i < count; i++) {
            out[i] += step * i;
        }
        return NArray(out);
    }

    NArray arange(const float& start, const float& stop, const float& step = 1) {
        size_t count = static_cast<size_t>(1 + (stop - start)/step);
        std::vector<float> out(count, start);

        for(size_t i = 1; i < count; i++) {
            out[i] += step * i;
        }
        return NArray(out);
    }
}
