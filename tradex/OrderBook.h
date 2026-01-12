#pragma once
#include "Types.h"

#include <map>
#include <shared_mutex>
#include <string>
#include <vector>

namespace tradex {

    class OrderBook {
    public:
        explicit OrderBook(std::string symbol);

        void UpdateSnapshot(const std::vector<Order>& bids, const std::vector<Order>& asks);
        void PrintTop() const;

    private:
        std::string symbol_;
        std::map<Price, Order> bids_; 
        std::map<Price, Order> asks_; 
        mutable std::shared_mutex mtx_;
    };

}
