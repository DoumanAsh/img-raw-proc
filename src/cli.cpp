#include "cli.hpp"

#include <span>
#include <tuple>
#include <ranges>
#include <string_view>
#include <type_traits>

#include <fmt/core.h>

namespace cli {

namespace detail {
    template <typename Arg>
    inline constexpr auto argv_iter(int argc, char** argv) {
        static_assert(std::is_constructible_v<Arg, char*>, "Should be able to construct Arg form char*");
        return std::views::drop(
            std::views::transform(
                std::span<char*>(argv, argc),
                [](auto arg) -> Arg { return arg; }
            ),
            1
        );
    }
}

int Args::parse(int argc, char** argv) {
    if (argc < 2) {
        this->help = true;
        return 0;
    }

    const auto args = detail::argv_iter<std::string_view>(argc, argv);

    size_t idx = 1;
    const auto args_end = std::ranges::end(args);
    for (auto it = std::ranges::begin(args); it != args_end; ++it) {
        auto arg = *it;
        if (arg[0] == '-') {
            arg.remove_prefix(1);
            if (arg.empty()) {
                fmt::println("Incomplete short option at position {}", idx);
                return 1;
            }

            if (arg[0] == '-') {
                arg.remove_prefix(1);
                if (arg.empty()) {
                    fmt::println("Incomplete long option at position {}", idx);
                    return 1;
                }

                if (arg == "help") {
                    this->help = true;
                    return 0;
                }

                if (arg == "yes") {
                    this->yes = true;
                } else if (arg == "ext") {
                    ++it;
                    if (it == args_end) {
                        fmt::println("'ext' is missing argument");
                        return 2;
                    }
                    arg = *it;
                    this->extensions.insert(arg);
                } else {
                    fmt::println("Invalid long option '{}' at position {}", arg.data(), idx);
                    return 2;
                }
            } else {
                if (arg == "h") {
                    this->help = true;
                    return 0;
                }

                if (arg == "y") {
                    this->yes = true;
                } else {
                    fmt::println("Invalid short option '{}' at position {}", arg.data(), idx);
                    return 2;
                }
            }
        } else {
            this->paths.push_back(arg.data());
        }

        idx += 1;
    }

    return 0;
}

} //cli
