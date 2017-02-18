#include "util.h"

namespace h9 {

std::string bar_type_to_string(Bar::Type type) {
    switch (type) {
    case Bar::Type::kTime:
        return "Time";
    case Bar::Type::kTick:
        return "Tick";
    case Bar::Type::kVolume:
        return "Volume";
    case Bar::Type::kRange:
        return "Range";
    default:
        return "Session";
    }
}

std::string bar_input_to_string(BarInput input)
{
    switch(input) {
    case BarInput::kTrade:
        return "Trade";
    case BarInput::kBid:
        return "Bid";
    case BarInput::kAsk:
        return "Ask";
    case BarInput::kMiddle:
        return "Middle";
    case BarInput::kTick:
        return "Tick";
    default:
        return "BidAsk";
    }
}

} // namespace h9
