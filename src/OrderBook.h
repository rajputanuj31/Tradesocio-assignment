#pragma once

#include "Types.h"
#include <map>
#include <list>
#include <unordered_map>
#include <functional>
#include <optional>

class OrderBook {
public:
    void addOrder(Order order);
    bool cancelOrder(int orderId);
    bool modifyOrder(int orderId, int newPrice, int newQty);
    void print() const;

    std::optional<std::reference_wrapper<Order>> getBestBuy();
    std::optional<std::reference_wrapper<Order>> getBestSell();

    void removeBestBuy();
    void removeBestSell();

    bool hasBuys() const { return !buyLevels_.empty(); }
    bool hasSells() const { return !sellLevels_.empty(); }

private:
    using OrderList = std::list<Order>;
    using OrderIterator = OrderList::iterator;

    // BUY side: highest price first
    std::map<int, OrderList, std::greater<int>> buyLevels_;
    // SELL side: lowest price first
    std::map<int, OrderList> sellLevels_;

    struct OrderLocation {
        Side side;
        int price;
        OrderIterator it;
    };

    // O(1) lookup by orderId for cancel/modify
    std::unordered_map<int, OrderLocation> orderIndex_;

    void removeOrder(int orderId);
    void cleanEmptyLevel(Side side, int price);
};
