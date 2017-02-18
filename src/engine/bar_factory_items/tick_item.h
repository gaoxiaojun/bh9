#ifndef TICK_BAR_FACTORY_ITEM_H
#define TICK_BAR_FACTORY_ITEM_H

#include "../common.h"
#include "../event.h"
#include "../bar_factory_item.h"
#include "../bar_factory.h"
#include "../bar.h"

namespace h9 {

class TickBarFactoryItem : public BarFactoryItem {
public:
  TickBarFactoryItem(InstrumentId iid, long barSize, BarInput input,
                     ProviderId pid = -1)
      : BarFactoryItem(iid, Bar::Type::kTick, barSize, input, pid) {}

  TickBarFactoryItem(InstrumentId iid, long barSize, BarInput input,
                     time_duration session1, time_duration session2,ProviderId pid = -1)
      : BarFactoryItem(iid, Bar::Type::kTick, barSize, input, session1,
                       session2, pid) {}
protected:
    void on_tick(const Event::Pointer& e) override {
        BarFactoryItem::on_tick(e);
        if (m_bar->tick_count >=  m_size)
            emit_bar();
    }
};

} // namespace h9

#endif // TICK_BAR_FACTORY_ITEM_H
