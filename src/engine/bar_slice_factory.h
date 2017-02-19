#ifndef BAR_SLICE_FACTORY_H
#define BAR_SLICE_FACTORY_H

#include "bar.h"
#include "event.h"
#include <unordered_map>

namespace h9 {

class Framework;

class EBarSlice : public Event {
public:
  explicit EBarSlice(const Bar &bar)
      : Event(Event::Type::kBarSlice, bar.time()) {}

  explicit EBarSlice(ptime time, long size)
      : Event(Event::Type::kBarSlice, time), m_size(size) {}

  long size() const { return m_size; }

private:
  long m_size;
};

struct BarSliceItem {
  std::vector<Bar> m_bars;
  ptime m_close_time;
  int m_bar_count;
};

class BarSliceFactory {
public:
  explicit BarSliceFactory(Framework *framework) : m_framework(framework) {}

  void clear() { m_items.clear(); }

protected:
  void on_bar(const Bar &bar);

  bool on_bar_open(const Bar &bar);

private:
  Framework *m_framework;
  std::unordered_map<long, BarSliceItem> m_items;
};

} // namespace h9
#endif // BAR_SLICE_FACTORY_H
