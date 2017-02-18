#ifndef BAR_FACTORY_H
#define BAR_FACTORY_H

#include "bar.h"
#include "bar_factory_item.h"
#include "event.h"
#include <unordered_map>

namespace h9 {
class EventBus;
using BarFactoryItemPtr = std::shared_ptr<BarFactoryItem>;

class BarFactory : public noncopyable{

public:
  explicit BarFactory(EventBus *bus);

public:
  void add(InstrumentId iid, Bar::Type barType, long barSize,
           BarInput barInput = BarInput::kTrade);
  void add(InstrumentId iid, Bar::Type barType, long barSize, BarInput barInput,
           time_duration session1, time_duration session2);
  void add(const BarFactoryItemPtr &item);
  void remove(const BarFactoryItemPtr &item);
  void clear();

  void on_tick(const Event::Pointer &e);

protected:
  friend class BarFactoryItem;
  bool add_reminder(ptime time, const BarFactoryItemPtr &item);
  void on_reminder(ptime time);

private:
  EventBus *m_bus;
  std::unordered_multimap<int, BarFactoryItemPtr> m_item_map; // key = instrument_id
  // 为了减轻定时器的开销，同一个时间的定时器回调串成链表，向Bus仅仅发送一个定时器请求
  // 这个功能应该移入EventBus，对于EventBus应该是采取TimeWheel类似的数据结构
  std::unordered_multimap<ptime, BarFactoryItemPtr> m_reminder_map;
};

} // namespace h9
#endif // BAR_FACTORY_H
