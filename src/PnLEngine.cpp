#include "PnLEngine.h"
#include <iostream>
#include <cmath>
#include <algorithm>

int64_t PnLEngine::Position::avgPrice() const {
    if (netQty == 0) return 0;
    return totalCost / std::abs(netQty);
}

int64_t PnLEngine::Position::unrealizedPnL() const {
    if (netQty == 0) return 0;
    int64_t avg = avgPrice();
    return (static_cast<int64_t>(currentPrice) - avg) * netQty;
}

void PnLEngine::onFill(const std::string& symbol, Side side, int qty, int price) {
    auto& pos = positions_[symbol];

    if (side == Side::BUY) {
        if (pos.netQty >= 0) {
            // Adding to long or opening long
            pos.totalCost += static_cast<int64_t>(price) * qty;
            pos.netQty += qty;
        } else {
            // Closing short position (partially or fully)
            int closeQty = std::min(qty, std::abs(pos.netQty));
            int64_t avg = pos.avgPrice();
            pos.realizedPnL += (avg - static_cast<int64_t>(price)) * closeQty;

            pos.netQty += closeQty;
            pos.totalCost = avg * std::abs(pos.netQty);

            int remaining = qty - closeQty;
            if (remaining > 0) {
                // Flipping from short to long
                pos.totalCost = static_cast<int64_t>(price) * remaining;
                pos.netQty += remaining;
            }
        }
    } else {
        if (pos.netQty <= 0) {
            // Adding to short or opening short
            pos.totalCost += static_cast<int64_t>(price) * qty;
            pos.netQty -= qty;
        } else {
            // Closing long position (partially or fully)
            int closeQty = std::min(qty, pos.netQty);
            int64_t avg = pos.avgPrice();
            pos.realizedPnL += (static_cast<int64_t>(price) - avg) * closeQty;

            pos.netQty -= closeQty;
            pos.totalCost = avg * std::abs(pos.netQty);

            int remaining = qty - closeQty;
            if (remaining > 0) {
                // Flipping from long to short
                pos.totalCost = static_cast<int64_t>(price) * remaining;
                pos.netQty -= remaining;
            }
        }
    }
}

void PnLEngine::updatePrice(const std::string& symbol, int newPrice) {
    positions_[symbol].currentPrice = newPrice;
}

void PnLEngine::print() const {
    std::cout << "=== POSITIONS & PnL ===" << std::endl;
    for (const auto& [symbol, pos] : positions_) {
        std::cout << "  Symbol=" << symbol
                  << "  NetQty=" << pos.netQty
                  << "  AvgPrice=" << pos.avgPrice()
                  << "  RealizedPnL=" << pos.realizedPnL
                  << "  UnrealizedPnL=" << pos.unrealizedPnL()
                  << "  MarketPrice=" << pos.currentPrice
                  << std::endl;
    }
    std::cout << "=======================" << std::endl;
}

const PnLEngine::Position* PnLEngine::getPosition(const std::string& symbol) const {
    auto it = positions_.find(symbol);
    if (it == positions_.end()) return nullptr;
    return &it->second;
}
