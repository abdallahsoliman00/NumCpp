/* ReadFile.hpp */
#pragma once

#include <fstream>
#include <string>
#include <type_traits>

#include "Core/NArray.hpp"


namespace numcpp {


    template <typename T = double, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> read_from_file(const char* filepath, const char delimiter = ' ') {
        std::ifstream file(filepath);
        if (!file) {
            throw error::ArgumentError("Error opening file \"" + std::string(filepath) + "\"");
        }

        std::vector<T> out_vec;
        std::vector<size_t> shape{0, 0};

        std::string line;
        size_t& line_counter = shape[0];

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string token;
            size_t element_counter = 0;

            while (std::getline(ss, token, delimiter)) {
                if (!token.empty()) {
                    std::stringstream converter(token);
                    T elem;
                    converter >> elem;
                    out_vec.push_back(elem);
                    element_counter++;
                }
            }

            if (line_counter > 0 && element_counter != shape[1])
                throw error::ShapeError(
                    "The input data is inconsistent. Please check line " +
                    std::to_string(line_counter + 1) + " in \"" + std::string(filepath) + "\"."
                );
            else
                shape[1] = element_counter;

            line_counter++;
        }

        file.close();
        return NArray<T>(std::move(out_vec), Shape(shape));
    }


} // namespace numcpp
