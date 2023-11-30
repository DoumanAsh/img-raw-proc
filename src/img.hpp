#pragma once

#include <filesystem>

#include <libraw/libraw.h>

namespace img {

class Proc {
    LibRaw proc;

    public:
        Proc() = default;

        void process_file(const std::filesystem::path&) noexcept(true);
};

} //img
