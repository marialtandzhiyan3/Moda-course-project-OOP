#ifndef PRODUCT_H
#define PRODUCT_H

#include "Utils.h"

class Product {
private:
    unsigned int id;
    CharArray name;
    double price;
    unsigned int quantity;
    CharArray description;
    double rating;
    bool available;
    unsigned int salesCount;

public:
    Product(unsigned int id, const CharArray& name, double price,
        unsigned int quantity, const CharArray& description,double rating);

    unsigned int getId() const;
    const CharArray& getName() const;
    double getPrice() const;
    unsigned int getQuantity() const;
    const CharArray& getDescription() const;
    double getRating() const;

    bool isAvailable() const;

    void setQuantity(unsigned int newQuantity);
    void updateRating(double newRating);

    void incrementSalesCount(unsigned int quantity);
    unsigned int getSalesCount() const;

};

#endif