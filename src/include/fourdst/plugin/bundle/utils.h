/**
 * @file utils.h
 * @brief Utility classes and functions for the bundle module.
 * 
 * This header provides utility functionality used by the bundle module,
 * including temporary directory management.
 */

#pragma once

#include <filesystem>
#include <iostream>
#include <random>

namespace fourdst::plugin::bundle::utils {
    /**
     * @brief Manages a temporary directory that is automatically cleaned up.
     * 
     * This class creates a uniquely named temporary directory upon construction
     * and automatically removes it when the object is destroyed. It's useful for
     * operations that require temporary storage, such as extracting bundle contents.
     * 
     * @par Example: Using TemporaryDirectory
     * @code
     * try {
     *     // Create a temporary directory
     *     fourdst::plugin::bundle::utils::TemporaryDirectory tempDir;
     *     
     *     // Get the path to the temporary directory
     *     std::filesystem::path tempPath = tempDir.get_path();
     *     
     *     // Use the temporary directory...
     *     std::ofstream tempFile(tempPath / "example.txt");
     *     tempFile << "Temporary data";
     *     
     *     // The directory and its contents will be automatically deleted when tempDir goes out of scope
     * } catch (const std::exception& e) {
     *     std::cerr << "Error: " << e.what() << std::endl;
     * }
     * @endcode
     */
    class TemporaryDirectory {
    public:
        /**
         * @brief Construct a new TemporaryDirectory object.
         * 
         * Creates a uniquely named temporary directory in the system's temporary directory.
         * The directory will be automatically removed when this object is destroyed.
         * 
         * @throws std::runtime_error If the temporary directory cannot be created.
         */
        TemporaryDirectory();

        // Prevent copying
        TemporaryDirectory(const TemporaryDirectory&) = delete;
        TemporaryDirectory& operator=(const TemporaryDirectory&) = delete;

        /**
         * @brief Move constructor.
         * 
         * @param other The TemporaryDirectory to move from.
         */
        TemporaryDirectory(TemporaryDirectory&& other) noexcept;

        /**
         * @brief Move assignment operator.
         * 
         * @param other The TemporaryDirectory to move from.
         * @return TemporaryDirectory& Reference to this object.
         */
        TemporaryDirectory& operator=(TemporaryDirectory&& other) noexcept;

        /**
         * @brief Destroy the TemporaryDirectory object.
         * 
         * Automatically removes the temporary directory and all its contents.
         */
        ~TemporaryDirectory();

        /**
         * @brief Get the path to the temporary directory.
         * 
         * @return std::filesystem::path The filesystem path to the temporary directory.
         */
        [[nodiscard]] std::filesystem::path get_path() const;

    private:
        std::filesystem::path directoryPath;  ///< Path to the temporary directory

        /**
         * @brief Clean up the temporary directory.
         * 
         * This function is called by the destructor to remove the temporary directory.
         * It's marked noexcept to ensure it doesn't throw during destruction.
         */
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
                // Log the error but don't throw from a destructor
                std::cerr << "Warning: failed to remove "
                          << directoryPath << ": " << e.what() << "\n";
            }
        }
    };
}
