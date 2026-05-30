#include <iostream>
#include <algorithm>
#include "cmd.hpp"


void add(Vault& vault){
    Entry e;
    
    std::cout << "id: ";
    std::cin >> e.id;

    std::cout << "name: ";
    std::cin >> e.name;

    std::cout << "login: ";
    std::cin >> e.login;

    std::cout << "password: ";
    std::cin >> e.password;

    std::cout << "notes: ";
    std::cin >> e.notes;

    vault.entries.push_back(e);
    vault.dirty = true;
}

void list(Vault& vault){
    if (vault.entries.empty()) {
        std::cout << "no entries\n";
        return;
    }
    if(vault.dirty){
        std::sort(vault.entries.begin(), vault.entries.end(), [](const Entry& a, const Entry& b) {return a.id < b.id; });
        vault.dirty = false;
    }
    std::cout << "id" << " | " << "name" << " | " << "login" << " | " << "password" << " | " << "notes" << "\n";
    for(const auto& e : vault.entries){
        std::cout << e.id << " | " << e.name << " | " << e.login << " | " << e.password << " | " << e.notes << "\n";
    }
}

void remove(Vault& vault){
    uint64_t id;
    std::cout << "id remove: ";
    std::cin >> id;

    for(auto it = vault.entries.begin(); it != vault.entries.end(); ++it){
        if (it->id == id){
            vault.entries.erase(it);
            std::cout << "remove\n";
            return;
        }
    }
    std::cout << "not found\n";
}


void edit(Vault& vault){
    uint64_t id;
    std::cout << "id edit: ";
    std::cin >> id;

    for (auto& e : vault.entries){
        if(e.id == id){
            int choise;
            std:: cout << "what to edit?\n";
            std:: cout << "1. name\n";
            std:: cout << "2. login\n";
            std:: cout << "3. password\n";
            std:: cout << "4. notes\n";
            std:: cout << "> ";

            std::cin >> choise;
            
            std::string new_value;

            std::cout << "new value: ";
            std::cin >> new_value;

            switch(choise){
            case 1: e.name = new_value; break;
            case 2: e.login = new_value; break;
            case 3: e.password = new_value; break;
            case 4: e.notes = new_value; break;
            default: std::cout << "invalid choice\n"; return;
            }

            std::cout << "update\n";
            return;
        }
    }

    std::cout << "not found\n";
}