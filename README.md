# Trading System -- Order Matching & PnL Engine

A C++17 in-memory order matching engine and position/PnL tracker.

## Project Structure

```
trading_system/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── main.cpp              Command parser (stdin loop)
│   ├── Types.h               Order, Trade, Side enum
│   ├── OrderBook.h/.cpp      Order book data structure
│   ├── MatchingEngine.h/.cpp Matching logic
│   └── PnLEngine.h/.cpp      Position & PnL tracking
└── tests/
    ├── test_order_book.cpp    OrderBook unit tests
    ├── test_matching.cpp      Matching engine unit tests
    └── test_pnl.cpp           PnL engine unit tests
```

## Build & Run

```bash
# Build
cmake -B build
cmake --build build

# Run the trading system
./build/trading_system

# Run all tests
cd build && ctest --output-on-failure
```

## Commands

### Task 1 -- Order Matching Engine

| Command | Format | Example |
|---------|--------|---------|
| NEW     | `NEW <id> <symbol> <BUY\|SELL> <price> <qty>` | `NEW 1 AAPL BUY 150 100` |
| CANCEL  | `CANCEL <id>` | `CANCEL 1` |
| MODIFY  | `MODIFY <id> <newPrice> <newQty>` | `MODIFY 1 155 50` |
| PRINT   | `PRINT BOOK` | `PRINT BOOK` |

### Task 2 -- Position & PnL Engine

| Command | Format | Example |
|---------|--------|---------|
| FILL    | `FILL <symbol> <BUY\|SELL> <qty> <price>` | `FILL AAPL BUY 100 150` |
| PRICE   | `PRICE <symbol> <marketPrice>` | `PRICE AAPL 170` |
| PRINT   | `PRINT PNL` | `PRINT PNL` |

Use `QUIT` to exit.

## Design Decisions

### Integer Prices
All prices are integers (representing cents) to avoid floating-point comparison
bugs. For display purposes, divide by 100 if needed. Internally, `150` means
$1.50 or can represent any unit -- the math stays exact.

### Data Structures
- **Buy orders:** `std::map<int, std::list<Order>, std::greater<int>>` -- sorted
  highest price first. Each price level holds a list of orders in FIFO order.
- **Sell orders:** `std::map<int, std::list<Order>>` -- sorted lowest price first.
- **Order index:** `std::unordered_map<int, OrderLocation>` -- O(1) lookup by
  order ID for cancel/modify using stored list iterators.
- `std::list` (not `std::queue`) is used per price level so that cancel can
  erase any order in the middle in O(1).

### Matching Rules
- BUY matches the **lowest** available SELL price.
- SELL matches the **highest** available BUY price.
- At the same price level, **earlier orders fill first** (time priority / FIFO).
- **Partial fills** are supported -- remaining quantity stays in the book.
- Trade executes at the **passive order's price** (the resting order).

### Modify Behavior
Modify is implemented as cancel + re-insert with a new timestamp. This means
modified orders **lose time priority**, which is standard exchange behavior.

### PnL Calculation
- **Average price** = total cost / |net quantity|
- **Realized PnL** is computed when closing a position (selling longs or
  buying back shorts).
- **Unrealized PnL** = (market price - avg price) * net quantity.
- Supports **position flipping** (e.g., going from long to short in one fill).

## Example Session

```
NEW 1 AAPL SELL 105 50
NEW 2 AAPL SELL 100 30
NEW 3 AAPL BUY 102 40
PRINT BOOK
PRICE AAPL 103
PRINT PNL
QUIT
```
# Tradesocio-assignment
# Tradesocio-assignment
# Tradesocio-assignment
