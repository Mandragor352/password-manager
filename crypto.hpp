#pragma once

#include <string>
#include <vector>
#include <cstdint>

struct EnData{
    std::vector<uint8_t> salt;
    std::vector<uint8_t> nonce;
    std::vector<uint8_t> tag;
    std::vector<uint8_t> ciphertext;
};

std::vector<uint8_t> randBytes(size_t size);

std::vector<uint8_t> deriveKey(const std::string& password, std::vector<uint8_t>& salt);

EnData encrypt(const std::string& plaintext, const std::string& password);

std::string decrypt(const EnData& data, std::string& password);