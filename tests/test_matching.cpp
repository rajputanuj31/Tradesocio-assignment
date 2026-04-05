#include "../src/MatchingEngine.h"
#include <cassert>
#include <iostream>

void test_basic_match() {
    MatchingEngine engine;

    engine.newOrder(1, "AAPL", Side::BUY, 100, 50);
    engine.newOrder(2, "AAPL", Side::SELL, 100, 50);

    auto& trades = engine.getTrades();
    assert(trades.size() == 1);
    assert(trades[0].buyOrderId == 1);
    assert(trades[0].sellOrderId == 2);
    assert(trades[0].quantity == 50);
    assert(trades[0].price == 100);

    // Book should be empty
    assert(!engine.getBook().hasBuys());
    assert(!engine.getBook().hasSells());

    std::cout << "  PASS: test_basic_match" << std::endl;
}

void test_no_match() {
    MatchingEngine engine;

    engine.newOrder(1, "AAPL", Side::BUY, 95, 50);
    engine.newOrder(2, "AAPL", Side::SELL, 100, 50);

    assert(engine.getTrades().empty());
    assert(engine.getBook().hasBuys());
    assert(engine.getBook().hasSells());

    std::cout << "  PASS: test_no_match" << std::endl;
}

void test_partial_fill() {
    MatchingEngine engine;

    engine.newOrder(1, "AAPL", Side::BUY, 100, 100);
    engine.newOrder(2, "AAPL", Side::SELL, 100, 60);

    auto& trades = engine.getTrades();
    assert(trades.size() == 1);
    assert(trades[0].quantity == 60);

    // 40 remaining on buy side, sell side empty
    assert(engine.getBook().hasBuys());
    assert(!engine.getBook().hasSells());

    std::cout << "  PASS: test_partial_fill" << std::endl;
}

void test_multiple_partial_fills() {
    MatchingEngine engine;

    engine.newOrder(1, "AAPL", Side::SELL, 100, 30);
    engine.newOrder(2, "AAPL", Side::SELL, 100, 40);
    engine.newOrder(3, "AAPL", Side::BUY, 100, 50);

    auto& trades = engine.getTrades();
    // Should fill 30 from order 1 (time priority), then 20 from order 2
    assert(trades.size() == 2);
    assert(trades[0].sellOrderId == 1);
    assert(trades[0].quantity == 30);
    assert(trades[1].sellOrderId == 2);
    assert(trades[1].quantity == 20);

    // 20 remaining on sell side from order 2
    assert(!engine.getBook().hasBuys());
    assert(engine.getBook().hasSells());

    std::cout << "  PASS: test_multiple_partial_fills" << std::endl;
}

void test_buy_matches_lowest_sell() {
    MatchingEngine engine;

    engine.newOrder(1, "AAPL", Side::SELL, 105, 10);
    engine.newOrder(2, "AAPL", Side::SELL, 100, 10);
    engine.newOrder(3, "AAPL", Side::BUY, 110, 10);

    auto& trades = engine.getTrades();
    // Should match against the lowest sell (price=100) first
    assert(trades.size() == 1);
    assert(trades[0].sellOrderId == 2);
    assert(trades[0].price == 100);

    std::cout << "  PASS: test_buy_matches_lowest_sell" << std::endl;
}

void test_sell_matches_highest_buy() {
    MatchingEngine engine;

    engine.newOrder(1, "AAPL", Side::BUY, 95, 10);
    engine.newOrder(2, "AAPL", Side::BUY, 100, 10);
    engine.newOrder(3, "AAPL", Side::SELL, 90, 10);

    auto& trades = engine.getTrades();
    // Should match against the highest buy (price=100) first
    assert(trades.size() == 1);
    assert(trades[0].buyOrderId == 2);
    assert(trades[0].price == 100);

    std::cout << "  PASS: test_sell_matches_highest_buy" << std::endl;
}

void test_cancel_prevents_match() {
    MatchingEngine engine;

    engine.newOrder(1, "AAPL", Side::BUY, 100, 50);
    engine.cancelOrder(1);
    engine.newOrder(2, "AAPL", Side::SELL, 100, 50);

    assert(engine.getTrades().empty());
    assert(!engine.getBook().hasBuys());
    assert(engine.getBook().hasSells());

    std::cout << "  PASS: test_cancel_prevents_match" << std::endl;
}

void test_modify_triggers_match() {
    MatchingEngine engine;

    engine.newOrder(1, "AAPL", Side::BUY, 95, 50);
    engine.newOrder(2, "AAPL", Side::SELL, 100, 50);

    // No match yet
    assert(engine.getTrades().empty());

    // Modify buy price up to meet sell
    engine.modifyOrder(1, 100, 50);

    assert(engine.getTrades().size() == 1);
    assert(!engine.getBook().hasBuys());
    assert(!engine.getBook().hasSells());

    std::cout << "  PASS: test_modify_triggers_match" << std::endl;
}

void test_price_time_priority() {
    MatchingEngine engine;

    engine.newOrder(1, "AAPL", Side::SELL, 100, 10);
    engine.newOrder(2, "AAPL", Side::SELL, 100, 10);
    engine.newOrder(3, "AAPL", Side::SELL, 100, 10);

    engine.newOrder(4, "AAPL", Side::BUY, 100, 15);

    auto& trades = engine.getTrades();
    // Should fill order 1 (10), then 5 from order 2
    assert(trades.size() == 2);
    assert(trades[0].sellOrderId == 1);
    assert(trades[0].quantity == 10);
    assert(trades[1].sellOrderId == 2);
    assert(trades[1].quantity == 5);

    std::cout << "  PASS: test_price_time_priority" << std::endl;
}

void test_aggressive_sweeps_multiple_levels() {
    MatchingEngine engine;

    engine.newOrder(1, "AAPL", Side::SELL, 100, 10);
    engine.newOrder(2, "AAPL", Side::SELL, 101, 10);
    engine.newOrder(3, "AAPL", Side::SELL, 102, 10);

    engine.newOrder(4, "AAPL", Side::BUY, 102, 25);

    auto& trades = engine.getTrades();
    // Sweeps: 10@100, 10@101, 5@102
    assert(trades.size() == 3);
    assert(trades[0].price == 100);
    assert(trades[0].quantity == 10);
    assert(trades[1].price == 101);
    assert(trades[1].quantity == 10);
    assert(trades[2].price == 102);
    assert(trades[2].quantity == 5);

    std::cout << "  PASS: test_aggressive_sweeps_multiple_levels" << std::endl;
}

int main() {
    std::cout << "[MatchingEngine Tests]" << std::endl;
    test_basic_match();
    test_no_match();
    test_partial_fill();
    test_multiple_partial_fills();
    test_buy_matches_lowest_sell();
    test_sell_matches_highest_buy();
    test_cancel_prevents_match();
    test_modify_triggers_match();
    test_price_time_priority();
    test_aggressive_sweeps_multiple_levels();
    std::cout << "All MatchingEngine tests passed!" << std::endl;
    return 0;
}
