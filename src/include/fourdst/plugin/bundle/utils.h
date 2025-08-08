#pragma once

#include <filesystem>
#include <iostream>
#include <random>

namespace fourdst::plugin::bundle::utils {
    class TemporaryDirectory {
public:
    // Constructor: pick a unique directory under the system temp dir, create it
    TemporaryDirectory();

    TemporaryDirectory(const TemporaryDirectory&) = delete;
    TemporaryDirectory& operator=(const TemporaryDirectory&) = delete;
    TemporaryDirectory(TemporaryDirectory&& other) noexcept;

    TemporaryDirectory& operator=(TemporaryDirectory&& other) noexcept;

    ~TemporaryDirectory();

    [[nodiscard]] std::filesystem::path get_path() const;

private:
    std::filesystem::path directoryPath;

    void cleanup() const noexcept
    {
        try {
            if (!directoryPath.empty() &&
                std::filesystem::exists(directoryPath))
            {
                std::filesystem::remove_all(directoryPath);
            }
        }
        catch (const std::exception& e) {
            // swallow: destructors must not throw
            std::cerr << "Warning: failed to remove "
                      << directoryPath << ": " << e.what() << "\n";
        }
    }
};
}
