#include <iostream>
#include "data.hpp"
#include "cmd.hpp"
#include "db.hpp"

int main(){
    Vault vault;

    std::string cmd, master_password;

    std::cout << "Password Manager\n";

    std::cout << "Enter master password: ";
    std::cin >> master_password;

    load(vault, master_password);
    while(true){
        std::cout << "> ";
        std::cin >> cmd;

        if (cmd == "add") add(vault);
        else if (cmd == "list") list(vault);
        else if (cmd == "edit") edit(vault);
        else if (cmd == "remove") remove(vault);
        else if (cmd == "exit"){save(vault, master_password); break;}
        else std::cout << "unknown command\n";
    }
    
}