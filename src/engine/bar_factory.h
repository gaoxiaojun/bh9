#ifndef BAR_FACTORY_H
#define BAR_FACTORY_H

#include "bar.h"
#include "bar_factory_item.h"
#include "clock.h"
#include "event.h"
#include <unordered_map>

namespace h9 {

using BFItemPtr = std::shared_ptr<BarFactoryItem>;

class BarFactory {

public:
  explicit BarFactory();

public:
  void add(const BarFactoryItem &item);
  void remove(const BarFactoryItem &item);
  void clear();

protected:
  void on_data(const Event::Pointer &e);
  bool add_reminder(const BarFactoryItem &item, ptime time);
  void on_reminder(ptime time, const BFItemPtr &item);

private:
  using ItemList = std::vector<BFItemPtr>;
  std::unordered_map<int, ItemList> m_item_lists;
  // 为了减轻定时器的开销，同一个时间的定时器回调串成链表，向Bus仅仅发送一个定时器请求
  std::unordered_map<ptime, ItemList> m_reminder_lists;
};

} // namespace h9
#endif // BAR_FACTORY_H
