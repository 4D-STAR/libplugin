#include "fourdst/plugin/bundle/utils.h"

#include <algorithm>
#include <string>

namespace {
    // ReSharper disable once CppDFAConstantParameter
    std::string random_string(const size_t length )
    {
        auto randchar = []() -> char
        {
            std::random_device random_device;
            std::mt19937 generator(random_device());
            std::uniform_int_distribution<int> distribution(0,  100);
            const int random_int = distribution(generator);
            constexpr char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
            constexpr size_t max_index = (sizeof(charset) - 1);
            return charset[ random_int % max_index ];
        };
        std::string str(length,0);
        std::generate_n( str.begin(), length, randchar );
        return str;
    }
}

namespace fourdst::plugin::bundle::utils {
    TemporaryDirectory::TemporaryDirectory() {
        const std::filesystem::path base = std::filesystem::temp_directory_path();
        const auto unique = std::filesystem::path(random_string(10));
        directoryPath = base / unique;

        std::filesystem::create_directory(directoryPath);
    }

    TemporaryDirectory::TemporaryDirectory(TemporaryDirectory&& other) noexcept
        : directoryPath(std::move(other.directoryPath))
    {
        other.directoryPath.clear();
    }

    TemporaryDirectory& TemporaryDirectory::operator=(TemporaryDirectory&& other) noexcept {
        if (this != &other) {
            cleanup();
            directoryPath = std::move(other.directoryPath);
            other.directoryPath.clear();
        }
        return *this;
    }

    TemporaryDirectory::~TemporaryDirectory() {
        cleanup();
    }

    std::filesystem::path TemporaryDirectory::get_path() const {
        return directoryPath;
    }
}