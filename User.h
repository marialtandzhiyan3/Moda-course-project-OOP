#pragma once
#ifndef USER_H
#define USER_H
#include "Utils.h"
#include <fstream>

class User {
protected:
    CharArray name;
    CharArray egn;
    CharArray password;

public:
    User(const CharArray& name, const CharArray& egn, const CharArray& password);
    virtual ~User() = default;

    virtual void viewProfile() const = 0;
    virtual void help() const = 0;

    bool authenticate(const CharArray& inputPassword) const;

    const CharArray& getName() const;
    const CharArray& getEgn() const;

    virtual void viewAllProducts() const=0;

    virtual void saveToFile(std::ofstream& out) const;
    virtual void loadFromFile(std::ifstream& in);

};

#endif
