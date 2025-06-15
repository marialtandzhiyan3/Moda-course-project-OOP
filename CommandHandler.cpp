#include "Client.h"
#include "BusinessProfile.h"
#include "Admin.h"
#include "Order.h"
#include "CommandHandler.h"
#include "ProductManager.h"
#include "FileHandler.h"

#include <iostream>
#include <cstring>
#include <limits>
#include <iomanip>
#pragma warning(disable : 4996)

CommandHandler::CommandHandler(User** users, unsigned int& usersCount,
    unsigned int& usersCapacity)
  : pm(ProductManager::getInstance()), users(users), usersCount(usersCount), usersCapacity(usersCapacity)
    , currentUser(nullptr){}

CommandHandler::~CommandHandler() {
    pm.saveToFile("Inventory.txt");
}

bool isValidEGN(const char* egn) {
    if (strlen(egn) != 10) {
        return false;
    }
    for (int i = 0; i < 10; ++i) {
        if (!isdigit(egn[i])) {
            return false;
        }
    }
    return true;
}

void CommandHandler::start() {
    const int BUFFER_SIZE = 256;
    char buffer[BUFFER_SIZE];

    while (true) {
        std::cout << "\n1. Login\n2. Register\n3. Exit\nChoice: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore();

        if (choice == 3) break;

        while (true) {
            std::cout << "Enter EGN: ";
            std::cin.getline(buffer, BUFFER_SIZE);

            if (isValidEGN(buffer)) {
                break; // valid input, exit loop
            }
            else {
                std::cout << "Incorrect input. EGN must be exactly 10 digits.\n";
            }
        }

        CharArray egn(buffer);

        std::cout << "Enter Password: ";
        std::cin.getline(buffer, BUFFER_SIZE);
        CharArray password(buffer);

        if (choice == 1) {
            for (unsigned i = 0; i < usersCount; ++i) {
                if (users[i]->getEgn() == egn && users[i]->authenticate(password)) {
                    currentUser = users[i];
                    break;
                }
            }
        }
        else if (choice == 2) {
            std::cout << "Enter Name: ";
            std::cin.getline(buffer, BUFFER_SIZE);
            CharArray name(buffer);

            // register type
            std::cout << "User Type (client/business/admin): ";
            std::cin.getline(buffer, BUFFER_SIZE);
            CharArray type(buffer);

            if (type == "client") {
                users[usersCount++] = new Client(name, egn, password);
            }
            else if (type == "business") {
                users[usersCount++] = new BusinessProfile(name, egn, password);
            }
            else if (type == "admin") {
                users[usersCount++] = new Admin(name, egn, password);
            }

            currentUser = users[usersCount - 1];
        }

        if (currentUser) {
            if (dynamic_cast<Client*>(currentUser)) handleClientCommands();
            else if (dynamic_cast<BusinessProfile*>(currentUser)) handleBusinessCommands();
            else if (dynamic_cast<Admin*>(currentUser)) handleAdminCommands();

            currentUser = nullptr;
        }
        else {
            std::cout << "Invalid credentials or user type!" << std::endl;
        }
    }
}

void CommandHandler::handleClientCommands() {
    Client* client = dynamic_cast<Client*>(currentUser);
    while (true) {
        std::cout << "\nClient Menu:\n"
            << "1. View Profile\n"
            << "2. Redeem code\n"
            << "3. Check Balance\n"
            << "4. View Products\n"
            << "5. Filter Products\n"
            << "6. View Product Detail\n"
            << "7. Add to Cart\n"
            << "8. View Cart\n"  
            << "9. Remove from Cart\n"
            << "10. Apply Discount\n"
            << "11. Checkout\n"   
            << "12. Remove Discount\n"
            << "13. Order History\n"
            << "14. Confirm Order Delivery\n"
            << "15. Request Return\n" 
            << "16. Logout\n"
            << "Choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
        case 1: client->viewProfile(); break;
        case 2: client->redeem(); break;
        case 3: client->checkBalance(); break;
        case 4: //showProducts(); break;
            client->viewAllProducts(); break;
        case 5: client->viewAllProductsSorted(); break;
        case 6: client->viewProductDetails(); break;
        case 7: purchaseFlow(client); break;
        case 8:
            client->viewCart();         
            break;
        case 9:
            if (client->getCartSize() > 0) {
                client->viewCart();
                std::cout << "Enter item number to remove: ";
                unsigned int index;
                std::cin >> index;
                std::cin.ignore();

                if (index >= 1 && index <= client->getCartSize()) {
                    client->removeFromCart(index - 1);  // Convert to 0-based index
                }
                else {
                    std::cout << "Invalid item number!" << std::endl;
                }
            }
            break;
        case 10:client->applyDiscount(); break;
        case 11:
        {
            client->checkout();
            Order* order = client->getLastOrder();
            if (order) {
                for (unsigned i = 0; i < usersCount; ++i) {
                    if (BusinessProfile* bp = dynamic_cast<BusinessProfile*>(users[i])) {
                        bp->addPendingOrder(order);
                        std::cout << "Order assigned to business.\n";
                        break;
                    }
                }
            }
        }
            break;
        case 12:client->removeDiscount(); break;
        case 13:
            client->listOrders();
            break;
        case 14: {  
            client->listOrders();
            if (client->getOrdersCount() > 0) {
                std::cout << "Enter order ID to confirm: ";
                unsigned int orderId;
                std::cin >> orderId;
                std::cin.ignore();
                client->confirmOrder(orderId);
            }
            break;
        }
        case 15: {
            client->listOrders();
            std::cout << "Enter Order ID to request return: ";
            unsigned int orderId; std::cin >> orderId; std::cin.ignore();
            client->requestReturn(orderId);
            break;
        }

            break;
        case 16:
            return;
        default:
            std::cout << "Invalid choice!" << std::endl;
        }
    }
}

void CommandHandler::handleBusinessCommands() {
    BusinessProfile* business = dynamic_cast<BusinessProfile*>(currentUser);
    if (!business) return;

    while (true) {
        std::cout << "\nBusiness Menu:\n"
            << "1. Add Product\n"
            << "2. Remove Product\n"
            << "3. List Pending Orders\n"
            << "4. Approve Orders\n"
            << "5. Reject Orders\n"
            << "6. List All Orders\n"
            << "7. List Best Selling Orders\n"
            << "8. View Revenue\n"
            << "9. List Return Requests\n"
            << "10.Approve Return Request\n"
            << "11.Reject Return Request\n"
            << "12.Logout\n"
            << "Choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
        case 1: {
            char nameBuffer[256], descBuffer[256];
            double price;
            unsigned int quantity;

            std::cout << "Product Name: ";
            std::cin.getline(nameBuffer, sizeof(nameBuffer));
            CharArray name(nameBuffer);

            std::cout << "Price: ";
            std::cin >> price;

            std::cout << "Quantity: ";
            std::cin >> quantity;
            std::cin.ignore();
            std::cout << "Description: ";
            std::cin.getline(descBuffer, sizeof(descBuffer));
            CharArray description(descBuffer);

            business->addItem(name, price, quantity, description);
            break;
        }
        case 2: {
            char nameBuffer[256];
            std::cout << "Product Name: ";
            std::cin.getline(nameBuffer, sizeof(nameBuffer));
            CharArray name(nameBuffer);
            business->removeItem(name);
            break;
        }
        case 3:
            business->listPendingOrders();
            break;
        case 4:
            business->approveOrder();
            break;
        case 5:
            business->rejectOrder();
            break;
        case 6:
            business->listAllOrders();
            break;
        case 7:
            business->viewBestSellingProducts();
            break;
        case 8:
            business->viewRevenue();
            break;
        case 9:
            business->listReturnRequests();
            break;
        case 10:
           
            business->listReturnRequests();
            std::cout << "Index to approve: ";
            unsigned idx; std::cin >> idx; std::cin.ignore();
            business->approveReturn(idx);
            break;
        case 11: {
           
            business->listReturnRequests();
            std::cout << "Index to reject: ";
            unsigned idx; std::cin >> idx; std::cin.ignore();
            char buf[256];
            std::cout << "Reason: ";
            std::cin.getline(buf, 256);
            business->rejectReturn(idx, CharArray(buf));
            break;
        }
        case 12: {
            
            return;
        }
        default:
            std::cout << "Invalid choice!" << std::endl;
        }
    }
}

void CommandHandler::handleAdminCommands() {
    Admin* admin = dynamic_cast<Admin*>(currentUser);
    if (!admin) return;

    while (true) {
        std::cout << "\nAdmin Menu:\n"
            << "1. Send Check\n"
            << "2. View Transactions\n"
            << "3. Customer Insights\n"
            << "4. Logout\n"
            << "Choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
        case 1: {
            double amount;
            char codeBuffer[256], egnBuffer[256];

            std::cout << "Enter amount: ";
            std::cin >> amount;
            std::cin.ignore();
            std::cout << "Enter code: ";
            std::cin.getline(codeBuffer, sizeof(codeBuffer));
            CharArray code(codeBuffer);

            std::cout << "Enter client EGN: ";
            std::cin.getline(egnBuffer, sizeof(egnBuffer));
            CharArray clientEgn(egnBuffer);

            admin->sendCheck(amount, code, clientEgn);
            break;
        }
        case 2:
            admin->viewTransactions(users, usersCount);
            break;
        case 3:
            admin->customerInsights(users, usersCount);
            break;
        case 4:
            return;
        default:
            std::cout << "Invalid choice!" << std::endl;
        }
    }
}

void CommandHandler::showProducts() const {
    Product** products = pm.getAllProducts();
    unsigned int count = pm.getProductsCount();

    std::cout << "\nAvailable Products (" << count << "):" << std::endl;
    for (unsigned int i = 0; i < count; ++i) {
        std::cout << "ID: " << products[i]->getId()
            << " | Name: " << products[i]->getName().getData()
            << " | Price: " << products[i]->getPrice()
            << " | Stock: " << products[i]->getQuantity()
            << " | Description: " << products[i]->getDescription().getData()
            << " | Rating: " << products[i]->getRating()

            << std::endl;
    }
}

void CommandHandler::purchaseFlow(Client* client) {
    if (!client) return;

    ProductManager& pm = ProductManager::getInstance();
    Product** products = pm.getAllProducts();
    unsigned int count = pm.getProductsCount();

    while (true) {
        std::cout << "\nAvailable Products:\n";
        for (unsigned i = 0; i < count; ++i) {
            std::cout << i + 1 << ". " << products[i]->getName().getData()
                << " | Price: " << products[i]->getPrice()
                << " | Stock: " << products[i]->getQuantity() << std::endl;
        }
        std::cout << "Enter product number (0 to finish): ";
        int choice;
        std::cin >> choice;
        std::cin.ignore();

        if (choice == 0) break;
        if (choice < 1 || choice > static_cast<int>(count)) {
            std::cout << "Invalid choice!" << std::endl;
            continue;
        }
        Product* selectedProduct = products[choice - 1];
        std::cout << "Enter quantity: ";
        unsigned quantity;
        std::cin >> quantity;
        std::cin.ignore();

        if (selectedProduct->getQuantity() < quantity) {
            std::cout << "Insufficient stock!" << std::endl;
            continue;
        }

        client->addToCart(selectedProduct, quantity);
        std::cout << "Added to cart: " << selectedProduct->getName().getData() << std::endl;
    }


}