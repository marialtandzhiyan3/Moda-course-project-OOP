#include "CommandHandler.h"
#include "FileHandler.h"
#include "ProductManager.h"
#include "Client.h"
#include "BusinessProfile.h"
#include "Admin.h"

#include <iostream>
#include <fstream>

int main() {
   
    std::ofstream usersFile("users.txt", std::ios::app);
    usersFile.close();
    std::ofstream invFile("Inventory.txt", std::ios::app);
    invFile.close();

    ProductManager& pm = ProductManager::getInstance();
    pm.loadFromFile("Inventory.txt");

   
    unsigned usersCount = 0;
    unsigned usersCapacity = 10;            
    User** users = FileHandler::loadSystem(usersCount, "users.txt");

    if (usersCapacity < usersCount + 1) {
        usersCapacity = usersCount + 1;
        
    }

    if (usersCount == 0) {
        delete[] users;                     
        usersCapacity = 2;
        users = new User * [usersCapacity];
        users[0] = new Admin ("Admin", "0000000000", "admin123");
        usersCount = 1;
        std::cout << "A default administrator has been created.";
    }

    for (unsigned i = 0; i < usersCount; ++i) {
        if (Client* client = dynamic_cast<Client*>(users[i])) {
            client->resolveCartItems();
        }
    }
  
     CommandHandler cmdHandler(users, usersCount, usersCapacity);
    cmdHandler.start();

    FileHandler::saveSystem(users, usersCount, "users.txt");
    pm.saveToFile("Inventory.txt");

    for (unsigned i = 0; i < usersCount; ++i) {
        delete users[i];
    }
    delete[] users;

    return 0;
}