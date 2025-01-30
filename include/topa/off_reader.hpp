#ifndef TOPA_OFF_READER_HPP
#define TOPA_OFF_READER_HPP

#include <fstream>
#include <optional>
#include <string>
#include <vector>

namespace topa {
    template<typename Vertex, typename value_t, size_t dimensions>
    std::optional<std::vector<Vertex>> read_pointcloud(std::string off_file_path) {
        std::ifstream file(off_file_path);
        std::string format;
        file >> format;
        if (format != "OFF") [[unlikely]] {
            return std::nullopt;
        }

        size_t vertices_number;
        size_t simplex_number;
        size_t edges_number;
        file >> vertices_number >> simplex_number >> edges_number;
        if (simplex_number > 0 || edges_number > 0) [[unlikely]] {
            return std::nullopt;
        }

        std::vector<Vertex> vertices;
        for (size_t i = 0; i < vertices_number; ++i) {
            std::array<value_t, dimensions> coordinates;
            for (size_t j = 0; j < dimensions; ++j) {
                file >> coordinates[j];
            }
            vertices.emplace_back(i, std::move(coordinates));
        }

        return vertices;
    }
};   // namespace topa

#endif
