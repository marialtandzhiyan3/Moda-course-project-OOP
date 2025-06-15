#include "Client.h"
#include "Order.h"
#include "ProductManager.h"
#include "Order.h" 
#include <iostream>
#include <sstream>  

unsigned int Client::lastOrderId = 0;
Client::Client(const CharArray& name, const CharArray& egn, const CharArray& password)
    : User(name, egn, password), cart(nullptr), cartSize(0), cartCapacity(2),
    orders(nullptr), ordersCount(0), ordersCapacity(10),
    savedCart(nullptr), savedCartSize(0), lastOrder(nullptr), pointsUsedForDiscount(0) {  // Initialize saved cart
    cart = new CartItem[cartCapacity];
    orders = new Order * [ordersCapacity];
}

Client::~Client() {
    for (unsigned int i = 0; i < ordersCount; ++i) {
        delete orders[i];
    }
    delete[] orders;
    delete[] cart;

    if (savedCart) {
        delete[] savedCart;
    }
}

void Client::resizeOrders() {
    capacity *= 2;
    Order** newOrders = new Order * [capacity];

    for (unsigned int i = 0; i < ordersCount; ++i) {
        newOrders[i] = orders[i];
    }

    delete[] orders;
    orders = newOrders;
}

void Client::viewProfile() const {
    std::cout << "Client Profile:" << std::endl;
    std::cout << "Name: " << name.getData() << std::endl;
    std::cout << "EGN: " << egn.getData() << std::endl;
    wallet.display();
}

void Client::help() const {
    std::cout << "Available commands:" << std::endl;
    std::cout << "check-balance - Show current balance" << std::endl;
    std::cout << "redeem [code] - Redeem a check" << std::endl;
 
}

void Client::checkBalance() const {
    wallet.display();
}

void Client::redeem( ) {
    char codeBuf[128];
    std::cout << "Enter check code: ";
    std::cin.getline(codeBuf, sizeof(codeBuf));
    CharArray enteredCode(codeBuf);

    char buffer[256];
    bool found = false;

    std::ifstream in("checks.txt");
    if (!in) {
        std::cerr << "No checks available!\n";
        return;
    }

    while (in.getline(buffer, sizeof(buffer))) {
   
        std::istringstream iss(buffer);
        std::string fileEGN, fileCode;
        double amount;

        if (!(iss >> fileEGN >> fileCode >>amount)) continue;  

        if (CharArray(fileEGN.c_str()) == this->egn
            && CharArray(fileCode.c_str()) == enteredCode) {
            wallet.addFunds(amount);
            found = true;
            std::cout << "Redeemed " << amount << " BGN using code " << enteredCode.getData()<< "\n";
            found = true;
            break;
        }
    }

    if (!found) {
        std::cerr << "No valid check found for your account!\n";
    }
}

void Client::addOrder(Order* order) {
    if (ordersCount == capacity) {
        resizeOrders();
    }
    orders[ordersCount++] = order;
}
void Client::viewAllProducts() const {
    ProductManager& pm = ProductManager::getInstance();
    Product** products = pm.getAllProducts();
    unsigned int count = pm.getProductsCount();

    std::cout << "Available Products (" << count << "):" << std::endl;
    for (unsigned int i = 0; i < count; ++i) {
        std::cout << "ID: " << products[i]->getId()
            << " | Name: " << products[i]->getName().getData()
            << " | Price: " << products[i]->getPrice()
            << " | Quantity: " << products[i]->getQuantity()
            << " | Description: " << products[i]->getDescription().getData()
            << " | Rating: " << products[i]->getRating()
            << std::endl;
    }
}

void Client::listOrders() const {
    std::cout << "Order History:" << std::endl;
    for (unsigned int i = 0; i < ordersCount; ++i) {
        std::cout << "Order ID: " << orders[i]->getId()
            << " | Total: " << orders[i]->getTotalPrice()
            << " | Status: ";
        switch (orders[i]->getStatus()) {
        case OrderStatus::PENDING:
            std::cout << "Pending";
            break;
        case OrderStatus::SHIPPED:
            std::cout << "Shipped";
            break;
        case OrderStatus::DELIVERED:
            std::cout << "Delivered";
            break;
        case OrderStatus::REJECTED: 
            std::cout << "Rejected";
            //std::cout << " (Reason: " << orders[i]->getRejectionReason().getData() << ")";
     
            break;
        }
        std::cout << std::endl;
    }
}

void Client::saveToFile(std::ofstream& out) const {
    
    out << name.getData() << std::endl;
    out << egn.getData() << std::endl;
    out << password.getData() << std::endl;

    wallet.saveToFile(out);  

     // Save orders
    out << ordersCount << '\n';
    for (unsigned i = 0; i < ordersCount; ++i) {
        orders[i]->saveToFile(out);
    }
    out << cartSize << '\n';
    for (unsigned i = 0; i < cartSize; ++i) {
        out << cart[i].product->getId() << '\n';
        out << cart[i].quantity << '\n';
    }
}

void Client::loadFromFile(std::ifstream& in) {
    
    wallet.loadFromFile(in);  

    unsigned int ordersCount;
    in >> ordersCount;
    in.ignore();

    for (unsigned i = 0; i < ordersCount; ++i) {
        Order* order = new Order();
        order->loadFromFile(in); 
        addOrder(order);
    }
    
    // Load cart data into temporary storage
    in >> savedCartSize;
    in.ignore();

    if (savedCartSize > 0) {
        savedCart = new SavedCartItem[savedCartSize];
        for (unsigned i = 0; i < savedCartSize; ++i) {
            in >> savedCart[i].productId;
            in.ignore();
            in >> savedCart[i].quantity;
            in.ignore();
        }
    }
    else {
        savedCart = nullptr;
    }
}
 


void Client::resolveCartItems() {
    if (!savedCart) return;
    cartSize = 0;

    ProductManager& pm = ProductManager::getInstance();
    unsigned int productCount = pm.getProductsCount();
    Product** allProducts = pm.getAllProducts();

    for (unsigned i = 0; i < savedCartSize; ++i) {
        int productId = savedCart[i].productId;
        unsigned int quantity = savedCart[i].quantity;

        Product* product = nullptr;
        for (unsigned j = 0; j < productCount; ++j) {
            if (allProducts[j]->getId() == productId) {
                product = allProducts[j];
                break;
            }
        }
        if (product) {
            addToCart(product, quantity);
        }
        else {
            std::cerr << "Warning: Product ID " << productId
                << " not found. Skipping cart item." << std::endl;
        }
    }
    delete[] savedCart;
    savedCart = nullptr;
    savedCartSize = 0;
}


void Client::addToCart(Product* product, unsigned int quantity) {
    if (cartSize == cartCapacity) {
        cartCapacity *= 2;
        CartItem* newCart = new CartItem[cartCapacity];
        for (unsigned i = 0; i < cartSize; ++i) {
            newCart[i] = cart[i];
        }
        delete[] cart;
        cart = newCart;
    }

    cart[cartSize].product = product;
    cart[cartSize].quantity = quantity;
    cartSize++;
}

void Client::saveCartToFile(std::ofstream& out) const {
    out << cartSize << '\n';
    for (unsigned i = 0; i < cartSize; ++i) {
        out << cart[i].product->getId() << '\n';
        out << cart[i].quantity << '\n';
    }
}

void Client::loadCartFromFile(std::ifstream& in) {
    unsigned int savedCartSize;
    in >> savedCartSize;
    in.ignore(); 

    clearCart();

    ProductManager& pm = ProductManager::getInstance();
    Product** allProducts = pm.getAllProducts();
    unsigned int productCount = pm.getProductsCount();

    for (unsigned i = 0; i < savedCartSize; ++i) {
        int productId;
        unsigned int quantity;

        in >> productId;
        in.ignore(); 
        in >> quantity;
        in.ignore(); 

        Product* product = nullptr;
        for (unsigned j = 0; j < productCount; ++j) {
            if (allProducts[j]->getId() == productId) {
                product = allProducts[j];
                break;
            }
        }

        if (product) {
            addToCart(product, quantity);
        }
        else {
            std::cerr << "Warning: Product ID " << productId
                << " not found. Skipping cart item." << std::endl;
        }
    }
}

void Client::clearCart() {
    cartSize = 0;
}

void Client::removeFromCart(unsigned int index) {
    if (index >= cartSize) {
        std::cerr << "Invalid item index!" << std::endl;
        return;
    }

    // Shift items to fill the gap
    for (unsigned int i = index; i < cartSize - 1; ++i) {
        cart[i] = cart[i + 1];
    }

    cartSize--;
    std::cout << "Item removed from cart." << std::endl;
}


double Client::calculateTotal() const {
    double total = 0.0;
    for (unsigned i = 0; i < cartSize; ++i) {
        total += cart[i].product->getPrice() * cart[i].quantity;
    }
    return total;
}

void Client::checkout() {
    if (cartSize == 0) {
        std::cout << "\nCart is empty!\n";
        return;
    }

    ProductManager& pm = ProductManager::getInstance();
    for (unsigned i = 0; i < cartSize; ++i) {
        if (cart[i].product->getQuantity() < cart[i].quantity) {
            std::cout << "\nNot enough available '"
                << cart[i].product->getName().getData() << "'\n";
            return;
        }
    }

    double total = calculateTotal();

    //Discount prompt**
    char useDisc = '\0';
    std::cout << "\nYour total is " << total << " BGN."
        << " You have " << wallet.getPoints()
        << " points (" << wallet.getPoints() / 100.0
        << " BGN).\n"
        << "Apply discount? (y/n): ";
    std::cin >> useDisc;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (useDisc == 'y' || useDisc == 'Y') {
        double discount = applyDiscount();
        total = std::max(0.0, total - discount);
        std::cout << "Total after discount: "
            << total << " BGN.\n";
    }

    if (wallet.getBalance() < total) {
        std::cout << "\nNot enough available! You have: "
            << wallet.getBalance() << " lv.\n";
        return;
    }

    Product** orderedProducts = new Product * [cartSize];
    unsigned int* quantities = new unsigned int[cartSize];

    for (unsigned i = 0; i < cartSize; ++i) {
        orderedProducts[i] = cart[i].product;
        quantities[i] = cart[i].quantity;
    }

    // Order
    Order* newOrder = new Order(
        generateOrderId(),  
        orderedProducts,
        quantities,
        cartSize,
        total,
        this
    );

    // Inventory
    for (unsigned i = 0; i < cartSize; ++i) {
        orderedProducts[i]->setQuantity(
            orderedProducts[i]->getQuantity() - quantities[i]
        );
    }
    pm.saveToFile("Inventory.txt");

    wallet.deductFunds(total);
    wallet.addPoints(static_cast<unsigned>(total * 0.05)); 

    // History
    if (ordersCount >= ordersCapacity) {
        ordersCapacity *= 2;
        Order** newOrders = new Order * [ordersCapacity];
        for (unsigned i = 0; i < ordersCount; ++i) {
            newOrders[i] = orders[i];
        }
        delete[] orders;
        orders = newOrders;
    }
    orders[ordersCount++] = newOrder;

    cartSize = 0;
    std::cout << "\nOrder successful! You have: "
        << wallet.getBalance() << "lv.\n";

    delete[] orderedProducts;
    delete[] quantities;

    lastOrder = newOrder;
    std::cout << "Last order saved with total: " << lastOrder->getTotalPrice() << "\n";


}

void Client::confirmOrder(unsigned int orderId) {
    for (unsigned int i = 0; i < ordersCount; ++i) {
        if (orders[i]->getId() == orderId) {
            if (orders[i]->getStatus() == OrderStatus::SHIPPED) {

                unsigned int points = static_cast<unsigned int>(
                    orders[i]->getTotalPrice() * 5
                    );

                // Update status and points
                orders[i]->setStatus(OrderStatus::DELIVERED);
                wallet.addPoints(points);

                std::cout << "Order confirmed! You earned "
                    << points << " points.\n";
            }
            else if (orders[i]->getStatus() == OrderStatus::DELIVERED) {
                std::cout << "Order already confirmed.\n";
            }
            else {
                std::cout << "Order not shipped yet.\n";
            }

            return;
        }
    }
    std::cout << "Order ID not found.\n";
}

void Client::viewCart() const {
    if (cartSize == 0) {
        std::cout << "\nCart is empty!\n";
        return;
    }

    double total = 0.0;
    std::cout << "\n=== Cart ===";
    for (unsigned i = 0; i < cartSize; ++i) {
        std::cout << "\n" << i + 1 << ". " << cart[i].product->getName().getData()
            << "\n   Quantity: " << cart[i].quantity
            << "\n   Price for one: " << cart[i].product->getPrice()
            << "\n   Amount: " << (cart[i].product->getPrice() * cart[i].quantity);
        total += cart[i].product->getPrice() * cart[i].quantity;
    }
    std::cout << "\n-----------------------------"
        << "\nTotal amount: " << total
        << "\n=============================\n";
}


double Client::applyDiscount() {
    unsigned int pts = wallet.getPoints();
    if (pts == 0) {
        std::cout << "You have no points to apply.\n";
        return 0.0;
    }
    double discount = static_cast<double>(pts) / 100.0;
    wallet.deductPoints(pts);
    pointsUsedForDiscount = pts;
    std::cout << "Applied discount of "
        << discount << " BGN using "
        << pts << " points.\n";
    return discount;
}




unsigned int Client::generateOrderId() {
    lastOrderId++; 
    return lastOrderId;
}


Order* Client::getLastOrder() const {
    return lastOrder;
}

double Client::getTotalSpent() const {
    double total = 0.0;
    for (unsigned i = 0; i < ordersCount; ++i) {
        total += orders[i]->getTotalPrice();
    }
    return total;
}

bool Client::requestReturn(unsigned int orderId) {
    for (unsigned i = 0; i < ordersCount; ++i) {
        Order* o = orders[i];
        if (o->getId() == orderId) {
            switch (o->getStatus()) {
            case OrderStatus::SHIPPED:
            case OrderStatus::DELIVERED:
                o->setStatus(OrderStatus::RETURN_REQUESTED);
                return true;
            default:
                std::cout << "Only shipped or delivered orders can be returned.\n";
                return false;
            }
        }
    }
    std::cout << "Order ID not found!\n";
    return false;
}

void Client::refund(double amount) {
    wallet.addFunds(amount);
}

double Client::removeDiscount() {
    if (pointsUsedForDiscount == 0) {
        std::cout << "No applied discount to remove.\n";
        return 0.0;
    }
    double discountBGN = static_cast<double>(pointsUsedForDiscount) / 100.0;
    wallet.addPoints(pointsUsedForDiscount);
    std::cout << "Removed discount of "
        << discountBGN << " BGN; "
        << pointsUsedForDiscount << " points returned.\n";
    pointsUsedForDiscount = 0;
    return discountBGN;
}

void Client::viewAllProductsSorted() const {
    ProductManager& pm = ProductManager::getInstance();
    Product** products = pm.getAllProducts();
    unsigned int count = pm.getProductsCount();

    // copy pointers into a dynamic C?array
    Product** list = new Product * [count];
    for (unsigned int i = 0; i < count; ++i) {
        list[i] = products[i];
    }

    std::cout << "Filter by:\n"
        << "  1) Price (low to high)\n"
        << "  2) Alphabetically (A to Z)\n"
        << "  3) Rating (high to low)\n";

    int choice = 0;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    //selection?sort on the list array:
    for (unsigned int i = 0; i + 1 < count; ++i) {
        unsigned int best = i;
        for (unsigned int j = i + 1; j < count; ++j) {
            bool shouldSwap = false;
            switch (choice) {
            case 1:
                if (list[j]->getPrice() < list[best]->getPrice())
                    shouldSwap = true;
                break;
            case 2:
                if (std::strcmp(
                    list[j]->getName().getData(),
                    list[best]->getName().getData()
                ) < 0)
                    shouldSwap = true;
                break;
            case 3:
                if (list[j]->getRating() > list[best]->getRating())
                    shouldSwap = true;
                break;
            default:
                best = i; j = count; 
                i = count;         
                break;
            }
            if (shouldSwap) {
                best = j;
            }
        }
        if (best != i) {
            Product* tmp = list[i];
            list[i] = list[best];
            list[best] = tmp;
        }
    }

    std::cout << "\nProducts (" << count << "):\n";
    for (unsigned int i = 0; i < count; ++i) {
        Product* p = list[i];
        std::cout << "ID: " << p->getId()
            << " | Name: " << p->getName().getData()
            << " | Price: " << p->getPrice()
            << " | Quantity: " << p->getQuantity()
            << " | Rating: " << p->getRating()
            << "\n";
    }

    delete[] list;
}

void Client::viewProductDetails() const {
    ProductManager& pm = ProductManager::getInstance();
    Product** products = pm.getAllProducts();
    unsigned int count = pm.getProductsCount();

    if (count == 0) {
        std::cout << "No products available.\n";
        return;
    }

    std::cout << "\n=== Products ===\n";
    for (unsigned i = 0; i < count; ++i) {
        std::cout << "  [" << i << "] "
            << products[i]->getName().getData()
            << " (ID: " << products[i]->getId() << ")\n";
    }

    std::cout << "Enter product index to view details: ";
    int choice = -1;
    if (!(std::cin >> choice) ||
        choice < 0 ||
        static_cast<unsigned>(choice) >= count)
    {
        // clear error flag and discard rest of the line
        std::cin.clear();
        std::string discard;
        std::getline(std::cin, discard);
        std::cout << "Invalid selection.\n";
        return;
    }
    // discard the trailing newline before next input
    std::string discard;
    std::getline(std::cin, discard);

    Product* p = products[choice];
    std::cout << "\n=== Product Details ===\n"
        << "ID:          " << p->getId() << "\n"
        << "Name:        " << p->getName().getData() << "\n"
        << "Price:       " << p->getPrice() << " BGN\n"
        << "Quantity:    " << p->getQuantity() << "\n"
        << "Description: " << p->getDescription().getData() << "\n"
        << "Rating:      " << p->getRating() << "\n"
        << std::endl;
}