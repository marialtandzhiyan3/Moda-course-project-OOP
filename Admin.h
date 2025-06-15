#pragma once
//#ifndef ADMIN_H
#define ADMIN_H
#include <fstream>

#include "User.h"
#include "Utils.h"

class Admin : public User {
private:
    struct Check {
        CharArray code;
        double amount;
        CharArray clientEgn;
        bool redeemed;
    };

    struct CustomerData {
        CharArray egn;
        CharArray name;
        double totalSpent;
        int orderCount;
    };

    Check* checks;
    unsigned int checksCount;
    unsigned int checksCapacity;

    void resizeChecks();

public:
    Admin(const CharArray& name, const CharArray& egn, const CharArray& password);
    ~Admin();

    void viewProfile() const override;
    void help() const override;

    void sendCheck(double amount, const CharArray& code, const CharArray& clientEgn);
    void customerInsights(User** users, unsigned int usersCount) const;

    void viewTransactions(User** users, unsigned int usersCount) const;


    void saveToFile(std::ofstream& out) const;
    void loadFromFile(std::ifstream& in);

    void viewAllProducts() const;
};


