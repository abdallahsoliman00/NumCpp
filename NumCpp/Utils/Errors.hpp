/* Errors.hpp */
#pragma once

#include <iostream>
#include <stdexcept>

#include "StringOps.hpp"


namespace numcpp {

struct Shape;

namespace error {

class ShapeError final : public std::runtime_error {
public:
    ShapeError(const Shape& lshape, const Shape& rshape, const std::string& operation)
        : std::runtime_error("[ShapeError]: Unable to " + operation + " Vectors. Cannot "
                            + operation +" shapes " + util::toString(lshape) + " and "
                            + util::toString(rshape) + ".")
    {
        std::cerr << runtime_error::what() << std::endl;
    }
    explicit ShapeError(std::string&& message)
        : std::runtime_error("[ShapeError]: " + std::move(message))
    {
        std::cerr << runtime_error::what() << std::endl;
    }
};

class ValueError final : public std::runtime_error {
public:
    explicit ValueError(std::string&& message)
        : std::runtime_error("[ValueError]: " + std::move(message))
    {
        std::cerr << runtime_error::what() << std::endl;
    }
};


class ConversionError final : public std::domain_error {
public:
    ConversionError(const Shape& shape, std::string&& type)
        : std::domain_error(
            "[ConversionError]: Unable to convert array of shape " +
            util::toString(shape) + " to " + std::move(type) + "."
        )
    {
        std::cerr << logic_error::what() << std::endl;
    }
};

} // namespace error
} // namespace numcpp