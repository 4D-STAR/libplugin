/**
 * @file public_key.h
 * @brief Public key handling for cryptographic operations.
 * 
 * This header defines the PublicKey class for managing public keys in various formats,
 * including loading from files and memory, and performing key-related operations.
 */

#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include <filesystem>

namespace fourdst::crypt {
    /**
     * @brief Custom deleter for OpenSSL EVP_PKEY objects.
     * 
     * This deleter ensures proper cleanup of OpenSSL EVP_PKEY objects
     * when used with std::unique_ptr.
     */
    struct EVP_PKEY_Deleter {
        /**
         * @brief Frees the EVP_PKEY object.
         * 
         * @param[in] pkey The EVP_PKEY object to free.
         */
        void operator()(EVP_PKEY* pkey) const {
            EVP_PKEY_free(pkey);
        }
    };

    /// Unique pointer type for managing EVP_PKEY objects with automatic cleanup.
    using Unique_EVP_PKEY = std::unique_ptr<EVP_PKEY, EVP_PKEY_Deleter>;

    /**
     * @brief Represents a public key for cryptographic operations.
     * 
     * This class provides an interface for loading and working with public keys
     * in various formats (PEM, DER) and performing key-related operations.
     * 
     * @note This class is move-constructible and move-assignable, but not copyable.
     */
    class PublicKey {
    public:
        /**
         * @brief Constructs an uninitialized PublicKey object.
         */
        PublicKey() = default;

        /**
         * @brief Constructs a PublicKey by loading from a file.
         * 
         * @param[in] filepath Path to the file containing the public key in PEM format.
         * 
         * @throws std::runtime_error If the file cannot be opened or the key cannot be loaded.
         */
        explicit PublicKey(const std::string& filepath);
        
        /**
         * @brief Constructs a PublicKey by loading from a file.
         * 
         * @param[in] filepath Filesystem path to the file containing the public key in PEM format.
         * 
         * @throws std::runtime_error If the file cannot be opened or the key cannot be loaded.
         */
        explicit PublicKey(const std::filesystem::path& filepath);
        
        /**
         * @brief Constructs a PublicKey by loading from a file.
         * 
         * @param[in] filepath C-style string path to the file containing the public key in PEM format.
         * 
         * @throws std::runtime_error If the file cannot be opened or the key cannot be loaded.
         */
        explicit PublicKey(const char* filepath);

        /**
         * @brief Constructs a PublicKey from binary data in memory.
         * 
         * The data can be in either PEM or DER format. The format is automatically detected.
         * 
         * @param[in] data Binary data containing the public key.
         * 
         * @throws std::runtime_error If the key cannot be loaded from the provided data.
         * @throws std::runtime_error If the key format is not recognized.
         */
        explicit PublicKey(const std::vector<unsigned char>& data);

        ~PublicKey() = default;

        // Deleted copy constructor and assignment operator
        PublicKey(const PublicKey&) = delete;
        PublicKey& operator=(const PublicKey&) = delete;

        // Allow move semantics
        PublicKey(PublicKey&&) = default;
        PublicKey& operator=(PublicKey&&) noexcept = default;

        /**
         * @brief Gets the type of the public key.
         * 
         * @return std::string A string representing the key type (e.g., "RSA", "EC"),
         *                    or "Invalid" if the key is not initialized.
         */
        [[nodiscard]] std::string type() const;

        /**
         * @brief Gets the size of the public key in bits.
         * 
         * @return size_t The size of the key in bits, or 0 if the key is not initialized.
         */
        [[nodiscard]] size_t size() const;

        /**
         * @brief Gets the underlying OpenSSL EVP_PKEY object.
         * 
         * @return EVP_PKEY* A pointer to the OpenSSL EVP_PKEY, or nullptr if not initialized.
         * 
         * @warning The returned pointer is owned by this PublicKey object and should not be freed.
         */
        [[nodiscard]] EVP_PKEY* get() const;

        /**
         * @brief Gets a fingerprint of the public key.
         * 
         * The fingerprint is a SHA-256 hash of the key's DER-encoded SubjectPublicKeyInfo.
         * 
         * @return std::string A string in the format "sha256:<hex_encoded_hash>".
         * 
         * @throws std::runtime_error If the key is not initialized or an error occurs.
         */
        [[nodiscard]] std::string get_fingerprint() const;

        /**
         * @brief Checks if the PublicKey is initialized with a valid key.
         * 
         * @return bool True if the key is initialized, false otherwise.
         */
        [[nodiscard]] bool is_initialized() const;

        /**
         * @brief Compares two PublicKey objects for equality.
         * 
         * @param[in] other The PublicKey to compare with.
         * @return bool True if the keys are equal, false otherwise.
         * 
         * @throws std::runtime_error If an OpenSSL error occurs during comparison.
         */
        bool operator==(const PublicKey& other) const;
        
        /**
         * @brief Compares two PublicKey objects for inequality.
         * 
         * @param[in] other The PublicKey to compare with.
         * @return bool True if the keys are not equal, false otherwise.
         */
        bool operator!=(const PublicKey& other) const;
        
        /**
         * @brief Compares the PublicKey with raw key data for equality.
         * 
         * @param[in] data The raw key data to compare with.
         * @return bool True if the keys are equal, false otherwise.
         */
        bool operator==(const std::vector<unsigned char>& data) const;
        
        /**
         * @brief Compares the PublicKey with raw key data for inequality.
         * 
         * @param[in] data The raw key data to compare with.
         * @return bool True if the keys are not equal, false otherwise.
         */
        bool operator!=(const std::vector<unsigned char>& data) const;

        /**
         * @brief Outputs a string representation of the PublicKey to a stream.
         * 
         * @param[in,out] os The output stream to write to.
         * @param[in] key The PublicKey to output.
         * @return std::ostream& Reference to the output stream.
         */
        friend std::ostream& operator<<(std::ostream& os, const PublicKey& key) {
            os << "PublicKey(Type: " << key.type() << ", Size: " << key.size() << " bits)";
            return os;
        }

    private:
        Unique_EVP_PKEY m_pkey;  ///< The underlying OpenSSL key object.
        bool m_initialized = false;  ///< Flag indicating if the key is initialized.
    };
}

