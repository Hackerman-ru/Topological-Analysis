#ifndef __OFF_PARSER_H__
#define __OFF_PARSER_H__

#include "core/complex.h"
#include "core/simplex.h"
#include "vertices/point.h"

#include <fstream>
#include <optional>
#include <unordered_set>

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
    vertices.reserve(vertices_number);
    std::unordered_set<Simplex<Vertex>> simplices;
    for (size_t i = 0; i < vertices_number; ++i) {
        typename Vertex::Coordinates coordinates;
        for (size_t j = 0; j < dimensions; ++j) {
            data_t coordinate;
            file >> coordinate;
            coordinates[j] = std::move(coordinate);
        }
        vertices.emplace_back(Vertex(i, std::move(coordinates)));
        simplices.insert({vertices.back()});
    }

    std::sort(vertices.begin(), vertices.end());

    if (pointcloud) {
        if (simplex_number != 0 || edges_number != 0) {
            return std::nullopt;
        }

        for (size_t i = 0; i < vertices_number; ++i) {
            for (size_t j = i + 1; j < vertices_number; ++j) {
                for (size_t k = j + 1; k < vertices_number; ++k) {
                    simplices.insert({vertices[i], vertices[j]});
                    simplices.insert({vertices[j], vertices[k]});
                    simplices.insert({vertices[i], vertices[k]});
                    simplices.insert({vertices[i], vertices[j], vertices[k]});
                }
            }
        }
    } else {
        for (size_t i = 0; i < simplex_number; ++i) {
            size_t simplex_size;
            file >> simplex_size;
            std::vector<Vertex> simplex_vertices;
            simplex_vertices.reserve(simplex_size);
            for (size_t j = 0; j < simplex_size; ++j) {
                size_t vertex_id;
                file >> vertex_id;
                simplex_vertices.push_back(vertices[vertex_id]);
            }
            if (simplex_size > 1) {
                for (size_t j = 0; j < simplex_size; ++j) {
                    if (j == simplex_size - 1) {
                        simplices.insert({simplex_vertices[0], simplex_vertices[j]});
                    } else {
                        simplices.insert({simplex_vertices[j], simplex_vertices[j + 1]});
                    }
                }
            }
            simplices.insert(std::move(simplex_vertices));
        }
    }

    std::vector<Simplex<Vertex>> complex_simplices(simplices.begin(), simplices.end());
    return Complex<Vertex>(std::move(complex_simplices));
}

#endif
