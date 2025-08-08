#pragma once

#include <iostream>
#include <string>
#include <memory>

#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include <filesystem>

namespace fourdst::crypt {
    // Custom deleter for EVP_PKEY
    struct EVP_PKEY_Deleter {
        void operator()(EVP_PKEY* pkey) const {
            EVP_PKEY_free(pkey);
        }
    };

    using Unique_EVP_PKEY = std::unique_ptr<EVP_PKEY, EVP_PKEY_Deleter>;

    class PublicKey {
    public:
        PublicKey() = default;

        explicit PublicKey(const std::string& filepath);
        explicit PublicKey(const std::filesystem::path& filepath);
        explicit PublicKey(const char* filepath);

        explicit PublicKey(const std::vector<unsigned char>& data);

        ~PublicKey() = default;

        PublicKey(const PublicKey&) = delete;
        PublicKey& operator=(const PublicKey&) = delete;

        PublicKey(PublicKey&&) = default;
        PublicKey& operator=(PublicKey&&) noexcept = default;

        [[nodiscard]] std::string type() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] EVP_PKEY* get() const;

        [[nodiscard]] std::string get_fingerprint() const;

        [[nodiscard]] bool is_initialized() const;

        bool operator==(const PublicKey& other) const;
        bool operator!=(const PublicKey& other) const;
        bool operator==(const std::vector<unsigned char>& data) const;
        bool operator!=(const std::vector<unsigned char>& data) const;

        friend std::ostream& operator<<(std::ostream& os, const PublicKey& key) {
            os << "PublicKey(Type: " << key.type() << ", Size: " << key.size() << " bits)";
            return os;
        }

    private:
        Unique_EVP_PKEY m_pkey;
        bool m_initialized = false;
    };
}

