#ifndef FUNCLIST_H
#define FUNCLIST_H

#include <functional>
#include <string>
#include <sstream>
#include <ranges>
#include <memory>

namespace flist {

    namespace detail {
        template <typename Func>
        struct Callable {
            Func func;

            template <typename F, typename A>
            auto operator()(F f, A a) const {
                return func(f, a);
            }
        };

        template <typename T>
        auto wrap(T&& func) {
            return Callable<std::decay_t<T>>{std::forward<T>(func)};
        }
    } 

    // Empty list
    inline auto empty = detail::wrap([](auto, auto a) { return a; });

    // Cons operation
    inline auto cons(auto x, auto l) {
        return detail::wrap([=](auto f, auto a) { return f(x, l(f, a)); });
    }

    // Create a list from variadic arguments
    inline auto create(auto... args) {
        auto result = empty;
        (..., (result = cons(args, result)));
        return result;
    }

    // Create a list from a range
    inline auto of_range(auto r) {
        auto result = empty;
        for (auto it = std::rbegin(r); it != std::rend(r); ++it) {
            result = cons(*it, result);
        }
        return result;
    }

    // Concatenate two lists
    inline auto concat(auto l, auto k) {
        return detail::wrap([=](auto f, auto a) { return l(f, k(f, a)); });
    }

    // Reverse a list
    inline auto rev(auto l) {
        return detail::wrap([=](auto f, auto a) {
            auto fold = [](auto l, auto acc) {
                return l([&](auto x, auto innerAcc) {
                    return cons(x, innerAcc);
                }, acc);
            };
            return fold(l, empty)(f, a);
        });
    }

    // Map operation
    inline auto map(auto m, auto l) {
        return detail::wrap([=](auto f, auto a) {
            return l([&](auto x, auto acc) {
                return f(m(x), acc);
            }, a);
        });
    }

    // Filter operation
    inline auto filter(auto p, auto l) {
        return detail::wrap([=](auto f, auto a) {
            return l([&](auto x, auto acc) {
                return p(x) ? f(x, acc) : acc;
            }, a);
        });
    }

    // Flatten operation
    inline auto flatten(auto l) {
        return detail::wrap([=](auto f, auto a) {
            return l([&](auto inner, auto acc) {
                return inner(f, acc);
            }, a);
        });
    }

    // Convert list to string
    inline std::string as_string(const auto& l) {
        std::ostringstream oss;
        oss << "[";
        bool first = true;
        l([&](auto x, auto) {
            if (!first) {
                oss << ", ";
            }
            first = false;
            oss << x;
            return 0; 
        }, 0); 
        oss << "]";
        return oss.str();
    }
} 

#endif // FUNCLIST_H
