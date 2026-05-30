#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include "db.hpp"
#include "crypto.hpp"

using json = nlohmann::json;

std::vector<uint8_t> pack(const EnData& e)
{
    std::vector<uint8_t> out;

    auto append = [&](const std::vector<uint8_t>& v)
    {
        uint32_t size = v.size();

        out.insert(out.end(),
                   (uint8_t*)&size,
                   (uint8_t*)&size + 4);

        out.insert(out.end(), v.begin(), v.end());
    };

    append(e.salt);
    append(e.nonce);
    append(e.tag);
    append(e.ciphertext);

    return out;
}

EnData unpack(const std::vector<uint8_t>& data)
{
    EnData e;
    size_t i = 0;

    auto read = [&](std::vector<uint8_t>& v)
    {
        uint32_t size;
        memcpy(&size, &data[i], 4);
        i += 4;

        v.assign(data.begin() + i, data.begin() + i + size);
        i += size;
    };

    read(e.salt);
    read(e.nonce);
    read(e.tag);
    read(e.ciphertext);

    return e;
}

void save(const Vault& vault, std::string& password){
    json j;

    for (const auto& e : vault.entries){
        j.push_back({
            {"id", e.id},
            {"name", e.name},
            {"login", e.login},
            {"password", e.password},
            {"notes", e.notes}
        });
    }

    std::string plaintext = j.dump();

    EnData enc = encrypt(plaintext, password);

    auto raw = pack(enc);

    std::ofstream file("vault.dat", std::ios::binary);
    file.write((char*)raw.data(), raw.size());
}

void load(Vault& vault, std::string& password){
    std::ifstream file("vault.dat", std::ios::binary);
    if (!file.is_open()) return;

    std::vector<uint8_t> raw((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    EnData enc = unpack(raw);

    std::string plaintext;

    try{
        plaintext = decrypt(enc, password);
    } catch(...){
        std::cout << "Wrong password or corrupted vault\n";
        return;
    }
    json j = json::parse(plaintext);

    for (const auto& item : j){
        Entry e;
        e.id = item["id"];
        e.name = item["name"];
        e.login = item["login"];
        e.password = item["password"];
        e.notes = item["notes"];

        vault.entries.push_back(e);
    }
}