#pragma once
#include <cassert>
#include <string>

bool find_option(const std::string& tag, int argc, const char* argv[]) {
    for (int i = 0; i < argc; ++i) {
        if (argv[i] == tag) return true;
    }
    return false;
}

unsigned get_option(const std::string& tag, int argc, const char* argv[],
                    const unsigned& dflt) {
    if (find_option(tag, argc, argv)) {
        for (int i = 0; i < argc; ++i) {
            if (argv[i] == tag) {
                assert(argv[i + 1][0] != '-');
                return std::atoi(argv[i + 1]);
            }
        }
    }
    return dflt;
}