/**
 * @file openSSL_utils.h
 * @brief OpenSSL utility functions for cryptographic operations.
 * 
 * This header provides utility functions for working with OpenSSL, including
 * error handling and common cryptographic operations like SHA-256 hashing.
 */

#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <openssl/err.h>
#include <filesystem>

namespace fourdst::crypt::utils {
    /**
     * @brief Retrieves the most recent OpenSSL error messages as a formatted string.
     * 
     * This function retrieves all pending OpenSSL error messages from the error queue
     * and returns them as a single formatted string. Each error is separated by a semicolon.
     * 
     * @return std::string A string containing all pending OpenSSL error messages.
     * 
     * @note This function clears the OpenSSL error queue after retrieving the messages.
     * 
     * @par Example
     * @code
     * if (some_openssl_function() != 1) {
     *     std::cerr << "OpenSSL error: " << fourdst::crypt::utils::get_openssl_error() << std::endl;
     * }
     * @endcode
     */
    inline std::string get_openssl_error() {
        std::stringstream ss;
        unsigned long err_code;
        while ((err_code = ERR_get_error()) != 0) {
            char err_buf[256];
            ERR_error_string_n(err_code, err_buf, sizeof(err_buf));
            ss << err_buf << "; ";
        }
        return ss.str();
    }

    /**
     * @brief Calculates the SHA-256 hash of a file's contents.
     * 
     * This function reads the contents of the specified file and calculates its
     * SHA-256 hash. The hash is returned as a hexadecimal string.
     * 
     * @param[in] filepath The path to the file to hash.
     * @return std::string The SHA-256 hash of the file contents as a lowercase hexadecimal string.
     * 
     * @throws std::runtime_error If the file cannot be opened or read.
     * @throws std::runtime_error If an OpenSSL error occurs during hashing.
     * 
     * @note The file is read in binary mode.
     * 
     * @par Example
     * @code
     * try {
     *     std::string hash = fourdst::crypt::utils::calculate_sha256("document.txt");
     *     std::cout << "SHA-256 hash: " << hash << std::endl;
     * } catch (const std::exception& e) {
     *     std::cerr << "Error: " << e.what() << std::endl;
     * }
     * @endcode
     */
    std::string calculate_sha256(const std::filesystem::path& filepath);

    /**
     * @brief Calculates the SHA-256 hash of a binary buffer.
     * 
     * This function calculates the SHA-256 hash of the provided binary data buffer
     * and returns the result as a hexadecimal string.
     * 
     * @param[in] data The binary data to hash.
     * @return std::string The SHA-256 hash of the input data as a lowercase hexadecimal string.
     * 
     * @throws std::runtime_error If an OpenSSL error occurs during hashing.
     * 
     * @par Example
     * @code
     * std::vector<unsigned char> data = {'h', 'e', 'l', 'l', 'o'};
     * std::string hash = fourdst::crypt::utils::calculate_sha256_from_buffer(data);
     * std::cout << "SHA-256 hash: " << hash << std::endl;
     * @endcode
     */
    std::string calculate_sha256_from_buffer(const std::vector<unsigned char>& data);
}
