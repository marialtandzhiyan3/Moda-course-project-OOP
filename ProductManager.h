#pragma once
#ifndef PRODUCT_MANAGER_H
#define PRODUCT_MANAGER_H

#include "Product.h"
static constexpr const char* INVENTORY_FILE = "Inventory.txt";

class ProductManager {
private:
    static ProductManager* instance;
    Product** products;
    unsigned int productsCount;
    unsigned int productsCapacity;

    ProductManager();
    void resizeProducts();

public:
    ~ProductManager();

    static ProductManager& getInstance();

    void addProduct(Product* product);
    void removeProduct(unsigned int id);
    Product* getProduct(unsigned int id) const;
    Product** getAllProducts() const;
    Product* getProductById(int id);
    unsigned int getProductsCount() const;
    unsigned int getNextProductId() const;

    void saveToFile(const char* filename = "Inventory.txt") const;
    void loadFromFile(const char* filename = "Inventory.txt");

    ProductManager(const ProductManager&) = delete;
    ProductManager& operator=(const ProductManager&) = delete;
};

#endif