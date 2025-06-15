#include "Wallet.h"
#include <fstream>
#include <iostream>


Wallet::Wallet() : balance(0.0), points(0) {}

void Wallet::addFunds(double amount) {
    balance += amount;
}

void Wallet::deductFunds(double amount) {
    if (balance >= amount) {
        balance -= amount;
    }
}

void Wallet::addPoints(unsigned int points) {
    this->points += points;
}

void Wallet::deductPoints(unsigned int points) {
    if (this->points >= points) {
        this->points -= points;
    }
}

double Wallet::getBalance() const {
    return balance;
}

unsigned int Wallet::getPoints() const {
    return points;
}

void Wallet::display() const {
    std::cout << "Balance: " << balance << std::endl;
    std::cout << "Points: " << points << std::endl;
}

void Wallet::saveToFile(std::ofstream& out) const {
    out << balance << std::endl;
    out << points << std::endl;
}

void Wallet::loadFromFile(std::ifstream& in) {
    in >> balance;
    in >> points;
}