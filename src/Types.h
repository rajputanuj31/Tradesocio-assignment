#pragma once

#include <string>
#include <cstdint>
#include <iostream>
#include <atomic>

enum class Side { BUY, SELL };

inline std::string sideToString(Side s) {
    return s == Side::BUY ? "BUY" : "SELL";
}

inline Side stringToSide(const std::string& s) {
    return s == "BUY" ? Side::BUY : Side::SELL;
}

struct Order {
    int orderId;
    std::string symbol;
    Side side;
    int price;
    int quantity;
    int64_t timestamp;

    static int64_t nextTimestamp() {
        static std::atomic<int64_t> counter{0};
        return counter++;
    }
};

struct Trade {
    int buyOrderId;
    int sellOrderId;
    std::string symbol;
    int price;
    int quantity;
};

inline std::ostream& operator<<(std::ostream& os, const Trade& t) {
    os << "TRADE " << t.symbol
       << " BuyOrder=" << t.buyOrderId
       << " SellOrder=" << t.sellOrderId
       << " Price=" << t.price
       << " Qty=" << t.quantity;
    return os;
}
