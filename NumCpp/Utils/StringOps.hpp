/* StringOps.hpp */
#pragma once

#include <sstream>

namespace numcpp::util {

// Converts anything to a string, given this thing has an << overload.
template <typename T>
std::string toString(const T& obj) {
    std::ostringstream oss;
    oss << obj;
    return oss.str();
}

}
