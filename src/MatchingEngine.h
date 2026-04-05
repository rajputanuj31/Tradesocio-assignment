#pragma once

#include "Types.h"
#include "OrderBook.h"
#include <vector>
#include <functional>

class MatchingEngine {
public:
    using TradeCallback = std::function<void(const Trade&)>;

    void setTradeCallback(TradeCallback cb) { tradeCallback_ = std::move(cb); }

    void newOrder(int orderId, const std::string& symbol, Side side,
                  int price, int quantity);
    bool cancelOrder(int orderId);
    bool modifyOrder(int orderId, int newPrice, int newQty);
    void printBook() const;

    const std::vector<Trade>& getTrades() const { return trades_; }
    const OrderBook& getBook() const { return book_; }

private:
    OrderBook book_;
    std::vector<Trade> trades_;
    TradeCallback tradeCallback_;

    void tryMatch();
};
