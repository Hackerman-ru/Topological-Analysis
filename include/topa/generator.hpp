#ifndef TOPA_GENERATOR_HPP_
#define TOPA_GENERATOR_HPP_

#include <cassert>
#include <coroutine>
#include <exception>
#include <iterator>
#include <optional>

namespace topa::detail {

template <std::movable T>
class Generator {
   public:
    struct promise_type {                   // NOLINT
        Generator<T> get_return_object() {  // NOLINT
            return Generator{Handle::from_promise(*this)};
        }

        static std::suspend_always initial_suspend() noexcept {  // NOLINT
            return {};
        }

        static std::suspend_always final_suspend() noexcept {  // NOLINT
            return {};
        }

        std::suspend_always yield_value(T value) noexcept {  // NOLINT
            current_value.emplace(std::move(value));
            return {};
        }

        void await_transform() = delete;

        [[noreturn]]
        static void unhandled_exception() {  // NOLINT
            throw;
        }

        std::optional<T> current_value;
    };

    using Handle = std::coroutine_handle<promise_type>;

    explicit Generator(const Handle coroutine)
        : coroutine_{coroutine} {
    }

    Generator() = default;

    ~Generator() {
        if (coroutine_) {
            coroutine_.destroy();
        }
    }

    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;

    Generator(Generator&& other) noexcept
        : coroutine_{other.coroutine_} {
        other.coroutine_ = {};
    }

    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (coroutine_) {
                coroutine_.destroy();
            }
            coroutine_ = other.coroutine_;
            other.coroutine_ = {};
        }
        return *this;
    }

    class Iter {
        friend class Generator;

       public:
        // NOLINTBEGIN
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        // NOLINTEND

       public:
        void operator++() {
            if (coroutine_ && !coroutine_.done()) {
                coroutine_.resume();
            }
        }

        T& operator*() {
            assert(coroutine_ && !coroutine_.done() &&
                   "Dereferencing invalid or ended generator");
            return *coroutine_.promise().current_value;
        }

        const T& operator*() const {
            assert(coroutine_ && !coroutine_.done() &&
                   "Dereferencing invalid or ended generator");
            return *coroutine_.promise().current_value;
        }

        bool operator==(const Iter& other) const {
            return Invalid() == other.Invalid();
        }

       private:
        explicit Iter(const Handle coroutine)
            : coroutine_{coroutine} {
        }

        Iter() = default;

        bool Invalid() const {
            return !coroutine_ || coroutine_.done();
        }

       private:
        Handle coroutine_;
    };

    Iter begin() {  // NOLINT
        if (coroutine_ && !coroutine_.done()) {
            coroutine_.resume();
        }
        return Iter{coroutine_};
    }

    Iter end() {  // NOLINT
        return Iter{};
    }

   private:
    Handle coroutine_;
};

}  // namespace topa::detail

#endif
