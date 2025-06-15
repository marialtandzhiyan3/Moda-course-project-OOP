#pragma once
#ifndef CLIENT_H
#define CLIENT_H

#include "User.h"
#include "Wallet.h"
#include "Order.h"

class Client : public User {
private:
    Wallet wallet;
    Order** orders;
    unsigned int ordersCount;
    unsigned int ordersCapacity;
    unsigned int capacity;



    void resizeOrders();
    struct CartItem {
        Product* product;
        unsigned int quantity;
    };

    CartItem* cart;
    struct SavedCartItem {
        int productId;
        unsigned int quantity;
    };

    SavedCartItem* savedCart;
    unsigned int savedCartSize;

    unsigned int cartSize;
    unsigned int cartCapacity;

    void saveCartToFile(std::ofstream& out) const;
    void loadCartFromFile(std::ifstream& in);
    void clearCart();

    static unsigned int lastOrderId; 
    unsigned int generateOrderId();  

    double calculateTotal() const;

    Order* lastOrder;  

    unsigned int pointsUsedForDiscount;
public:
    Client(const CharArray& name, const CharArray& egn, const CharArray& password);
    ~Client();

    void viewProfile() const override;
    void help() const override;

    void checkBalance() const;
    void redeem();
    void addOrder(Order* order);
    void viewAllProducts() const override;
    void listOrders() const;
    void viewProductDetails() const;


    void saveToFile(std::ofstream& out) const;
    void loadFromFile(std::ifstream& in);

    void addToCart(Product* product, unsigned int quantity);
    void viewCart() const;
    void resolveCartItems();

    void removeFromCart(unsigned int index);
    unsigned int getCartSize() const { return cartSize; }

    void confirmOrder(unsigned int orderId);
    void checkout();

    unsigned int getOrdersCount() const { return ordersCount; }

    double applyDiscount();

    Order* getLastOrder() const;

    double getTotalSpent() const;

  
    bool requestReturn(unsigned int orderId);

    void refund(double amount);

    double removeDiscount();
    void viewAllProductsSorted() const;

   Order** getOrders() const { return orders; }
    

};

#endif