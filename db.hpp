#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include "data.hpp"


void save(const Vault& vault, std::string& password);
void load(Vault& vault, std::string& password);