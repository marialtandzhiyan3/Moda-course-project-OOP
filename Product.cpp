#include "Product.h"

Product::Product(unsigned int id, const CharArray& name, double price,
    unsigned int quantity, const CharArray& description,double rating)
    : id(id), name(name), price(price), quantity(quantity),
    description(description), rating(0.0), available(quantity > 0), salesCount(0) {}

unsigned int Product::getId() const {
    return id;
}

const CharArray& Product::getName() const {
    return name;
}

double Product::getPrice() const {
    return price;
}

unsigned int Product::getQuantity() const {
    return quantity;
}

const CharArray& Product::getDescription() const {
    return description;
}

double Product::getRating() const {
    return rating;
}

bool Product::isAvailable() const {
    return available;
}

void Product::setQuantity(unsigned int newQuantity) {
    quantity = newQuantity;
    available = (quantity > 0);
}

void Product::updateRating(double newRating) {
    rating = newRating;
}

void Product::incrementSalesCount(unsigned int quantity) {
    salesCount += quantity;
}

unsigned int Product::getSalesCount() const {
    return salesCount;
}
