#include "Admin.h"
#include "Client.h"
#include "ProductManager.h"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstring> 

Admin::Admin(const CharArray& name, const CharArray& egn, const CharArray& password)
    : User(name, egn, password), checks(nullptr), checksCount(0), checksCapacity(2) {
    checks = new Check[checksCapacity];
}

Admin::~Admin() {
    delete[] checks;
}

void Admin::resizeChecks() {
    checksCapacity *= 2;
    Check* newChecks = new Check[checksCapacity];

    for (unsigned int i = 0; i < checksCount; ++i) {
        newChecks[i] = checks[i];
    }

    delete[] checks;
    checks = newChecks;
}

void Admin::viewProfile() const {
    std::cout << "Admin Profile:" << std::endl;
    std::cout << "Name: " << getName().getData() << std::endl;
    std::cout << "EGN: " << getEgn().getData() << std::endl;
    std::cout << "Active Checks: " << checksCount << std::endl;
}

void Admin::help() const {
    std::cout << "Available commands for Admin:" << std::endl;
    std::cout << "send-check [amount] [code] [client_egn] - Issue a check" << std::endl;
    std::cout << "customer-insights - Show customer statistics" << std::endl;
    std::cout << "view-transactions - View all financial transactions" << std::endl;
}

void Admin::sendCheck(double amount, const CharArray& code, const CharArray& clientEgn) {
    if (checksCount == checksCapacity) {
        resizeChecks();
    }

    checks[checksCount].amount = amount;
    checks[checksCount].code = code;
    checks[checksCount].clientEgn = clientEgn;
    checks[checksCount].redeemed = false;
    checksCount++;

    std::ofstream out("checks.txt", std::ios::app);
    if (out) {
        out << clientEgn.getData() << " " << code.getData() <<" " << amount << "\n";
        std::cout << "Check issued successfully!" << std::endl;
    }
    else {
        std::cerr << "Warning: could not open checks.txt for writing\n";
    }

    
}

 void Admin::customerInsights(User * *users, unsigned int usersCount) const {
     for (unsigned i = 0; i < usersCount; ++i) {
         if (Client* client = dynamic_cast<Client*>(users[i])) {
             std::cout << "Name:         " << client->getName().getData() << "\n";
             std::cout << "EGN:          " << client->getEgn().getData() << "\n";
             std::cout << "Orders:       " << client->getOrdersCount() << "\n";
             std::cout << "Total Spent:  "
                 << std::fixed << std::setprecision(2)
                 << client->getTotalSpent() << " BGN\n\n";
         }
     }
        std::cout << "-----------------------------------------\n";
 }

 void Admin::viewTransactions(User** users, unsigned int usersCount) const {
     std::cout << "\n=== Checks Issued ===\n";
     if (checksCount == 0) {
         std::cout << "No checks have been issued.\n";
     }
     else {
         std::cout
             << std::left
             << std::setw(15) << "Code"
             << std::setw(12) << "Recipient"
             << std::setw(10) << "Amount"
             << std::setw(10) << "Redeemed"
             << "\n";
         std::cout << "--------------------------------------\n";

         for (unsigned i = 0; i < checksCount; ++i) {
             const Check& ck = checks[i];
             std::cout
                 << std::setw(15) << ck.code.getData()
                 << std::setw(12) << ck.clientEgn.getData()
                 << std::fixed << std::setprecision(2)
                 << std::setw(10) << ck.amount
                 << std::setw(10) << (ck.redeemed ? "Yes" : "No")
                 << "\n";
         }
     }
     std::cout << "\n=== Orders Placed ===\n";
     bool anyOrder = false;

     for (unsigned u = 0; u < usersCount; ++u) {
         if (Client* client = dynamic_cast<Client*>(users[u])) {
             Order** ords = client->getOrders();
             unsigned int ordCount = client->getOrdersCount();

             for (unsigned j = 0; j < ordCount; ++j) {
                 Order* o = ords[j];
                 std::cout
                     << "Client: " << std::setw(12) << client->getName().getData()
                     << " | EGN: " << std::setw(10) << client->getEgn().getData()
                     << " | OrderID: " << std::setw(4) << o->getId()
                     << " | Total: " << std::fixed << std::setprecision(2)
                     << o->getTotalPrice() << " BGN\n";
                 anyOrder = true;
             }
         }
     }

     if (!anyOrder) {
         std::cout << "No orders have been placed yet.\n";
     }
 }
void Admin::saveToFile(std::ofstream& out) const {
    out << name.getData() << std::endl;
    out << egn.getData() << std::endl;
    out << password.getData() << std::endl;

    out << checksCount << std::endl;
    for (unsigned int i = 0; i < checksCount; ++i) {
        out << checks[i].amount << std::endl;
        out << checks[i].code.getData() << std::endl;
        out << checks[i].clientEgn.getData() << std::endl;
        out << checks[i].redeemed << std::endl;
    }
}

void Admin::loadFromFile(std::ifstream& in) {
    unsigned int count;
    in >> count;
    in.ignore();

    for (unsigned int i = 0; i < count; ++i) {
        double amount;
        in >> amount;
        in.ignore();

        char codeBuffer[256];
        in.getline(codeBuffer, 256);
        CharArray code(codeBuffer);

        char egnBuffer[256];
        in.getline(egnBuffer, 256);
        CharArray clientEgn(egnBuffer);

        bool redeemed;
        in >> redeemed;
        in.ignore();

        if (checksCount == checksCapacity) {
            resizeChecks();
        }

        checks[checksCount].amount = amount;
        checks[checksCount].code = code;
        checks[checksCount].clientEgn = clientEgn;
        checks[checksCount].redeemed = redeemed;
        checksCount++;
    }
    
}

void Admin::viewAllProducts() const {
    ProductManager& pm = ProductManager::getInstance();
    unsigned int count = pm.getProductsCount();
    Product** all = pm.getAllProducts();
    for (unsigned int i = 0; i < count; ++i) {
        Product* p = all[i];
        std::cout << "ID: " << p->getId()
            << " Name: " << p->getName().getData()
            << " Price: " << p->getPrice()
            << " Qty: " << p->getQuantity()
            << " Desc: " << p->getDescription().getData()
            << " Rating: " << p->getRating()
            << " Available: " << (p->isAvailable() ? "Yes" : "No")
            << std::endl;
    }
}

