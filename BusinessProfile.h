#pragma once
#ifndef BUSINESS_PROFILE_H
#define BUSINESS_PROFILE_H
#include <fstream>

#include "User.h"
#include "Product.h"
#include "Order.h"

class BusinessProfile : public User {
private:
    Product** products=nullptr;
    unsigned int* productIds;
    unsigned int productsCount;
    unsigned int productsCapacity;

    Order** pendingOrders;
    unsigned int pendingOrdersCount;
    unsigned int pendingOrdersCapacity;

    double totalRevenue;

    void resizeProductIds();

    Order** processedOrders;
    unsigned int processedCount;
    unsigned int processedCapacity;

    void resizeProcessedOrders();

    static const unsigned MAX_RETURN_REQ = 100;
    Order** returnRequests;    
    unsigned returnReqCount;
    unsigned returnReqCap;

    void resizeReturnRequests();

public:
    BusinessProfile(const CharArray& name, const CharArray& egn, const CharArray& password);
    ~BusinessProfile();

    void viewProfile() const override;
    void help() const override;

    void addItem(const CharArray& name, double price, unsigned int quantity, const CharArray& description, double rating=5.0);
    void removeItem(const CharArray& name);

    void listPendingOrders() const;
    void addPendingOrder(Order* order);
    Order** getPendingOrders() const; 
    unsigned int getPendingOrdersCount() const; 
    void approveOrder();
    void rejectOrder();

    void addReturnRequest(Order* o);
    void listReturnRequests() const;
    void approveReturn(unsigned idx);
    void rejectReturn(unsigned idx, const CharArray& reason);


    void viewRevenue() const;

    void saveToFile(std::ofstream& out) const;
    void loadFromFile(std::ifstream& in);
    void viewAllProducts() const override;

    void listAllOrders() const;

    void viewBestSellingProducts() const;
};

#endif
