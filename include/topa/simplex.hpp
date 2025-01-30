#ifndef TOPA_SIMPLEX_HPP
#define TOPA_SIMPLEX_HPP

#include <set>
#include <string>
#include <vector>

namespace topa {
    template<typename Vertex>
    class Simplex {
        using Vertices = std::set<Vertex>;

    public:
        Simplex(Vertices vertices) : m_vertices(std::move(vertices)) {}

        template<typename InputIterator>
        Simplex(InputIterator begin, InputIterator end) : m_vertices(begin, end) {};

        friend auto operator<=>(const Simplex& lhs, const Simplex& rhs) = default;

        std::string to_string() const {
            std::string result = "{";
            for (const auto& vertex : m_vertices) {
                result += vertex.to_string() + ",";
            }
            if (size() > 0) [[likely]] {
                result.pop_back();
            }
            result += '}';
            return result;
        }

        size_t size() const {
            return m_vertices.size();
        }

        std::vector<Simplex> subsimplices() const {
            std::vector<Simplex> simplex_subsimplices;
            simplex_subsimplices.reserve(m_vertices.size());
            for (size_t exclude_pos = m_vertices.size(); exclude_pos > 0; --exclude_pos) {
                Vertices subsimplex_vertices;
                size_t i = 0;
                for (auto it = m_vertices.begin(); it != m_vertices.end(); ++it, ++i) {
                    if (i != exclude_pos) {
                        subsimplex_vertices.insert(*it);
                    }
                }
                if (!subsimplex_vertices.empty()) {
                    simplex_subsimplices.push_back(std::move(subsimplex_vertices));
                }
            }
            return simplex_subsimplices;
        }

        Vertices vertices() const {
            return m_vertices;
        }

    private:
        Vertices m_vertices;
    };
}   // namespace topa

#endif
