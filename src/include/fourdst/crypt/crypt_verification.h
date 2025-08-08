/**
 * @file crypt_verification.h
 * @brief Provides cryptographic signature verification functionality.
 * 
 * This header defines functions for verifying digital signatures using public key
 * cryptography. It supports various key types through the OpenSSL backend.
 */

#pragma once

#include "fourdst/crypt/public_key.h"

namespace fourdst::crypt {
    /**
     * @brief Verifies a digital signature against given data using the provided public key.
     * 
     * This function verifies that the signature was created by the private key corresponding
     * to the provided public key and that the data has not been tampered with.
     * 
     * @param[in] key The public key to use for verification.
     * @param[in] data_to_verify The data that was signed.
     * @param[in] signature The signature to verify.
     * @return bool True if the signature is valid, false otherwise.
     * 
     * @throws std::runtime_error If:
     *         - The public key is invalid or null.
     *         - Memory allocation for the verification context fails.
     *         - The verification context cannot be initialized.
     *         - An error occurs during the verification process.
     * 
     * @note The function uses the default digest and padding scheme appropriate for the key type.
     * 
     * @par Example
     * @code
     * try {
     *     fourdst::crypt::PublicKey pubKey("public_key.pem");
     *     std::vector<unsigned char> data = {'t', 'e', 's', 't'};
     *     std::vector<unsigned char> signature = get_signature(); // Get signature from somewhere
     *     
     *     if (fourdst::crypt::verify_signature(pubKey, data, signature)) {
     *         std::cout << "Signature is valid" << std::endl;
     *     } else {
     *         std::cout << "Signature is invalid" << std::endl;
     *     }
     * } catch (const std::exception& e) {
     *     std::cerr << "Error: " << e.what() << std::endl;
     * }
     * @endcode
     */
    bool verify_signature(
        const PublicKey& key,
        const std::vector<unsigned char>& data_to_verify,
        const std::vector<unsigned char>& signature
    );
}