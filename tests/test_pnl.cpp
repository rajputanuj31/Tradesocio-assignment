#include "../src/PnLEngine.h"
#include <cassert>
#include <iostream>
#include <cmath>

void test_single_buy_position() {
    PnLEngine pnl;

    pnl.onFill("AAPL", Side::BUY, 100, 150);

    auto* pos = pnl.getPosition("AAPL");
    assert(pos != nullptr);
    assert(pos->netQty == 100);
    assert(pos->avgPrice() == 150);
    assert(pos->realizedPnL == 0);

    std::cout << "  PASS: test_single_buy_position" << std::endl;
}

void test_accumulate_long() {
    PnLEngine pnl;

    pnl.onFill("AAPL", Side::BUY, 100, 150);
    pnl.onFill("AAPL", Side::BUY, 100, 160);

    auto* pos = pnl.getPosition("AAPL");
    assert(pos->netQty == 200);
    // avgPrice = (150*100 + 160*100) / 200 = 31000/200 = 155
    assert(pos->avgPrice() == 155);
    assert(pos->realizedPnL == 0);

    std::cout << "  PASS: test_accumulate_long" << std::endl;
}

void test_close_long_profit() {
    PnLEngine pnl;

    pnl.onFill("AAPL", Side::BUY, 100, 150);
    pnl.onFill("AAPL", Side::SELL, 100, 170);

    auto* pos = pnl.getPosition("AAPL");
    assert(pos->netQty == 0);
    // Realized PnL = (170 - 150) * 100 = 2000
    assert(pos->realizedPnL == 2000);

    std::cout << "  PASS: test_close_long_profit" << std::endl;
}

void test_close_long_loss() {
    PnLEngine pnl;

    pnl.onFill("AAPL", Side::BUY, 100, 150);
    pnl.onFill("AAPL", Side::SELL, 100, 130);

    auto* pos = pnl.getPosition("AAPL");
    assert(pos->netQty == 0);
    // Realized PnL = (130 - 150) * 100 = -2000
    assert(pos->realizedPnL == -2000);

    std::cout << "  PASS: test_close_long_loss" << std::endl;
}

void test_partial_close_long() {
    PnLEngine pnl;

    pnl.onFill("AAPL", Side::BUY, 100, 150);
    pnl.onFill("AAPL", Side::SELL, 60, 170);

    auto* pos = pnl.getPosition("AAPL");
    assert(pos->netQty == 40);
    assert(pos->avgPrice() == 150);
    // Realized PnL = (170 - 150) * 60 = 1200
    assert(pos->realizedPnL == 1200);

    std::cout << "  PASS: test_partial_close_long" << std::endl;
}

void test_unrealized_pnl() {
    PnLEngine pnl;

    pnl.onFill("AAPL", Side::BUY, 100, 150);
    pnl.updatePrice("AAPL", 170);

    auto* pos = pnl.getPosition("AAPL");
    // Unrealized = (170 - 150) * 100 = 2000
    assert(pos->unrealizedPnL() == 2000);

    pnl.updatePrice("AAPL", 140);
    // Unrealized = (140 - 150) * 100 = -1000
    assert(pos->unrealizedPnL() == -1000);

    std::cout << "  PASS: test_unrealized_pnl" << std::endl;
}

void test_short_position() {
    PnLEngine pnl;

    pnl.onFill("AAPL", Side::SELL, 100, 150);

    auto* pos = pnl.getPosition("AAPL");
    assert(pos->netQty == -100);
    assert(pos->avgPrice() == 150);

    std::cout << "  PASS: test_short_position" << std::endl;
}

void test_close_short_profit() {
    PnLEngine pnl;

    pnl.onFill("AAPL", Side::SELL, 100, 150);
    pnl.onFill("AAPL", Side::BUY, 100, 130);

    auto* pos = pnl.getPosition("AAPL");
    assert(pos->netQty == 0);
    // Short profit: sold at 150, bought back at 130 => (150-130)*100 = 2000
    assert(pos->realizedPnL == 2000);

    std::cout << "  PASS: test_close_short_profit" << std::endl;
}

void test_close_short_loss() {
    PnLEngine pnl;

    pnl.onFill("AAPL", Side::SELL, 100, 150);
    pnl.onFill("AAPL", Side::BUY, 100, 170);

    auto* pos = pnl.getPosition("AAPL");
    assert(pos->netQty == 0);
    // Short loss: sold at 150, bought at 170 => (150-170)*100 = -2000
    assert(pos->realizedPnL == -2000);

    std::cout << "  PASS: test_close_short_loss" << std::endl;
}

void test_short_unrealized_pnl() {
    PnLEngine pnl;

    pnl.onFill("AAPL", Side::SELL, 100, 150);
    pnl.updatePrice("AAPL", 140);

    auto* pos = pnl.getPosition("AAPL");
    // Short unrealized: (140 - 150) * (-100) = 1000 (profit, price dropped)
    assert(pos->unrealizedPnL() == 1000);

    pnl.updatePrice("AAPL", 160);
    // (160 - 150) * (-100) = -1000 (loss, price went up)
    assert(pos->unrealizedPnL() == -1000);

    std::cout << "  PASS: test_short_unrealized_pnl" << std::endl;
}

void test_flip_long_to_short() {
    PnLEngine pnl;

    pnl.onFill("AAPL", Side::BUY, 50, 100);
    pnl.onFill("AAPL", Side::SELL, 80, 120);

    auto* pos = pnl.getPosition("AAPL");
    // Closed 50 long: realized = (120 - 100) * 50 = 1000
    // Opened 30 short at 120
    assert(pos->netQty == -30);
    assert(pos->realizedPnL == 1000);
    assert(pos->avgPrice() == 120);

    std::cout << "  PASS: test_flip_long_to_short" << std::endl;
}

void test_multiple_symbols() {
    PnLEngine pnl;

    pnl.onFill("AAPL", Side::BUY, 100, 150);
    pnl.onFill("GOOG", Side::BUY, 50, 2800);

    auto* apple = pnl.getPosition("AAPL");
    auto* goog = pnl.getPosition("GOOG");

    assert(apple->netQty == 100);
    assert(apple->avgPrice() == 150);
    assert(goog->netQty == 50);
    assert(goog->avgPrice() == 2800);

    std::cout << "  PASS: test_multiple_symbols" << std::endl;
}

void test_zero_position_after_close() {
    PnLEngine pnl;

    pnl.onFill("AAPL", Side::BUY, 100, 150);
    pnl.onFill("AAPL", Side::SELL, 100, 150);

    auto* pos = pnl.getPosition("AAPL");
    assert(pos->netQty == 0);
    assert(pos->realizedPnL == 0);
    assert(pos->unrealizedPnL() == 0);

    std::cout << "  PASS: test_zero_position_after_close" << std::endl;
}

int main() {
    std::cout << "[PnL Engine Tests]" << std::endl;
    test_single_buy_position();
    test_accumulate_long();
    test_close_long_profit();
    test_close_long_loss();
    test_partial_close_long();
    test_unrealized_pnl();
    test_short_position();
    test_close_short_profit();
    test_close_short_loss();
    test_short_unrealized_pnl();
    test_flip_long_to_short();
    test_multiple_symbols();
    test_zero_position_after_close();
    std::cout << "All PnL tests passed!" << std::endl;
    return 0;
}
