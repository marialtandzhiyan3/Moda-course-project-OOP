#include "FileHandler.h"
#include "FileHandler.h"
#include <iostream>
#include <limits> 

void FileHandler::saveSystem(User** users, unsigned int usersCount, const char* filename ) {
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Error opening file for writing!" << std::endl;
        return;
    }

    out << usersCount << std::endl;
    
    for (unsigned int i = 0; i < usersCount; ++i) {

        if (dynamic_cast<Client*>(users[i])) {
            out << "CLIENT" << std::endl;
            dynamic_cast<Client*>(users[i])->saveToFile(out);
        }
        else if (dynamic_cast<BusinessProfile*>(users[i])) {
            out << "BUSINESS_PROFILE" << std::endl;
            dynamic_cast<BusinessProfile*>(users[i])->saveToFile(out);
        }
        else if (dynamic_cast<Admin*>(users[i])) {
            out << "ADMIN" << std::endl;
            dynamic_cast<Admin*>(users[i])->saveToFile(out);
        }
    }

    out.close();
}

User** FileHandler::loadSystem(unsigned int& usersCount, const char* filename ) {
    std::ifstream in(filename);
    if (!in) {
        std::cerr << "Error opening user file\n";
        usersCount = 0;
        return new User * [0];
    }

    if (!(in >> usersCount)) {
        std::cerr << "Invalid user count format\n";
        usersCount = 0;
        return new User * [0];
        
    }
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    User** users = new User * [usersCount];

    for (unsigned int i = 0; i < usersCount; ++i) {
        char typeBuffer[256];
        in.getline(typeBuffer, 256);
        CharArray type(typeBuffer);

        users[i] = createUserFromType(type, in);
    }

    in.close();
    return users;
}

User* FileHandler::createUserFromType(const CharArray& type, std::ifstream& in) {
    char nameBuffer[256];
    in.getline(nameBuffer, 256);
    CharArray name(nameBuffer);

    char egnBuffer[256];
    in.getline(egnBuffer, 256);
    CharArray egn(egnBuffer);

    char passBuffer[256];
    in.getline(passBuffer, 256);
    CharArray password(passBuffer);

    if (type == "CLIENT") {
        Client* client = new Client(name, egn, password);
        client->loadFromFile(in); 
        return client;
    }
    else if (type == "BUSINESS_PROFILE") {
        BusinessProfile* business = new BusinessProfile(name, egn, password);
        business->loadFromFile(in);
        return business;
    }
    else if (type == "ADMIN") {
        Admin* admin = new Admin(name, egn, password);
        admin->loadFromFile(in);
        return admin;
    }

    return nullptr;
}