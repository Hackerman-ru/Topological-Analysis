#include "fast/twist.hpp"
#include "fast/double_twist.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include <unordered_map>
#include <set>

using namespace topa;

// Mock-реализация FilteredComplex
struct MockComplex : models::FilteredComplex<MockComplex> {
    // Структура комплекса: 3 точки (0-2), 3 ребра (3-5), 1 треугольник (6)
    // Фильтрация: сначала точки, потом ребра, потом треугольник
    std::size_t Size() const {
        return 7;
    }

    const Positions& GetPosesBySize(std::size_t size) const {
        static const std::vector<Position> points = {0, 1, 2},
                                           edges = {3, 4, 5}, triangle = {6},
                                           empty;

        switch (size) {
            case 1:
                return points;
            case 2:
                return edges;
            case 3:
                return triangle;
            default:
                return empty;
        }
    }

    Positions GetFacetsPosition(Position pos) const {
        // Грани для симплексов
        if (pos >= 3 && pos <= 5) {  // Ребра
            switch (pos) {
                case 3:
                    return std::vector<Position>{0, 1};  // Ребро 0-1
                case 4:
                    return std::vector<Position>{1, 2};  // Ребро 1-2
                case 5:
                    return std::vector<Position>{0, 2};  // Ребро 0-2
            }
        }
        if (pos == 6) {                             // Треугольник
            return std::vector<Position>{3, 4, 5};  // Грани - все три ребра
        }
        return std::vector<Position>{};
    }

    Positions GetCofacetsPosition(Position pos) const {
        // Кограни (симплексы, в которые входит данный)
        if (pos <= 2) {  // Точки
            std::vector<Position> cofacets;
            for (auto edge : {3, 4, 5}) {
                auto facets = GetFacetsPosition(edge);
                if (std::find(facets.begin(), facets.end(), pos) !=
                    facets.end()) {
                    cofacets.push_back(edge);
                }
            }
            return cofacets;
        }
        if (pos >= 3 && pos <= 5) {           // Ребра
            return std::vector<Position>{6};  // Все ребра входят в треугольник
        }
        return std::vector<Position>{};
    }
};

// Полная реализация Matrix с поддержкой граничных операций
struct MockMatrix : models::Matrix<MockMatrix> {
    using Row = std::vector<Position>;
    std::unordered_map<std::size_t, Row> data;

    MockMatrix() = default;

    explicit MockMatrix(std::size_t size) {
        data.reserve(size);
    }

    bool Contains(std::size_t col) const {
        return data.contains(col);
    }
    std::size_t Size() const {
        return data.size();
    }

    const Row& At(std::size_t col) const {
        return data.at(col);
    }

    const Row& operator[](std::size_t col) const {
        return data.at(col);
    }
    Row& operator[](std::size_t col) {
        return data[col];
    }

    template <std::ranges::range R>
    void Insert(std::size_t col, R&& row) {
        data.try_emplace(col, row.begin(), row.end());
    }

    void Insert(std::size_t col, std::initializer_list<Position> row) {
        Insert(col, std::views::all(row));
    }

    void Erase(std::size_t col) {
        data.erase(col);
    }
    void Reserve(std::size_t n_cols) {
        data.reserve(n_cols);
    }
};

// Реализация PosHeap с сохранением порядка
struct MockHeap : models::PosHeap<MockHeap> {
    std::set<Position> elements;

    MockHeap() = default;

    explicit MockHeap(std::size_t) {
        //
    }

    Position GetMaxPos() const {
        if (IsEmpty()) {
            return kUnknownPos;
        }
        return *elements.rbegin();
    }

    bool IsEmpty() const {
        return elements.empty();
    }

    template <std::ranges::range R>
    void Add(R&& range) {
        for (const auto& pos : range) {
            auto it = elements.find(pos);
            if (it != elements.end()) {
                elements.erase(it);
            } else {
                elements.insert(pos);
            }
        }
    }

    void Add(std::initializer_list<Position> positions) {
        Add(std::views::all(positions));
    }

    Position PopMaxPos() {
        if (IsEmpty()) {
            return kUnknownPos;
        }
        auto it = elements.end();
        --it;
        Position max_pos = *it;
        elements.erase(it);
        return max_pos;
    }

    std::vector<Position> PopAll() {
        Positions res(elements.begin(), elements.end());
        elements.clear();
        return res;
    }

    void Clear() {
        elements.clear();
    }
};

TEST_CASE("BoundaryMatrix constructs matrix correctly") {
    using namespace topa::common;
    using namespace topa::models;

    MockComplex complex;

    auto matrix = BoundaryMatrix<MockMatrix, MockComplex>(complex);
    REQUIRE(matrix.Size() == 4);
    REQUIRE(matrix.Contains(3));
    REQUIRE(matrix.Contains(4));
    REQUIRE(matrix.Contains(5));
    REQUIRE(matrix.Contains(6));

    CHECK_THAT(matrix[3],
               Catch::Matchers::UnorderedEquals(std::vector<Position>{0, 1}));
    CHECK_THAT(matrix[4],
               Catch::Matchers::UnorderedEquals(std::vector<Position>{1, 2}));
    CHECK_THAT(matrix[5],
               Catch::Matchers::UnorderedEquals(std::vector<Position>{0, 2}));
    CHECK_THAT(matrix[6], Catch::Matchers::UnorderedEquals(
                              std::vector<Position>{3, 4, 5}));
}

TEST_CASE("Twist computes correct persistence pairs") {
    MockComplex complex;

    SECTION("For triangle complex") {
        auto pairs = fast::Twist<MockMatrix, MockHeap>::Compute(complex);

        REQUIRE(pairs.size() == 3);
        // Ожидаемые пары:
        // - 3 компоненты (0,1,2) умирают при добавлении ребер
        // - 1 цикл умирает при добавлении треугольника
        CHECK(pairs[0] == (PersistencePair{1, 3}));
        CHECK(pairs[1] == (PersistencePair{2, 4}));
        CHECK(pairs[2] == (PersistencePair{5, 6}));
    }
}

TEST_CASE("DoubleTwist handles dual computation") {
    MockComplex complex;

    SECTION("For same triangle complex") {
        auto pairs = fast::DoubleTwist<MockMatrix, MockHeap>::Compute(complex);

        REQUIRE(pairs.size() == 3);
        CHECK(pairs[0] == (PersistencePair{1, 3}));
        CHECK(pairs[1] == (PersistencePair{2, 4}));
        CHECK(pairs[2] == (PersistencePair{5, 6}));
    }

    SECTION("Equal answers with twist") {
        auto twist_result = fast::Twist<MockMatrix, MockHeap>::Compute(complex);
        auto double_twist_result =
            fast::DoubleTwist<MockMatrix, MockHeap>::Compute(complex);
        std::sort(twist_result.begin(), twist_result.end());
        std::sort(double_twist_result.begin(), double_twist_result.end());
        REQUIRE(twist_result == double_twist_result);
    }
}

TEST_CASE("Matrix operations work correctly") {
    MockMatrix matrix(5);

    SECTION("Insert and retrieve column") {
        matrix.Insert(2, {5, 3, 1});
        REQUIRE(matrix.Contains(2));
        REQUIRE(matrix[2] == std::vector<Position>{5, 3, 1});
    }

    SECTION("Erase column") {
        matrix.Insert(1, {2, 4});
        matrix.Erase(1);
        REQUIRE_FALSE(matrix.Contains(1));
    }
}

TEST_CASE("PosHeap maintains order") {
    MockHeap heap;

    SECTION("Add and pop max") {
        heap.Add({3, 1, 5});
        REQUIRE(heap.GetMaxPos() == 5);
        REQUIRE(heap.PopMaxPos() == 5);
        REQUIRE(heap.PopMaxPos() == 3);
    }

    SECTION("PopAll returns ascending order") {
        heap.Add({2, 4, 1});
        auto all = heap.PopAll();
        REQUIRE(all == std::vector<Position>{1, 2, 4});
    }
}

#include "common/pointcloud.hpp"
#include "fast/full_vr.hpp"
#include "fast/full_tree_opt.hpp"
#include "common/filtered_complex.hpp"
#include "fast/bit_tree_heap.hpp"
#include "fast/sparse_matrix.hpp"

#include <algorithm>

using namespace topa::common;
using namespace topa::fast;

TEST_CASE("Real data") {
    Pointcloud cloud =
        Pointcloud::Load(DATA_DIR "/pointclouds/100.off").value();
    auto complex = FilteredComplex<FullTreeOpt>::From(cloud, FullVR());
    auto twist_result = Twist<SparseMatrix, BitTreeHeap>::Compute(complex);
    auto double_twist_result =
        DoubleTwist<SparseMatrix, BitTreeHeap>::Compute(complex);
    std::sort(twist_result.begin(), twist_result.end());
    std::sort(double_twist_result.begin(), double_twist_result.end());
    REQUIRE(twist_result == double_twist_result);
}
