#include "BinanceGateway.h"
#include "OrderBook.h"
#include "WinHttpClient.h"
#include "Logger.h"
#include "Types.h"

#include <nlohmann/json.hpp>
#include <chrono>
#include <thread>
#include <vector>

using json = nlohmann::json;

namespace tradex {

    BinanceGateway::BinanceGateway(std::shared_ptr<OrderBook> book, std::string restSymbol, std::string printSymbol)
        : book_(std::move(book)), restSymbol_(std::move(restSymbol)), printSymbol_(std::move(printSymbol)) {
    }

    BinanceGateway::~BinanceGateway() {
        Stop();
    }

    void BinanceGateway::Start() {
        if (running_) return;
        running_ = true;
        worker_ = std::thread(&BinanceGateway::PollLoop, this);
        Logger::Log("TradeX: Binance Gateway started.");
    }

    void BinanceGateway::Stop() {
        running_ = false;
        if (worker_.joinable()) worker_.join();
    }

    void BinanceGateway::SleepResponsive(std::chrono::milliseconds total) {
        const auto step = std::chrono::milliseconds(50);
        while (running_ && total.count() > 0) {
            auto s = (total > step) ? step : total;
            std::this_thread::sleep_for(s);
            total -= s;
        }
    }

    void BinanceGateway::PollLoop() {
        const std::string url =
            "https://api.binance.com/api/v3/depth?symbol=" + restSymbol_ + "&limit=10";

        while (running_) {
            auto t0 = std::chrono::high_resolution_clock::now();

            try {
                std::string body = WinHttpClient::Get(url, 5000);
                if (body.empty()) {
                    Logger::Log("TradeX: Empty response (retrying).");
                    SleepResponsive(std::chrono::milliseconds(1000));
                    continue;
                }

                json j = json::parse(body);

                if (!j.contains("bids") || !j.contains("asks")) {
                    if (j.contains("msg")) {
                        Logger::Log("TradeX: Binance error: " + j["msg"].get<std::string>()); //for debugging
                    }
                    else {
                        Logger::Log("TradeX: Invalid Binance response (no bids/asks).");
                    }
                    SleepResponsive(std::chrono::milliseconds(1000));
                    continue;
                }

                std::vector<Order> bids, asks;

                for (const auto& item : j["bids"]) {
                    bids.push_back(Order{
                        std::stod(item[0].get<std::string>()),
                        std::stod(item[1].get<std::string>()),
                        Side::BUY
                        });
                }

                for (const auto& item : j["asks"]) {
                    asks.push_back(Order{
                        std::stod(item[0].get<std::string>()),
                        std::stod(item[1].get<std::string>()),
                        Side::SELL
                        });
                }

                book_->UpdateSnapshot(bids, asks);
                book_->PrintTop();

            }
            catch (const std::exception& e) {
                Logger::Log(std::string("TradeX: Exception: ") + e.what());
                SleepResponsive(std::chrono::milliseconds(1000));
            }

            
            auto t1 = std::chrono::high_resolution_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
            if (ms.count() < 1000) SleepResponsive(std::chrono::milliseconds(1000 - ms.count()));
        }
    }

} 
