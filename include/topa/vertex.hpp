#ifndef TOPA_BASE_VERTEX_HPP
#define TOPA_BASE_VERTEX_HPP

#include <compare>
#include <memory>
#include <string>

namespace topa {
    template<typename id_t = size_t>
    class BaseVertex {
    public:
        BaseVertex(id_t id) : m_id(id) {}

        friend std::partial_ordering operator<=>(const BaseVertex& lhs, const BaseVertex& rhs) = default;

        virtual std::string to_string() const {
            return std::to_string(m_id);
        }

        virtual ~BaseVertex() = default;

    protected:
        id_t m_id;
    };

    template<typename value_t, size_t dimensions, typename id_t = size_t>
    class Point : public BaseVertex<id_t> {
        using Base = BaseVertex<id_t>;
        using Coordinates = std::array<value_t, dimensions>;

    public:
        template<typename Container>
        Point(id_t id, Container&& coordinates) :
            Base(id), m_coordinates(std::forward<Container>(coordinates)) {};

        friend auto operator<=>(const Point& lhs, const Point& rhs) = default;

        const value_t& operator[](size_t i) const {
            return m_coordinates[i];
        }

        const value_t& at(size_t i) const {
            return m_coordinates[i];
        }

        const Coordinates& get_coordinates() const {
            return m_coordinates;
        }

    private:
        Coordinates m_coordinates;
    };

    template<typename Vertex>
    class LightVertex {
    public:
        template<typename... Args>
        LightVertex(Args... args) : m_vertex_pointer(std::make_shared<Vertex>(std::forward<Args>(args)...)) {}

        friend bool operator==(const LightVertex& lhs, const LightVertex& rhs) {
            return *lhs.m_vertex_pointer == *rhs.m_vertex_pointer;
        }

        friend std::partial_ordering operator<=>(const LightVertex& lhs, const LightVertex& rhs) {
            return *lhs.m_vertex_pointer <=> *rhs.m_vertex_pointer;
        }

        const Vertex& operator*() const {
            return *m_vertex_pointer;
        }

    private:
        std::shared_ptr<const Vertex> m_vertex_pointer;
    };
};   // namespace topa

#endif
