#pragma once

#include "fourdst/crypt/public_key.h"

namespace fourdst::crypt {
    bool verify_signature(
        const PublicKey& key,
        const std::vector<unsigned char>& data_to_verify,
        const std::vector<unsigned char>& signature
    );
}