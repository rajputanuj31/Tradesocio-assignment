#include "../src/OrderBook.h"
#include <cassert>
#include <iostream>

void test_add_and_best() {
    OrderBook book;

    Order buy;
    buy.orderId = 1; buy.symbol = "AAPL"; buy.side = Side::BUY;
    buy.price = 100; buy.quantity = 50; buy.timestamp = Order::nextTimestamp();
    book.addOrder(buy);

    Order sell;
    sell.orderId = 2; sell.symbol = "AAPL"; sell.side = Side::SELL;
    sell.price = 105; sell.quantity = 30; sell.timestamp = Order::nextTimestamp();
    book.addOrder(sell);

    assert(book.hasBuys());
    assert(book.hasSells());

    auto bestBuy = book.getBestBuy();
    assert(bestBuy.has_value());
    assert(bestBuy->get().orderId == 1);
    assert(bestBuy->get().price == 100);

    auto bestSell = book.getBestSell();
    assert(bestSell.has_value());
    assert(bestSell->get().orderId == 2);
    assert(bestSell->get().price == 105);

    std::cout << "  PASS: test_add_and_best" << std::endl;
}

void test_buy_price_priority() {
    OrderBook book;

    Order b1; b1.orderId = 1; b1.symbol = "X"; b1.side = Side::BUY;
    b1.price = 100; b1.quantity = 10; b1.timestamp = Order::nextTimestamp();
    book.addOrder(b1);

    Order b2; b2.orderId = 2; b2.symbol = "X"; b2.side = Side::BUY;
    b2.price = 105; b2.quantity = 10; b2.timestamp = Order::nextTimestamp();
    book.addOrder(b2);

    // Best buy should be highest price
    auto best = book.getBestBuy();
    assert(best->get().orderId == 2);
    assert(best->get().price == 105);

    std::cout << "  PASS: test_buy_price_priority" << std::endl;
}

void test_sell_price_priority() {
    OrderBook book;

    Order s1; s1.orderId = 1; s1.symbol = "X"; s1.side = Side::SELL;
    s1.price = 110; s1.quantity = 10; s1.timestamp = Order::nextTimestamp();
    book.addOrder(s1);

    Order s2; s2.orderId = 2; s2.symbol = "X"; s2.side = Side::SELL;
    s2.price = 105; s2.quantity = 10; s2.timestamp = Order::nextTimestamp();
    book.addOrder(s2);

    // Best sell should be lowest price
    auto best = book.getBestSell();
    assert(best->get().orderId == 2);
    assert(best->get().price == 105);

    std::cout << "  PASS: test_sell_price_priority" << std::endl;
}

void test_time_priority() {
    OrderBook book;

    Order s1; s1.orderId = 1; s1.symbol = "X"; s1.side = Side::SELL;
    s1.price = 100; s1.quantity = 10; s1.timestamp = Order::nextTimestamp();
    book.addOrder(s1);

    Order s2; s2.orderId = 2; s2.symbol = "X"; s2.side = Side::SELL;
    s2.price = 100; s2.quantity = 20; s2.timestamp = Order::nextTimestamp();
    book.addOrder(s2);

    // Same price -> earlier order (id=1) is first
    auto best = book.getBestSell();
    assert(best->get().orderId == 1);

    book.removeBestSell();

    // Now id=2 should be best
    best = book.getBestSell();
    assert(best->get().orderId == 2);

    std::cout << "  PASS: test_time_priority" << std::endl;
}

void test_cancel_order() {
    OrderBook book;

    Order o; o.orderId = 1; o.symbol = "X"; o.side = Side::BUY;
    o.price = 100; o.quantity = 10; o.timestamp = Order::nextTimestamp();
    book.addOrder(o);

    assert(book.hasBuys());
    assert(book.cancelOrder(1));
    assert(!book.hasBuys());

    // Cancel non-existent order
    assert(!book.cancelOrder(999));

    std::cout << "  PASS: test_cancel_order" << std::endl;
}

void test_cancel_middle_order() {
    OrderBook book;

    Order o1; o1.orderId = 1; o1.symbol = "X"; o1.side = Side::BUY;
    o1.price = 100; o1.quantity = 10; o1.timestamp = Order::nextTimestamp();
    book.addOrder(o1);

    Order o2; o2.orderId = 2; o2.symbol = "X"; o2.side = Side::BUY;
    o2.price = 100; o2.quantity = 20; o2.timestamp = Order::nextTimestamp();
    book.addOrder(o2);

    Order o3; o3.orderId = 3; o3.symbol = "X"; o3.side = Side::BUY;
    o3.price = 100; o3.quantity = 30; o3.timestamp = Order::nextTimestamp();
    book.addOrder(o3);

    // Cancel the middle one
    assert(book.cancelOrder(2));
    assert(book.hasBuys());

    auto best = book.getBestBuy();
    assert(best->get().orderId == 1);

    book.removeBestBuy();
    best = book.getBestBuy();
    assert(best->get().orderId == 3);

    std::cout << "  PASS: test_cancel_middle_order" << std::endl;
}

void test_modify_order() {
    OrderBook book;

    Order o; o.orderId = 1; o.symbol = "X"; o.side = Side::SELL;
    o.price = 110; o.quantity = 50; o.timestamp = Order::nextTimestamp();
    book.addOrder(o);

    assert(book.modifyOrder(1, 105, 30));
    auto best = book.getBestSell();
    assert(best->get().orderId == 1);
    assert(best->get().price == 105);
    assert(best->get().quantity == 30);

    // Modify non-existent
    assert(!book.modifyOrder(999, 100, 10));

    std::cout << "  PASS: test_modify_order" << std::endl;
}

void test_empty_book() {
    OrderBook book;

    assert(!book.hasBuys());
    assert(!book.hasSells());
    assert(!book.getBestBuy().has_value());
    assert(!book.getBestSell().has_value());

    std::cout << "  PASS: test_empty_book" << std::endl;
}

int main() {
    std::cout << "[OrderBook Tests]" << std::endl;
    test_add_and_best();
    test_buy_price_priority();
    test_sell_price_priority();
    test_time_priority();
    test_cancel_order();
    test_cancel_middle_order();
    test_modify_order();
    test_empty_book();
    std::cout << "All OrderBook tests passed!" << std::endl;
    return 0;
}
