#pragma once
#ifndef ORDER_H
#define ORDER_H

#include "Product.h"
#include "User.h" 
#include <fstream>

enum class OrderStatus { PENDING, SHIPPED, DELIVERED, REJECTED, RETURNED, RETURN_REQUESTED, RETURN_REJECTED};

class Order {
private:
    unsigned int id;
    Product** products;
    unsigned int* quantities;
    unsigned int productsCount;
    double totalPrice;
    unsigned int pointsEarned;
    OrderStatus status;
    User* client;
    CharArray rejectionReason;

public:
    Order();
    Order(unsigned int id, Product** products, unsigned int* quantities,
        unsigned int count, double price, User* client);
    ~Order();

    void setStatus(OrderStatus newStatus);
    OrderStatus getStatus() const;
    unsigned int getId() const;
    double getTotalPrice() const;
    unsigned int getPointsEarned() const;
    User* getClient() const { return client; }

    void saveToFile(std::ofstream& out) const;
    void loadFromFile(std::ifstream& in);

    void setRejectionReason(const CharArray& reason) { rejectionReason = reason; }
    CharArray getRejectionReason() const { return rejectionReason; }

    unsigned int getProductsCount() const;
    Product* getProductByIndex(unsigned int index) const;
    unsigned int getQuantityByIndex(unsigned int index) const;
};

#endif