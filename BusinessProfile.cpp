#include "BusinessProfile.h"
#include "ProductManager.h"
#include "Product.h"
#include "Client.h"
#include "Wallet.h"
#include <iostream>
#include <fstream>

BusinessProfile::BusinessProfile(const CharArray& name, const CharArray& egn, const CharArray& password)
    : User(name, egn, password), productIds(nullptr), productsCount(0), productsCapacity(2),
    pendingOrders(nullptr), pendingOrdersCount(0), pendingOrdersCapacity(2), totalRevenue(0.0),
    processedOrders(nullptr),processedCount(0), processedCapacity(2), returnReqCount(0),returnReqCap(MAX_RETURN_REQ) {
    productIds = new unsigned int[productsCapacity];
    pendingOrders = new Order * [pendingOrdersCapacity];
    processedOrders = new Order * [processedCapacity];
    returnRequests = new Order * [returnReqCap];
}

BusinessProfile::~BusinessProfile() {
    delete[] productIds; 

    for (unsigned int i = 0; i < pendingOrdersCount; ++i) {
        delete pendingOrders[i];
    }
    delete[] pendingOrders;
    delete[] processedOrders;
    delete[] returnRequests;
}

void BusinessProfile::resizeReturnRequests() {
    returnReqCap *= 2;
    Order** tmp = new Order * [returnReqCap];
    for (unsigned i = 0; i < returnReqCount; ++i)
        tmp[i] = returnRequests[i];
    delete[] returnRequests;
    returnRequests = tmp;
}

void BusinessProfile::resizeProductIds() {
    productsCapacity *= 2;
    unsigned int* newIds = new unsigned int[productsCapacity];
    for (unsigned int i = 0; i < productsCount; ++i) {
        newIds[i] = productIds[i];
    }
    delete[] productIds;
    productIds = newIds;
}

void BusinessProfile::viewProfile() const {
    std::cout << "Business Profile:" << std::endl;
    std::cout << "Name: " << getName().getData() << std::endl;
    std::cout << "EGN: " << getEgn().getData() << std::endl;
    std::cout << "Total Revenue: " << totalRevenue << std::endl;
    std::cout << "Products Count: " << productsCount << std::endl;
}

void BusinessProfile::help() const {
    std::cout << "Available commands for Business Profile:" << std::endl;
    std::cout << "add-item [name] [price] [quantity] [description] - Add new product" << std::endl;
    std::cout << "remove-item [name] - Remove product" << std::endl;
    std::cout << "list-pending-orders - List all pending orders" << std::endl;
    std::cout << "approve-order [index] - Approve order" << std::endl;
    std::cout << "reject-order [index] [description] - Reject order" << std::endl;
    std::cout << "view-revenue - View total revenue" << std::endl;
}

void BusinessProfile::addItem(const CharArray& name, double price, unsigned int quantity, const CharArray& description, double rating) {
    ProductManager& pm = ProductManager::getInstance();
    unsigned int newId = pm.getNextProductId();

    pm.addProduct(new Product(newId, name, price, quantity, description,rating));
    pm.saveToFile("Inventory.txt");

    if (productsCount == productsCapacity) resizeProductIds();
    productIds[productsCount++] = newId;
}

void BusinessProfile::removeItem(const CharArray& name) {
ProductManager& pm = ProductManager::getInstance();
for (unsigned int i = 0; i < productsCount; ++i) {
    Product* p = pm.getProduct(productIds[i]);
    if (p && p->getName() == name) {
        // Remove from ProductManager
        pm.removeProduct(p->getId());

        // Remove ID from local array
        for (unsigned j = i; j < productsCount - 1; j++) {
            productIds[j] = productIds[j + 1];
        }
        productsCount--;
        return;
    }
    }
    std::cout << "Product not found!" << std::endl;
}

void BusinessProfile::addPendingOrder(Order* order) {
    if (pendingOrdersCount == pendingOrdersCapacity) {
        pendingOrdersCapacity *= 2;
        Order** newOrders = new Order * [pendingOrdersCapacity];
        for (unsigned i = 0; i < pendingOrdersCount; ++i) {
            newOrders[i] = pendingOrders[i];
        }
        delete[] pendingOrders;
        pendingOrders = newOrders;
    }

    pendingOrders[pendingOrdersCount++] = order;
    std::cout << "Order added to pending list. Total: " << pendingOrdersCount << "\n";
}


Order** BusinessProfile::getPendingOrders() const {
    return pendingOrders;
}

unsigned int BusinessProfile::getPendingOrdersCount() const {
    return pendingOrdersCount;
}

void BusinessProfile::listPendingOrders() const {
    for (unsigned int i = 0; i < pendingOrdersCount; ++i) {
        Order* order = pendingOrders[i];
        User* client = order->getClient();

        std::cout << "Order #" << i
            << " - Total: " << order->getTotalPrice()
            << " - Client: " << client->getName().getData()
            << " (" << client->getEgn().getData() << ")"
            << std::endl;
    }
    if (pendingOrdersCount == 0) {
        std::cout << "No pending orders found.\n";
    }

}

void BusinessProfile::approveOrder() {
    if (pendingOrdersCount == 0) {
        std::cout << "No pending orders to approve." << std::endl;
        return;
    }

    std::cout << "Pending Orders:\n";
    for (unsigned int i = 0; i < pendingOrdersCount; ++i) {
        std::cout << "  [" << i << "] Total: "
            << pendingOrders[i]->getTotalPrice() << "\n";
    }
    std::cout << "Enter the index of the order to approve: ";
    unsigned int choice;
    if (!(std::cin >> choice) || choice >= pendingOrdersCount) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid selection.\n";
        return;
    }
    Order* approvedOrder = pendingOrders[choice];
    pendingOrders[choice]->setStatus(OrderStatus::SHIPPED);
    std::cout << "Order [" << choice << "] shipped to client.\n";

    // remove from array
    pendingOrders[choice]->setStatus(OrderStatus::SHIPPED);
    totalRevenue += pendingOrders[choice]->getTotalPrice();
    for (unsigned int j = choice; j + 1 < pendingOrdersCount; ++j) {
        pendingOrders[j] = pendingOrders[j + 1];
    }
    --pendingOrdersCount;
    std::cout << "Order [" << choice << "] approved and shipped.\n";

    //Order* approvedOrder = pendingOrders[choice];

    // Update sales count for each product in the order
    for (unsigned i = 0; i < approvedOrder->getProductsCount(); ++i) {
        Product* p = approvedOrder->getProductByIndex(i);
        unsigned int qty = approvedOrder->getQuantityByIndex(i);
        if (p) {
            p->incrementSalesCount(qty);
        }
    }
    if (processedCount == processedCapacity) resizeProcessedOrders();
    processedOrders[processedCount++] = approvedOrder;

    // Remove from pendingOrders
    for (unsigned int j = choice; j + 1 < pendingOrdersCount; ++j) {
        pendingOrders[j] = pendingOrders[j + 1];
    }
    --pendingOrdersCount;
}

void BusinessProfile::rejectOrder() {
    if (pendingOrdersCount == 0) {
        std::cout << "No pending orders to reject.\n";
        return;
    }

    std::cout << "Pending Orders:\n";
    for (unsigned int i = 0; i < pendingOrdersCount; ++i) {
        std::cout << "  [" << i << "] Total: "
            << pendingOrders[i]->getTotalPrice()
            << " | Client: " << pendingOrders[i]->getClient()->getName().getData()
            << "\n";
    }

    std::cout << "Enter the index of the order to reject: ";
    unsigned int choice;
    if (!(std::cin >> choice) || choice >= pendingOrdersCount) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid selection.\n";
        return;
    }

    std::cin.ignore();
    char reasonBuffer[256];
    std::cout << "Enter reason for rejection: ";
    std::cin.getline(reasonBuffer, sizeof(reasonBuffer));
    CharArray reason(reasonBuffer);

    Order* rejectedOrder = pendingOrders[choice];
    rejectedOrder->setStatus(OrderStatus::REJECTED);
    std::cout << "Order [" << choice << "] rejected. Reason: " << reason.getData() << "\n";

    if (processedCount == processedCapacity) resizeProcessedOrders();
    processedOrders[processedCount++] = rejectedOrder;

    for (unsigned int j = choice; j + 1 < pendingOrdersCount; ++j) {
        pendingOrders[j] = pendingOrders[j + 1];
    }
    --pendingOrdersCount;
}

void BusinessProfile::resizeProcessedOrders() {
    processedCapacity *= 2;
    Order** tmp = new Order * [processedCapacity];
    for (unsigned i = 0; i < processedCount; ++i)
        tmp[i] = processedOrders[i];
    delete[] processedOrders;
    processedOrders = tmp;
}

void BusinessProfile::listAllOrders() const {
    std::cout << "\n=== All Orders ===\n";
    if (processedCount == 0 && pendingOrdersCount == 0) {
        std::cout << "No orders at all.\n";
        return;
    }

    for (unsigned i = 0; i < pendingOrdersCount; ++i) {
        auto* o = pendingOrders[i];
        std::cout << "[PENDING]  ID=" << o->getId()
                  << " Total=" << o->getTotalPrice() << "\n";
    }
    
    for (unsigned i = 0; i < processedCount; ++i) {
        auto* o = processedOrders[i];
        std::cout << "[" 
                  << (o->getStatus()==OrderStatus::SHIPPED     ? "APPROVED" :
                      o->getStatus()==OrderStatus::DELIVERED   ? "DELIVERED" :
                      o->getStatus()==OrderStatus::REJECTED    ? "REJECTED" :
                      /*fallback*/                            "OTHER")
                  << "] ID=" << o->getId()
                  << " Total=" << o->getTotalPrice() << "\n";
    }
}


void BusinessProfile::viewRevenue() const {
 
    std::cout << "Total Revenue: " << totalRevenue << std::endl;
}

void BusinessProfile::saveToFile(std::ofstream& out) const {
    
    out << name.getData() << std::endl;
    out << egn.getData() << std::endl;
    out << password.getData() << std::endl;

    out << productsCount << std::endl;
    for (unsigned int i = 0; i < productsCount; ++i) {
        out << productIds[i] << std::endl;  
    }
    // Save pending orders
    out << pendingOrdersCount << std::endl;
    out << totalRevenue << std::endl;
}


void BusinessProfile::loadFromFile(std::ifstream& in) {
    
    unsigned int count;
    in >> count;
    in.ignore(); 

    for (unsigned int i = 0; i < count; ++i) {
        unsigned int id;
        in >> id;
        in.ignore();

        if (productsCount == productsCapacity) resizeProductIds();
        productIds[productsCount++] = id;
    }

    // Load pending orders
    in >> pendingOrdersCount;
    in >> totalRevenue;
    in.ignore();
}

void BusinessProfile::viewAllProducts() const {
    ProductManager& pm = ProductManager::getInstance();

    std::cout << "Product Inventory (owned by this business):" << std::endl;
    for (unsigned i = 0; i < productsCount; ++i) {
        Product* p = pm.getProduct(productIds[i]);
        if (p) {
            std::cout << "ID: " << p->getId()
                << " | Name: " << p->getName().getData()
                << " | Price: " << p->getPrice()
                << " | Stock: " << p->getQuantity()
                << std::endl;
        }
    }
}

void BusinessProfile::viewBestSellingProducts() const {
    ProductManager& pm = ProductManager::getInstance();

    if (productsCount == 0) {
        std::cout << "No products to evaluate.\n";
        return;
    }

    // Temporary array 
    struct ProductSales {
        Product* product;
        unsigned int sales;
    };

    ProductSales* data = new ProductSales[productsCount];
    unsigned int actualCount = 0;

    // Gather sales for your products
    for (unsigned i = 0; i < productsCount; ++i) {
        Product* p = pm.getProduct(productIds[i]);
        if (p) {
            data[actualCount++] = { p, p->getSalesCount() };
        }
    }

    if (actualCount == 0) {
        std::cout << "No valid products found.\n";
        delete[] data;
        return;
    }

    unsigned int topN = actualCount < 3 ? actualCount : 3;
    for (unsigned k = 0; k < topN; ++k) {
        
        unsigned int bestIdx = k;
        for (unsigned j = k + 1; j < actualCount; ++j) {
            if (data[j].sales > data[bestIdx].sales) {
                bestIdx = j;
            }
        }
 
        if (bestIdx != k) {
            std::swap(data[k], data[bestIdx]);
        }
    }

    std::cout << "\n=== Top " << topN << " Selling Products ===\n";
    for (unsigned i = 0; i < topN; ++i) {
        Product* p = data[i].product;
        std::cout << (i + 1) << ". ID: " << p->getId()
            << " | Name: " << p->getName().getData()
            << " | Sold: " << data[i].sales << " units"
            << " | Price: " << p->getPrice()
            << " | In Stock: " << p->getQuantity()
            << "\n";
    }

    delete[] data;
}

void BusinessProfile::addReturnRequest(Order* o) {
    if (returnReqCount == returnReqCap) resizeReturnRequests();
    returnRequests[returnReqCount++] = o;
}

void BusinessProfile::listReturnRequests() const {
    if (returnReqCount == 0) {
        std::cout << "No return requests.\n";
        return;
    }
    for (unsigned i = 0; i < returnReqCount; ++i) {
        auto* o = returnRequests[i];
        std::cout << "[" << i << "]  ID=" << o->getId()
            << "  Total=" << o->getTotalPrice()
            << "  Client=" << o->getClient()->getName().getData()
            << "\n";
    }
}

void BusinessProfile::approveReturn(unsigned idx) {
    Order* o = returnRequests[idx];
    double total = o->getTotalPrice();
    // Refund client
    if (Client* c = dynamic_cast<Client*>(o->getClient())) {
        c->refund(total);
        
    }
    // Restock
    for (unsigned i = 0; i < o->getProductsCount(); ++i) {
        auto* p = o->getProductByIndex(i);
        p->setQuantity(p->getQuantity() + o->getQuantityByIndex(i));
    }
    o->setStatus(OrderStatus::RETURNED);
    // Remove from queue
    for (unsigned j = idx; j + 1 < returnReqCount; ++j)
        returnRequests[j] = returnRequests[j + 1];
    --returnReqCount;
    std::cout << "Return approved; refunded " << total << " to client.\n";
}

void BusinessProfile::rejectReturn(unsigned idx, const CharArray& reason) {
    Order* o = returnRequests[idx];
    o->setStatus(OrderStatus::RETURN_REJECTED);
    // Remove from queue
    for (unsigned j = idx; j + 1 < returnReqCount; ++j)
        returnRequests[j] = returnRequests[j + 1];
    --returnReqCount;
    std::cout << "Return request rejected: " << reason.getData() << "\n";
}
