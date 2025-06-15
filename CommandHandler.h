#pragma once
#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "User.h"
#include "Client.h"
#include "BusinessProfile.h"
#include "Admin.h"
#include "ProductManager.h"
#include "FileHandler.h"

class CommandHandler {
private:
    User* currentUser;
    ProductManager& pm;
    User**& users;
    unsigned int& usersCount;
    unsigned int& usersCapacity;

    void handleClientCommands();
    void handleBusinessCommands();
    void handleAdminCommands();

    void showProducts() const;
    void purchaseFlow(Client* client);

public:
    CommandHandler(User** users, unsigned int& usersCount, unsigned int& usersCapacity);
    ~CommandHandler();

    void start();
};

#endif

