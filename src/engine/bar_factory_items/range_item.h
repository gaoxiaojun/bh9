#ifndef RANGE_BAR_FACTORY_ITEM_H
#define RANGE_BAR_FACTORY_ITEM_H

#include "../bar_factory_item.h"
#include "../bar_factory.h"

namespace h9 {

class RangeBarFactoryItem : public BarFactoryItem {
protected:
    void on_event(Event::Pointer e) override {
        auto tick = event_cast<ETick>(e);
    }
};

} // namespace h9

#endif // RANGE_BAR_FACTORY_ITEM_H
