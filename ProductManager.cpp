#include "ProductManager.h"
#include <fstream>
#include <iostream>
#include <cstring>

ProductManager* ProductManager::instance = nullptr;

ProductManager::ProductManager()
    : products(nullptr), productsCount(0), productsCapacity(2) {
    products = new Product * [productsCapacity];
    loadFromFile();
}


ProductManager::~ProductManager() {

    saveToFile(); 
    for (unsigned i = 0; i < productsCount; ++i) delete products[i];
    delete[] products;
}


ProductManager& ProductManager::getInstance() {
    if (!instance) {
        instance = new ProductManager();
    }
    return *instance;
}

void ProductManager::resizeProducts() {
    productsCapacity *= 2;
    Product** newProducts = new Product * [productsCapacity];
    for (unsigned i = 0; i < productsCount; ++i) newProducts[i] = products[i];
    delete[] products;
    products = newProducts;
}

void ProductManager::addProduct(Product* product) {
    if (productsCount == productsCapacity) resizeProducts();
    products[productsCount++] = product;
}

Product* ProductManager::getProductById(int id) {
    for (unsigned i = 0; i < productsCount; ++i) {
        if (products[i]->getId() == id) {
            return products[i];
        }
    }
    return nullptr;
}

void ProductManager::removeProduct(unsigned int id) {
    for (unsigned i = 0; i < productsCount; ++i) {
        if (products[i]->getId() == id) {
            delete products[i];
            for (unsigned j = i; j < productsCount - 1; ++j) products[j] = products[j + 1];
            productsCount--;
            return;
        }
    }
}

Product* ProductManager::getProduct(unsigned int id) const {
    for (unsigned int i = 0; i < productsCount; ++i) {
        if (products[i]->getId() == id) {
            return products[i];
        }
    }
    return nullptr;
}

unsigned int ProductManager::getNextProductId() const {
    unsigned int maxId = 0;
    for (unsigned i = 0; i < productsCount; ++i) {
        if (products[i]->getId() > maxId) maxId = products[i]->getId();
    }
    return maxId + 1;
}


Product** ProductManager::getAllProducts() const {
    return products;
}

unsigned int ProductManager::getProductsCount() const {
    return productsCount;
}

void ProductManager::saveToFile(const char* filename) const {
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    for (unsigned i = 0; i < productsCount; ++i) {
        out << products[i]->getId() << ","
            << products[i]->getName().getData() << ","
            << products[i]->getPrice() << ","
            << products[i]->getQuantity() << ","
            << products[i]->getDescription().getData() << ','
            << products[i]->getRating() << '\n';
    }
    out.close();
}

void ProductManager::loadFromFile(const char* filename) {
    std::ifstream in(filename);
    if (!in) {
        std::cerr << "Inventory file not found, creating new." << std::endl;
        return;
    }

    for (unsigned i = 0; i < productsCount; ++i) delete products[i];
    productsCount = 0;

    unsigned int id, quantity;
    double price,rating;
    char name[256], desc[256];

    while (in >> id) {
        in.ignore(); 
        in.getline(name, 256, ',');
        in >> price;
        in.ignore();
        in >> quantity;
        in.ignore();
        in.getline(desc, 256, ',');
        in >> rating;
        in.ignore();


        addProduct(new Product(id, CharArray(name), price, quantity, CharArray(desc),rating));
    }
    in.close();
}