#ifndef VOLUME_BAR_FACTORY_ITEM_H
#define VOLUME_BAR_FACTORY_ITEM_H

#include "../common.h"
#include "../event.h"
#include "../bar_factory_item.h"
#include "../bar_factory.h"

namespace h9 {

class VolumeBarFactoryItem : public BarFactoryItem {
public:
public:
  VolumeBarFactoryItem(InstrumentId iid, long barSize, BarInput input,
                     ProviderId pid = -1)
      : BarFactoryItem(iid, Bar::Type::kVolume, barSize, input, pid) {}

  VolumeBarFactoryItem(InstrumentId iid, long barSize, BarInput input,
                     time_duration session1, time_duration session2,ProviderId pid = -1)
      : BarFactoryItem(iid, Bar::Type::kVolume, barSize, input, session1,
                       session2, pid) {}
protected:
    void on_tick(const Event::Pointer& e) override {
        BarFactoryItem::on_tick(e);
        if (m_bar->volume >=  m_size)
            emit_bar();
    }
};

} // namespace h9

#endif // VOLUME_BAR_FACTORY_ITEM_H
