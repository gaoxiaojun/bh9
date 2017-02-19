#ifndef TIME_FACTORY_ITEM_H
#define TIME_FACTORY_TIME_H

#include "../common.h"
#include "../event.h"
#include "../bar_factory.h"
#include "../bar_factory_item.h"

namespace h9 {

class TimeBarFactoryItem : public BarFactoryItem {
public:
  TimeBarFactoryItem(InstrumentId iid, long barSize, BarInput input,
                     ProviderId pid = -1)
      : BarFactoryItem(iid, Bar::Type::kTime, barSize, input, pid) {}

  TimeBarFactoryItem(InstrumentId iid, long barSize, BarInput input,
                     time_duration session1, time_duration session2,ProviderId pid = -1)
      : BarFactoryItem(iid, Bar::Type::kTime, barSize, input, session1,
                       session2, pid) {}

protected:
  void on_tick(const Event::Pointer& e) override;
  virtual void on_reminder(ptime time) override;
  virtual ptime get_bar_close_time(const Event::Pointer &e) override;
};

} // namespace h9

#endif // TIME_BAR_FACTORY_H
