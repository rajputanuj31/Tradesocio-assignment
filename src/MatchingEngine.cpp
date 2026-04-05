#include "MatchingEngine.h"
#include <algorithm>
#include <iostream>

void MatchingEngine::newOrder(int orderId, const std::string& symbol,
                               Side side, int price, int quantity) {
    Order order;
    order.orderId = orderId;
    order.symbol = symbol;
    order.side = side;
    order.price = price;
    order.quantity = quantity;
    order.timestamp = Order::nextTimestamp();

    book_.addOrder(std::move(order));
    tryMatch();
}

bool MatchingEngine::cancelOrder(int orderId) {
    return book_.cancelOrder(orderId);
}

bool MatchingEngine::modifyOrder(int orderId, int newPrice, int newQty) {
    bool ok = book_.modifyOrder(orderId, newPrice, newQty);
    if (ok) tryMatch();
    return ok;
}

void MatchingEngine::printBook() const {
    book_.print();
}

void MatchingEngine::tryMatch() {
    while (book_.hasBuys() && book_.hasSells()) {
        auto bestBuyOpt = book_.getBestBuy();
        auto bestSellOpt = book_.getBestSell();
        if (!bestBuyOpt || !bestSellOpt) break;

        Order& bestBuy = bestBuyOpt->get();
        Order& bestSell = bestSellOpt->get();

        if (bestBuy.price < bestSell.price) break;

        int tradePrice = (bestBuy.timestamp < bestSell.timestamp)
                             ? bestBuy.price
                             : bestSell.price;

        int tradeQty = std::min(bestBuy.quantity, bestSell.quantity);

        Trade trade;
        trade.buyOrderId = bestBuy.orderId;
        trade.sellOrderId = bestSell.orderId;
        trade.symbol = bestBuy.symbol;
        trade.price = tradePrice;
        trade.quantity = tradeQty;

        std::cout << trade << std::endl;
        trades_.push_back(trade);

        if (tradeCallback_) {
            tradeCallback_(trade);
        }

        bestBuy.quantity -= tradeQty;
        bestSell.quantity -= tradeQty;

        if (bestBuy.quantity == 0) book_.removeBestBuy();
        if (bestSell.quantity == 0) book_.removeBestSell();
    }
}
