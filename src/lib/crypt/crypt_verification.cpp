#include "fourdst/crypt/crypt_verification.h"
#include "fourdst/crypt/public_key.h"

#include <sstream>

#include "fourdst/crypt/openSSL_utils.h"


namespace fourdst::crypt {
    struct EVP_MD_CTX_Deleter {
        void operator()(EVP_MD_CTX* ctx) const {
            if (ctx) {
                EVP_MD_CTX_free(ctx);
            }
        }
    };

    bool verify_signature(
        const PublicKey& key,
        const std::vector<unsigned char>& data_to_verify,
        const std::vector<unsigned char>& signature
    ) {
        EVP_PKEY* pkey = key.get();
        if (!pkey) {
            throw std::runtime_error("Public key is null.");
        }

        const std::unique_ptr<EVP_MD_CTX, EVP_MD_CTX_Deleter> md_ctx(EVP_MD_CTX_new());
        if (!md_ctx) {
            throw std::runtime_error("Failed to create EVP_MD_CTX: " + utils::get_openssl_error());
        }

        if (1 != EVP_DigestVerifyInit(md_ctx.get(), nullptr, nullptr, nullptr, pkey)) {
            throw std::runtime_error("Failed to initialize digest verification context: " + utils::get_openssl_error());
        }

        const int result = EVP_DigestVerify(
            md_ctx.get(),
            signature.data(),
            signature.size(),
            data_to_verify.data(),
            data_to_verify.size()
        );

        if (result < 0) {
            throw std::runtime_error("Error during final digest verification: " + utils::get_openssl_error());
        }

        return result == 1;
    }}

