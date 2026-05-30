#include "crypto.hpp"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <argon2.h>
#include <stdexcept>
#include <cstring>

std::vector<uint8_t> randBytes(size_t size){
    std::vector<uint8_t> buf(size);
    
    if(RAND_bytes(buf.data(), static_cast<int>(size)) != 1){
        throw std::runtime_error("RAND_bytes failed");

    }
    return buf;
}

std::vector<uint8_t> deriveKey(const std::string& password, std::vector<uint8_t>& salt){
    std::vector<uint8_t> key(32);
    
    int res = argon2d_hash_raw(3, 65536, 1, password.data(), password.size(), salt.data(), salt.size(), key.data(), key.size());

    if(res != ARGON2_OK){
        throw std::runtime_error("Argon2 failed");
    }

    return key;
}

EnData encrypt(const std::string& plaintext, const std::string& password){
    EnData out;
    
    out.salt = randBytes(16);
    out.nonce = randBytes(12);

    auto key = deriveKey(password, out.salt);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("CTX alloc failed");

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1)
        throw std::runtime_error("EncryptInit failed");

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, out.nonce.size(), nullptr) != 1)
        throw std::runtime_error("IVLEN set failed");

    if (EVP_EncryptInit_ex(ctx, nullptr, nullptr, key.data(), out.nonce.data()) != 1)
        throw std::runtime_error("Key init failed");

    out.ciphertext.resize(plaintext.size());

    int len = 0, total = 0;
    if (EVP_EncryptUpdate(ctx, out.ciphertext.data(), &len, reinterpret_cast<const uint8_t*>(plaintext.data()), plaintext.size()) != 1)
        throw std::runtime_error("EncryptUpdate failed");

    total = len;

    if (EVP_EncryptFinal_ex(ctx, out.ciphertext.data() + len, &len) != 1)
        throw std::runtime_error("EncryptFinal failed");

    total += len;
    out.ciphertext.resize(total);

    out.tag.resize(16);

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, out.tag.data()) != 1)
        throw std::runtime_error("GET_TAG failed");

    EVP_CIPHER_CTX_free(ctx);

    return out;
}

std::string decrypt(
    const EnData& data,
    std::string& password)
{
    auto key = deriveKey(password, const_cast<std::vector<uint8_t>&>(data.salt));

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) throw std::runtime_error("CTX alloc failed");

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1)
        throw std::runtime_error("DecryptInit failed");

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, data.nonce.size(), nullptr) != 1)
        throw std::runtime_error("IVLEN set failed");

    if (EVP_DecryptInit_ex(ctx, nullptr, nullptr, key.data(), data.nonce.data()) != 1)
        throw std::runtime_error("Key init failed");

    std::string plaintext;
    plaintext.resize(data.ciphertext.size());

    int len = 0, total = 0;

    if (EVP_DecryptUpdate(
            ctx,
            reinterpret_cast<uint8_t*>(plaintext.data()),
            &len,
            data.ciphertext.data(),
            data.ciphertext.size()) != 1)
        throw std::runtime_error("DecryptUpdate failed");

    total = len;

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16,
                            const_cast<uint8_t*>(data.tag.data())) != 1)
        throw std::runtime_error("SET_TAG failed");

    int ret = EVP_DecryptFinal_ex(
        ctx,
        reinterpret_cast<uint8_t*>(plaintext.data()) + len,
        &len
    );

    EVP_CIPHER_CTX_free(ctx);

    if (ret <= 0)
        throw std::runtime_error("Invalid password or corrupted data");

    total += len;
    plaintext.resize(total);

    return plaintext;
}

