#pragma once

#include <sstream>
#include <string>
#include <openssl/err.h>
#include <filesystem>

namespace fourdst::crypt::utils {
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

    std::string calculate_sha256(const std::filesystem::path& filepath);

    std::string calculate_sha256_from_buffer(const std::vector<unsigned char>& data);
}
