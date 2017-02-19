#ifndef RANGE_BAR_FACTORY_ITEM_H
#define RANGE_BAR_FACTORY_ITEM_H

#include "../common.h"
#include "../event.h"
#include "../bar_factory_item.h"
#include "../bar_factory.h"

namespace h9 {

class RangeBarFactoryItem : public BarFactoryItem {
public:
public:
  RangeBarFactoryItem(InstrumentId iid, long barSize, BarInput input,
                     ProviderId pid = -1)
      : BarFactoryItem(iid, Bar::Type::kRange, barSize, input, pid) {}

  RangeBarFactoryItem(InstrumentId iid, long barSize, BarInput input,
                     time_duration session1, time_duration session2, ProviderId pid = -1)
      : BarFactoryItem(iid, Bar::Type::kRange, barSize, input, session1,
                       session2, pid) {}
protected:
    void on_tick(const Event::Pointer& e) override {
        auto tick = event_cast<ETick>(e);
    }
};

} // namespace h9

#endif // RANGE_BAR_FACTORY_ITEM_H
