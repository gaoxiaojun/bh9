#ifndef TIME_FACTORY_ITEM_H
#define TIME_FACTORY_TIME_H

#include "../bar_factory_item.h"
#include "../bar_factory.h"

namespace h9 {
 
class TimeBarFactoryItem : public BarFactoryItem {
public:
  TimeBarFactoryItem(InstrumentId iid, long barSize, Bar::Input input,
                     Provider pid = -1)
      : BarFactoryItem(iid, Bar::Type::Time, barSize, input, pid) {}

protected:
  void on_event(Event::Pointer e) override;
  virtual void on_reminder(ptime time) override;
  virtual ptime get_bar_close_time(const Event::Pointer &e) override;
};

} // namespace h9

#endif // TIME_BAR_FACTORY_H
