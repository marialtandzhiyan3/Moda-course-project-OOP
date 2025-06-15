#pragma once
#ifndef WALLET_H
#define WALLET_H
#include <fstream>

class Wallet {
private:
    double balance;
    unsigned int points;

public:
    Wallet();

    void addFunds(double amount);
    void deductFunds(double amount);
    void addPoints(unsigned int points);
    void deductPoints(unsigned int points);

    double getBalance() const;
    unsigned int getPoints() const;

    void display() const;

    void saveToFile(std::ofstream& out) const;
    void loadFromFile(std::ifstream& in);
};

#endif
