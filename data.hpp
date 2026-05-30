#pragma once

#include <vector>
#include <string>
#include <cstdint>

struct Entry{
    uint64_t id;

    std::string name;
    std::string login;
    std::string password;
    std::string notes;
};

struct Vault{
    std::vector<Entry> entries;
    bool dirty = true;
};

