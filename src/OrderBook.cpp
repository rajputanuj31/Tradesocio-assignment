#include "OrderBook.h"
#include <iostream>
#include <iomanip>

void OrderBook::addOrder(Order order) {
    int price = order.price;
    Side side = order.side;
    int id = order.orderId;

    if (side == Side::BUY) {
        buyLevels_[price].push_back(std::move(order));
        auto it = std::prev(buyLevels_[price].end());
        orderIndex_[id] = {side, price, it};
    } else {
        sellLevels_[price].push_back(std::move(order));
        auto it = std::prev(sellLevels_[price].end());
        orderIndex_[id] = {side, price, it};
    }
}

bool OrderBook::cancelOrder(int orderId) {
    auto indexIt = orderIndex_.find(orderId);
    if (indexIt == orderIndex_.end()) return false;

    removeOrder(orderId);
    return true;
}

bool OrderBook::modifyOrder(int orderId, int newPrice, int newQty) {
    auto indexIt = orderIndex_.find(orderId);
    if (indexIt == orderIndex_.end()) return false;

    const auto& loc = indexIt->second;
    std::string symbol = loc.it->symbol;
    Side side = loc.side;

    removeOrder(orderId);

    Order newOrder;
    newOrder.orderId = orderId;
    newOrder.symbol = symbol;
    newOrder.side = side;
    newOrder.price = newPrice;
    newOrder.quantity = newQty;
    newOrder.timestamp = Order::nextTimestamp();

    addOrder(std::move(newOrder));
    return true;
}

std::optional<std::reference_wrapper<Order>> OrderBook::getBestBuy() {
    if (buyLevels_.empty()) return std::nullopt;
    auto& topLevel = buyLevels_.begin()->second;
    if (topLevel.empty()) return std::nullopt;
    return std::ref(topLevel.front());
}

std::optional<std::reference_wrapper<Order>> OrderBook::getBestSell() {
    if (sellLevels_.empty()) return std::nullopt;
    auto& topLevel = sellLevels_.begin()->second;
    if (topLevel.empty()) return std::nullopt;
    return std::ref(topLevel.front());
}

void OrderBook::removeBestBuy() {
    if (buyLevels_.empty()) return;
    auto levelIt = buyLevels_.begin();
    auto& orderList = levelIt->second;
    if (orderList.empty()) return;

    int id = orderList.front().orderId;
    orderIndex_.erase(id);
    orderList.pop_front();

    if (orderList.empty()) {
        buyLevels_.erase(levelIt);
    }
}

void OrderBook::removeBestSell() {
    if (sellLevels_.empty()) return;
    auto levelIt = sellLevels_.begin();
    auto& orderList = levelIt->second;
    if (orderList.empty()) return;

    int id = orderList.front().orderId;
    orderIndex_.erase(id);
    orderList.pop_front();

    if (orderList.empty()) {
        sellLevels_.erase(levelIt);
    }
}

void OrderBook::removeOrder(int orderId) {
    auto indexIt = orderIndex_.find(orderId);
    if (indexIt == orderIndex_.end()) return;

    const auto& loc = indexIt->second;

    if (loc.side == Side::BUY) {
        buyLevels_[loc.price].erase(loc.it);
        cleanEmptyLevel(Side::BUY, loc.price);
    } else {
        sellLevels_[loc.price].erase(loc.it);
        cleanEmptyLevel(Side::SELL, loc.price);
    }

    orderIndex_.erase(indexIt);
}

void OrderBook::cleanEmptyLevel(Side side, int price) {
    if (side == Side::BUY) {
        auto it = buyLevels_.find(price);
        if (it != buyLevels_.end() && it->second.empty()) {
            buyLevels_.erase(it);
        }
    } else {
        auto it = sellLevels_.find(price);
        if (it != sellLevels_.end() && it->second.empty()) {
            sellLevels_.erase(it);
        }
    }
}

void OrderBook::print() const {
    std::cout << "=== ORDER BOOK ===" << std::endl;

    std::cout << "--- SELL SIDE (ascending) ---" << std::endl;
    for (auto it = sellLevels_.rbegin(); it != sellLevels_.rend(); ++it) {
        int totalQty = 0;
        for (const auto& o : it->second) totalQty += o.quantity;
        std::cout << "  Price=" << it->first << "  TotalQty=" << totalQty
                  << "  Orders=" << it->second.size() << std::endl;
    }

    std::cout << "--- BUY SIDE (descending) ---" << std::endl;
    for (const auto& [price, orders] : buyLevels_) {
        int totalQty = 0;
        for (const auto& o : orders) totalQty += o.quantity;
        std::cout << "  Price=" << price << "  TotalQty=" << totalQty
                  << "  Orders=" << orders.size() << std::endl;
    }

    std::cout << "==================" << std::endl;
}
