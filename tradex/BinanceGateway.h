#pragma once
#include <atomic>
#include <memory>
#include <string>
#include <thread>

namespace tradex {

    class OrderBook;

    class BinanceGateway {
    public:
        BinanceGateway(std::shared_ptr<OrderBook> book, std::string restSymbol, std::string printSymbol);
        ~BinanceGateway();

        void Start();
        void Stop();

    private:
        void PollLoop();
        void SleepResponsive(std::chrono::milliseconds total);

    private:
        std::shared_ptr<OrderBook> book_;
        std::string restSymbol_;
        std::string printSymbol_;

        std::atomic<bool> running_{ false };
        std::thread worker_;
    };

} 

