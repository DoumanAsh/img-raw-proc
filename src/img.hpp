#pragma once

#include <filesystem>
#include <system_error>

#include <libraw/libraw.h>

namespace img {

class Proc {
    LibRaw proc;

    public:
        Proc() = default;

        void process_file(const std::filesystem::path&, std::error_code&) noexcept(true);
};

} //img
