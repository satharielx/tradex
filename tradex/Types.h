#pragma once
#include <string>

namespace tradex {

    using Price = double;
    using Quantity = double;

    enum class Side { BUY, SELL };

    struct Order {
        Price price{};
        Quantity quantity{};
        Side side{};
    };

} 
