#include "fourdst/crypt/openSSL_utils.h"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <openssl/evp.h>

namespace fourdst::crypt::utils {
    std::string calculate_sha256(const std::filesystem::path& filepath) {
        // 1. Set up the context using a smart pointer for automatic cleanup
        std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> mdctx(EVP_MD_CTX_new(), &EVP_MD_CTX_free);
        if (!mdctx) {
            throw std::runtime_error("Failed to create EVP_MD_CTX: " + get_openssl_error());
        }

        // 2. Initialize the digest operation
        if (1 != EVP_DigestInit_ex(mdctx.get(), EVP_sha256(), nullptr)) {
            throw std::runtime_error("Failed to initialize SHA256 digest: " + get_openssl_error());
        }

        // 3. Open the file for reading
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open file for checksum: " + filepath.string());
        }

        // 4. Read the file in chunks and update the digest
        std::vector<char> buffer(4096);
        while (file.good()) {
            file.read(buffer.data(), static_cast<long>(buffer.size()));
            const std::streamsize bytes_read = file.gcount();
            if (bytes_read > 0) {
                if (1 != EVP_DigestUpdate(mdctx.get(), buffer.data(), bytes_read)) {
                    throw std::runtime_error("Failed to update SHA256 digest: " + get_openssl_error());
                }
            }
        }

        // 5. Finalize the hash
        unsigned char hash[EVP_MAX_MD_SIZE];
        unsigned int digest_len = 0;
        if (1 != EVP_DigestFinal_ex(mdctx.get(), hash, &digest_len)) {
            throw std::runtime_error("Failed to finalize SHA256 digest: " + get_openssl_error());
        }

        // 6. Format the hash into a hex string
        std::stringstream ss;
        for (unsigned int i = 0; i < digest_len; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
        }

        return ss.str();
    }

    std::string calculate_sha256_from_buffer(const std::vector<unsigned char>& data) {
        const std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> md_ctx_st(EVP_MD_CTX_new(), &EVP_MD_CTX_free);
        if (!md_ctx_st) {
            throw std::runtime_error("Failed to create EVP_MD_CTX: " + get_openssl_error());
        }

        if (1 != EVP_DigestInit_ex(md_ctx_st.get(), EVP_sha256(), nullptr)) {
            throw std::runtime_error("Failed to initialize SHA256 digest: " + get_openssl_error());
        }

        if (1 != EVP_DigestUpdate(md_ctx_st.get(), data.data(), data.size())) {
            throw std::runtime_error("Failed to update SHA256 digest from buffer: " + get_openssl_error());
        }

        unsigned char hash[EVP_MAX_MD_SIZE];
        unsigned int digest_len = 0;
        if (1 != EVP_DigestFinal_ex(md_ctx_st.get(), hash, &digest_len)) {
            throw std::runtime_error("Failed to finalize SHA256 digest: " + get_openssl_error());
        }

        std::stringstream ss;
        for (unsigned int i = 0; i < digest_len; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
        }

        return ss.str();
    }
}