/* Errors.hpp */
#pragma once

#include <iostream>
#include <stdexcept>
#include <StringOps.hpp>
#include "../Core/Shape.hpp"

namespace numcpp::error {

class ShapeError : public std::runtime_error {
public:
    ShapeError(const Shape& lshape, const Shape& rshape, const std::string& operation)
        : std::runtime_error("[ShapeError]: Unable to " + operation + " Vectors. Cannot "
                            + operation +" shapes " + util::toString(lshape) + " and "
                            + util::toString(rshape) + ".")
    {
        std::cerr << what() << std::endl;
    }

};

}