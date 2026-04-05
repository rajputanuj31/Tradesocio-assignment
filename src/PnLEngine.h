#pragma once

#include "Types.h"
#include <string>
#include <unordered_map>

class PnLEngine {
public:
    void onFill(const std::string& symbol, Side side, int qty, int price);
    void updatePrice(const std::string& symbol, int newPrice);
    void print() const;

    struct Position {
        int netQty = 0;
        int64_t totalCost = 0;
        int64_t realizedPnL = 0;
        int currentPrice = 0;

        int64_t avgPrice() const;
        int64_t unrealizedPnL() const;
    };

    const Position* getPosition(const std::string& symbol) const;

private:
    std::unordered_map<std::string, Position> positions_;
};
