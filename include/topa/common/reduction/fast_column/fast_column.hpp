#pragma once

#include "common/type/position.hpp"

#include <concepts>
#include <ranges>

namespace topa::common::reduction {

template <typename T>
concept ColumnImplementation = requires(T t, const T& ct, size_t n) {
    { T(n) } -> std::same_as<T>;
    { ct.GetMaxPos() } -> std::same_as<Position>;
    { ct.IsEmpty() } -> std::same_as<bool>;
    { t.PopMaxPos() } -> std::same_as<Position>;
    t.Clear();
    t.Add(std::declval<std::ranges::input_range auto>());
};

template <ColumnImplementation Impl>
class FastColumn {
   public:
    explicit FastColumn(size_t number_of_columns)
        : impl_(number_of_columns) {
    }

    Position GetMaxPos() const {
        return impl_.GetMaxPos();
    }

    bool IsEmpty() const {
        return impl_.IsEmpty();
    }

    template <std::ranges::input_range R>
    void Add(R&& range) {
        impl_.Add(std::forward<R>(range));
    }

    template <typename It>
    void Add(It begin, It end) {
        impl_.Add(std::ranges::subrange(begin, end));
    }

    Position PopMaxPos() {
        return impl_.PopMaxPos();
    }

    void Clear() {
        impl_.Clear();
    }

   private:
    Impl impl_;
};

}  // namespace topa::common::reduction
