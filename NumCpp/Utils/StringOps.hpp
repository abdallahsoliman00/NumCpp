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

template <typename T>
std::string fillSpace(const T& obj, const unsigned int& whitespaces) {
    std::string out = std::to_string(obj);
    if(out.length() == whitespaces)
        return out;
    else
        return out + std::string(whitespaces - out.length(), ' ');
}

} // namespace numcpp::util
