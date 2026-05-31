#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include "data.hpp"

struct Header {
    char magic[4] = {'V','L','T','1'};
    uint32_t version = 1;
};

void save(const Vault& vault, std::string& password);
bool load(Vault& vault, std::string& password);