cmake_minimum_required(VERSION 3.14)
project(TradingSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Core library (shared between main and tests) ---
add_library(trading_core
    src/OrderBook.cpp
    src/MatchingEngine.cpp
    src/PnLEngine.cpp
)
target_include_directories(trading_core PUBLIC src)

# --- Main executable ---
add_executable(trading_system src/main.cpp)
target_link_libraries(trading_system PRIVATE trading_core)

# --- Tests ---
enable_testing()

add_executable(test_order_book tests/test_order_book.cpp)
target_link_libraries(test_order_book PRIVATE trading_core)
add_test(NAME OrderBookTests COMMAND test_order_book)

add_executable(test_matching tests/test_matching.cpp)
target_link_libraries(test_matching PRIVATE trading_core)
add_test(NAME MatchingTests COMMAND test_matching)

add_executable(test_pnl tests/test_pnl.cpp)
target_link_libraries(test_pnl PRIVATE trading_core)
add_test(NAME PnLTests COMMAND test_pnl)
