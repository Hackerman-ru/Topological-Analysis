#ifndef __OFF_PARSER_H__
#define __OFF_PARSER_H__

#include "core/complex.h"
#include "vertices/point.h"

#include <fstream>
#include <optional>

template<typename data_t, size_t dimensions>
std::optional<Complex<Point<data_t, dimensions>>> parse_off_file(std::string path, bool pointcloud = false) {
    using Vertex = Point<data_t, dimensions>;

    std::ifstream file(path);
    std::string format;

    file >> format;
    if (format != "OFF") {
        return std::nullopt;
    }

    size_t vertices_number;
    size_t simplex_number;
    size_t edges_number;
    file >> vertices_number >> simplex_number >> edges_number;

    std::vector<Vertex> vertices;
    for (size_t i = 0; i < vertices_number; ++i) {
        typename Vertex::Coordinates coordinates;
        for (size_t j = 0; j < dimensions; ++j) {
            data_t coordinate;
            file >> coordinate;
            coordinates[j] = std::move(coordinate);
        }
        vertices.emplace_back(Vertex(i, std::move(coordinates)));
    }

    auto complex = Complex<Vertex>::create();
    if (pointcloud) {
        if (simplex_number != 0 || edges_number != 0) {
            return std::nullopt;
        }

        for (size_t i = 0; i < vertices_number; ++i) {
            for (size_t j = i + 1; j < vertices_number; ++j) {
                for (size_t k = j + 1; k < vertices_number; ++k) {
                    std::vector<Vertex> simplex_vertices({vertices[i], vertices[j], vertices[k]});
                    complex.push(Simplex<Vertex>(std::move(simplex_vertices)));
                }
            }
        }
    } else {
        for (size_t i = 0; i < simplex_number; ++i) {
            size_t simplex_size;
            file >> simplex_size;
            std::vector<Vertex> simplex_vertices;
            for (size_t j = 0; j < simplex_size; ++j) {
                size_t vertex_id;
                file >> vertex_id;
                simplex_vertices.push_back(vertices[vertex_id]);
            }
            complex.push(Simplex<Vertex>(std::move(simplex_vertices)));
        }
    }

    return complex;
}

#endif
