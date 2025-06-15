#include "Order.h"
#include "ProductManager.h"
#include <fstream>

Order::Order()
    : id(0), products(nullptr), productsCount(0), totalPrice(0.0), status(OrderStatus::PENDING), client(nullptr) {}

Order::Order(unsigned int id, Product** products, unsigned int* quantities, unsigned int count, double totalPrice, User* client)
    : id(id), products(products), quantities(quantities), productsCount(count), totalPrice(totalPrice), status(OrderStatus::PENDING), client(client) {
    this->products = new Product * [count];
    for (unsigned int i = 0; i < count; ++i) {
        this->products[i] = products[i];

    }
}

Order::~Order() {
    delete[] products;
    delete[] quantities;
}

void Order::setStatus(OrderStatus newStatus) {
    status = newStatus;
}

OrderStatus Order::getStatus() const {
    return status;
}

unsigned int Order::getId() const {
    return id;
}

double Order::getTotalPrice() const {
    return totalPrice;
}

unsigned int Order::getPointsEarned() const {
    return pointsEarned;
}

void Order::saveToFile(std::ofstream& out) const {
    out << id << '\n';
    out << totalPrice << '\n';
    out << static_cast<int>(status) << '\n';
    out << productsCount << '\n';
    for (unsigned i = 0; i < productsCount; ++i) {
        out << products[i]->getId() << ' ' << quantities[i] << '\n';
    }
    out << client->getEgn().getData() << '\n';  
}


void Order::loadFromFile(std::ifstream& in) {
    in >> id;
    in >> totalPrice;

    int statusInt;
    in >> statusInt;
    status = static_cast<OrderStatus>(statusInt);


    in >> productsCount;
    products = new Product * [productsCount];
    quantities = new unsigned int[productsCount];

    for (unsigned i = 0; i < productsCount; ++i) {
        int productId;
        in >> productId;
        in >> quantities[i];

        // Get product from ProductManager 
        products[i] = ProductManager::getInstance().getProductById(productId);
    }
    std::string clientEgnStr;
    in >> clientEgnStr;
}

unsigned int Order::getProductsCount() const {
    return productsCount;
}

Product* Order::getProductByIndex(unsigned int index) const {
    if (index < productsCount) return products[index];
    return nullptr;
}

unsigned int Order::getQuantityByIndex(unsigned int index) const {
    if (index < productsCount) return quantities[index];
    return 0;
}