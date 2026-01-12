#include "OrderBook.h"
#include "Logger.h"

#include <sstream>
#include <iomanip>

namespace tradex {

    OrderBook::OrderBook(std::string symbol)
        : symbol_(std::move(symbol)) {
    }

    void OrderBook::UpdateSnapshot(const std::vector<Order>& bids, const std::vector<Order>& asks) {
        std::unique_lock<std::shared_mutex> lock(mtx_);
        bids_.clear();
        asks_.clear();

        for (const auto& o : bids) bids_[o.price] = o;
        for (const auto& o : asks) asks_[o.price] = o;
    }

    void OrderBook::PrintTop() const {
        std::shared_lock<std::shared_mutex> lock(mtx_);
        if (bids_.empty() || asks_.empty()) return;

        auto bestBid = bids_.rbegin(); 
        auto bestAsk = asks_.begin();  

        const double spread = bestAsk->first - bestBid->first;

        std::ostringstream ss;
        ss << "TradeX [" << symbol_ << "] "
            << "Spread: $" << std::fixed << std::setprecision(2) << spread
            << " | Bid: " << bestBid->first << " (" << bestBid->second.quantity << ")"
            << " | Ask: " << bestAsk->first << " (" << bestAsk->second.quantity << ")";

        Logger::Log(ss.str());
    }

} 
