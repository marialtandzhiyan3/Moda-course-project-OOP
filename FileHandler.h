#pragma once
#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "User.h"
#include "Client.h"
#include "BusinessProfile.h"
#include "Admin.h"
#include <fstream>

class FileHandler {
public:
    static void saveSystem(User** users, unsigned int usersCount, const char* filename = "users.txt");
    static User** loadSystem(unsigned int& usersCount, const char* filename = "users.txt");

private:
    static User* createUserFromType(const CharArray& type, std::ifstream& in);
};

#endif
