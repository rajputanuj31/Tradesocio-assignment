#include "MatchingEngine.h"
#include "PnLEngine.h"
#include <iostream>
#include <sstream>
#include <string>

int main() {
    MatchingEngine engine;
    PnLEngine pnl;

    std::cout << "Trading System Ready. Enter commands:" << std::endl;
    std::cout << "  NEW <id> <symbol> <BUY|SELL> <price> <qty>" << std::endl;
    std::cout << "  CANCEL <id>" << std::endl;
    std::cout << "  MODIFY <id> <newPrice> <newQty>" << std::endl;
    std::cout << "  FILL <symbol> <BUY|SELL> <qty> <price>" << std::endl;
    std::cout << "  PRICE <symbol> <marketPrice>" << std::endl;
    std::cout << "  PRINT BOOK | PRINT PNL" << std::endl;
    std::cout << "  QUIT" << std::endl;
    std::cout << std::endl;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command == "NEW") {
            int id, price, qty;
            std::string symbol, sideStr;
            if (!(iss >> id >> symbol >> sideStr >> price >> qty)) {
                std::cerr << "ERROR: Usage: NEW <id> <symbol> <BUY|SELL> <price> <qty>" << std::endl;
                continue;
            }
            engine.newOrder(id, symbol, stringToSide(sideStr), price, qty);

        } else if (command == "CANCEL") {
            int id;
            if (!(iss >> id)) {
                std::cerr << "ERROR: Usage: CANCEL <id>" << std::endl;
                continue;
            }
            if (!engine.cancelOrder(id)) {
                std::cerr << "ERROR: Order " << id << " not found" << std::endl;
            }

        } else if (command == "MODIFY") {
            int id, newPrice, newQty;
            if (!(iss >> id >> newPrice >> newQty)) {
                std::cerr << "ERROR: Usage: MODIFY <id> <newPrice> <newQty>" << std::endl;
                continue;
            }
            if (!engine.modifyOrder(id, newPrice, newQty)) {
                std::cerr << "ERROR: Order " << id << " not found" << std::endl;
            }

        } else if (command == "FILL") {
            std::string symbol, sideStr;
            int qty, price;
            if (!(iss >> symbol >> sideStr >> qty >> price)) {
                std::cerr << "ERROR: Usage: FILL <symbol> <BUY|SELL> <qty> <price>" << std::endl;
                continue;
            }
            pnl.onFill(symbol, stringToSide(sideStr), qty, price);

        } else if (command == "PRICE") {
            std::string symbol;
            int price;
            if (!(iss >> symbol >> price)) {
                std::cerr << "ERROR: Usage: PRICE <symbol> <marketPrice>" << std::endl;
                continue;
            }
            pnl.updatePrice(symbol, price);

        } else if (command == "PRINT") {
            std::string what;
            iss >> what;
            if (what == "BOOK") {
                engine.printBook();
            } else if (what == "PNL") {
                pnl.print();
            } else {
                engine.printBook();
                pnl.print();
            }

        } else if (command == "QUIT" || command == "EXIT") {
            break;

        } else {
            std::cerr << "ERROR: Unknown command: " << command << std::endl;
        }
    }

    return 0;
}
