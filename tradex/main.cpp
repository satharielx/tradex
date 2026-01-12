#include "Logger.h"
#include "OrderBook.h"
#include "BinanceGateway.h"

#include <memory>
#include <iostream>

int main() {
    using namespace tradex;

    Logger::Log("TradeX booting...");

    
    auto book = std::make_shared<OrderBook>("BTC-USDT");

    BinanceGateway gw(book, "BTCUSDT", "BTC-USDT");
    gw.Start();

    Logger::Log("Press Enter to stop...");
    std::cin.get();

    gw.Stop();
    Logger::Log("TradeX shutdown.");
    return 0;
}
