#include "cli.hpp"
#include "img.hpp"

#include <fmt/core.h>
#include <libraw/libraw.h>

#include <filesystem>
#include <system_error>

namespace fs = std::filesystem;

static void process_path(img::Proc& img, const fs::path& path, std::error_code& error_code, const cli::Args& args) noexcept(true);

constexpr size_t SYMLINK_RECURSIVE_LIMIT = 10;
static void process_folder(img::Proc& img, const fs::path& path, std::error_code& error_code, const cli::Args& args) noexcept(true) {
    auto options = fs::directory_options::skip_permission_denied | fs::directory_options::follow_directory_symlink;
    for (const auto& entry: fs::recursive_directory_iterator(path, options, error_code)) {
        if (error_code.value() != 0) {
            fmt::println("{}: failed to process: {}", entry.path().string(), error_code.message());
            error_code.clear();
            continue;
        }

        if (entry.is_regular_file()) {
            const auto ext = entry.path().extension().string();
            if (ext.empty()) {
                continue;
            }

            //File extension always has dot so skip it
            if (args.extensions.contains(ext.c_str() + 1)) {
                img.process_file(entry.path());
            }
        } else {
            process_path(img, entry.path(), error_code, args);
        }

        if (error_code.value() != 0) {
            fmt::println("{}: failed to process: {}", entry.path().string(), error_code.message());
            error_code.clear();
        }
    }
}

static void process_path(img::Proc& img, const fs::path& path, std::error_code& error_code, const cli::Args& args) noexcept(true) {
    const auto meta = fs::status(path, error_code);

    if (error_code.value() != 0) {
        return;
    }

    switch (meta.type()) {
        case fs::file_type::directory:
            return process_folder(img, path, error_code, args);
        case fs::file_type::regular:
            img.process_file(path);
            return;
        case fs::file_type::symlink: {
            fs::path actual_path;
            fs::file_status actual_meta;
            size_t reads = 1;
            do {
                actual_path = fs::read_symlink(path, error_code);

                if (error_code.value() != 0) {
                    return;
                }

                actual_meta = fs::status(path, error_code);

                if (error_code.value() != 0) {
                    return;
                }

                switch (actual_meta.type()) {
                    case fs::file_type::regular:
                        img.process_file(actual_path);
                        return;
                    case fs::file_type::directory:
                        return process_folder(img, actual_path, error_code, args);
                    case fs::file_type::symlink:
                        continue;
                    default:
                        error_code = std::make_error_code(std::errc::no_such_file_or_directory);
                        break;
                }
                reads += 1;
            } while (reads < SYMLINK_RECURSIVE_LIMIT);
            break;
        }
        default:
            error_code = std::make_error_code(std::errc::no_such_file_or_directory);
            return;
    }
}

int main(int argc, char** argv) {
    cli::Args args;
    auto result = args.parse(argc, argv);
    if (result != 0) {
        return result;
    }

    fmt::println("Process {} inputs", args.paths.size());
    if (args.help) {
        fmt::println("{}", cli::HELP);
        return 0;
    }

    img::Proc img;
    std::error_code error_code;

    for (const auto *const arg : args.paths) {
        const fs::path path(arg);
        process_path(img, path, error_code, args);

        if (error_code.value() != 0) {
            fmt::println("{}: No such file or directory", path.string());
            error_code.clear();
        }
    }

    return 0;
}
