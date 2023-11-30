#pragma once

#include <vector>
#include <unordered_set>
#include <string_view>

namespace cli {

constexpr const char* HELP = R"""(Raw image processing tool

USAGE: [OPTIONS] <path>...

OPTIONS:
        --ext  File extension to pick when passing through folder.
    -y, --yes  Automatically confirm all prompts
    -h, --help Show this help message

ARGS:
    <path>... File or folder with files to process. Defaults to current folder
)""";

struct Args {
    bool yes = false;
    bool help = false;
    std::vector<const char*> paths;
    std::unordered_set<std::string_view> extensions;

    Args() = default;
    int parse(int argc, char** argv);
};

Args parse(int argc, char** argv);

} //cli
