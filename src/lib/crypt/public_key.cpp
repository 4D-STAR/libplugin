#include "fourdst/crypt/public_key.h"
#include "fourdst/crypt/openSSL_utils.h"

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/encoder.h>

#include <vector>
#include <string>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <memory>

namespace {
    enum class KeyFormat {
        PEM,
        DER,
        UNKNOWN
    };

    KeyFormat detect_key_format(const std::vector<unsigned char>& data) {
        // skip leading ASCII whitespace (space, tab, CR, LF)
        std::size_t pos = 0;
        while (pos < data.size() &&
              (data[pos] == 0x20 /* ' ' */ ||
               data[pos] == 0x09 /* '\\t' */ ||
               data[pos] == 0x0D /* '\\r' */ ||
               data[pos] == 0x0A /* '\\n' */))
        {
            ++pos;
        }

        static constexpr char pemHeader[] = "-----BEGIN ";
        static constexpr std::size_t headerLen = sizeof(pemHeader) - 1;
        if (pos + headerLen <= data.size() &&
            std::memcmp(data.data() + pos, pemHeader, headerLen) == 0)
        {
            return KeyFormat::PEM;
        }

        if (pos < data.size() && data[pos] == 0x30)
        {
            return KeyFormat::DER;
        }

        return KeyFormat::UNKNOWN;
    }

    EVP_PKEY* load_pkey_from_der(const std::vector<unsigned char>& derData)
    {
        const unsigned char* p = derData.data();
        if (!p) {
            throw std::runtime_error("DER data pointer is null");
        }
        EVP_PKEY* pkey = d2i_PUBKEY(nullptr, &p, static_cast<long>(derData.size()));
        if (!pkey) {
            throw std::runtime_error("Failed to load private key from DER data: " + fourdst::crypt::utils::get_openssl_error());
        }
        return pkey;
    }

    EVP_PKEY* load_pkey_from_pem(const std::vector<unsigned char>& pemData)
    {
        BIO* bio = BIO_new_mem_buf(pemData.data(),
                                   static_cast<int>(pemData.size()));
        if (!bio) {
            throw std::runtime_error("Failed to create BIO for PEM data: " + fourdst::crypt::utils::get_openssl_error());
        }

        EVP_PKEY* pkey = PEM_read_bio_PUBKEY(bio,nullptr,nullptr, nullptr);
        BIO_free(bio);
        if (!pkey) {
            throw std::runtime_error("Failed to load private key from PEM data: " + fourdst::crypt::utils::get_openssl_error());
        }
        return pkey;
    }

    EVP_PKEY* load_pkey_from_vector(const std::vector<unsigned char>& data) {
        switch (detect_key_format(data)) {
            case KeyFormat::PEM:
                return load_pkey_from_pem(data);
            case KeyFormat::DER:
                return load_pkey_from_der(data);
            default:
                throw std::runtime_error("Unrecognized key format in provided data.");
        }
    }
}


namespace fourdst::crypt {
    PublicKey::PublicKey(const char* filepath) {
        const std::unique_ptr<FILE, decltype(&fclose)> pubkey_file(fopen(filepath, "r"), &fclose);
        if (!pubkey_file) {
            throw std::runtime_error("Failed to open public key file: " + std::string(filepath));
        }

        EVP_PKEY* raw_pkey = PEM_read_PUBKEY(pubkey_file.get(), nullptr, nullptr, nullptr);
        if (!raw_pkey) {
            throw std::runtime_error("Failed to parse public key from " + std::string(filepath) + ". OpenSSL error: " + utils::get_openssl_error());
        }
        m_pkey.reset(raw_pkey);
        m_initialized = true;
    }

    PublicKey::PublicKey(const std::string &filepath) : PublicKey(filepath.c_str()) {}

    PublicKey::PublicKey(const std::filesystem::path &filepath) : PublicKey(filepath.c_str()) {}

    PublicKey::PublicKey(const std::vector<unsigned char>& data) {
        EVP_PKEY* raw_pkey = load_pkey_from_vector(data);
        m_pkey.reset(raw_pkey);
    }


    std::string PublicKey::type() const {
        if (!m_pkey) return "Invalid";
        const int key_type = EVP_PKEY_get_base_id(m_pkey.get());
        const char* name = OBJ_nid2sn(key_type);
        return name ? name : "Unknown";
    }

    size_t PublicKey::size() const {
        if (!m_pkey) return 0;
        return static_cast<size_t>(EVP_PKEY_get_bits(m_pkey.get()));
    }

    EVP_PKEY* PublicKey::get() const {
        return m_pkey.get();
    }

    std::string PublicKey::get_fingerprint() const {
        if (!m_pkey) {
            throw std::runtime_error("Cannot generate fingerprint from an invalid key.");
        }

        std::unique_ptr<BIO, decltype(&BIO_free)> mem_bio(BIO_new(BIO_s_mem()), &BIO_free);
        if (!mem_bio) {
            throw std::runtime_error("Failed to create memory BIO: " + utils::get_openssl_error());
        }

        std::unique_ptr<OSSL_ENCODER_CTX, decltype(&OSSL_ENCODER_CTX_free)> ctx(
            OSSL_ENCODER_CTX_new_for_pkey(m_pkey.get(), EVP_PKEY_PUBLIC_KEY, "DER", "SubjectPublicKeyInfo", nullptr),
            &OSSL_ENCODER_CTX_free
        );
        if (!ctx) {
            throw std::runtime_error("Failed to create OSSL_ENCODER_CTX: " + utils::get_openssl_error());
        }

        if (!OSSL_ENCODER_to_bio(ctx.get(), mem_bio.get())) {
            throw std::runtime_error("Failed to encode public key to BIO: " + utils::get_openssl_error());
        }

        BUF_MEM* bptr = nullptr;
        BIO_get_mem_ptr(mem_bio.get(), &bptr);
        if (!bptr || !bptr->data || bptr->length == 0) {
            throw std::runtime_error("Failed to get data from memory BIO after encoding.");
        }

        const std::vector<unsigned char> der_vector(
            reinterpret_cast<unsigned char*>(bptr->data),
            reinterpret_cast<unsigned char*>(bptr->data) + bptr->length
        );


        const std::string hash_hex = utils::calculate_sha256_from_buffer(der_vector);

        return "sha256:" + hash_hex;
    }

    bool PublicKey::is_initialized() const {
        return m_initialized;
    }

    bool PublicKey::operator==(const PublicKey &other) const {
        switch (EVP_PKEY_eq(m_pkey.get(), other.m_pkey.get())) {
            case 1:
                return true;
            case 0:
                return false;
            default:
                throw std::runtime_error("Error comparing public keys: " + utils::get_openssl_error());
        }
    }
    bool PublicKey::operator!=(const PublicKey &other) const {
        return !(*this == other);
    }

    bool PublicKey::operator==(const std::vector<unsigned char>& data) const {
        const PublicKey other(data);
        return *this == other;
    }

    bool PublicKey::operator!=(const std::vector<unsigned char>& data) const {
        return !(*this == data);
    }


}