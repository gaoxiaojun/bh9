#ifndef BAR_SLICE_FACTORY_H
#define BAR_SLICE_FACTORY_H

#include "event.h"
#include "bar2.h"

namespace h9 {

class EBarSlice : public Event {
public:
  explicit EBarSlice(const Bar &bar)
      : Event(Event::Type::kBarSlice, bar.time()) {}

  explicit EBarSlice(ptime time, long size)
      : Event(Event::Type::kBarSlice, time) : m_size(size) {}

  long size() const { return m_size; }

private:
  long m_size;

}

class BarSliceItem {
private:
  std::vector<Bar> m_bars;
  ptime m_close_time;
  int m_bar_count;
}

class BarSliceFactory {
public:
    BarSliceFactory();

    void clear();

protected:
    void on_bar(const Bar& bar);
    bool on_bar_open(const Bar& bar);
private:
    std::vector<BarSliceItem> m_items;
};

} // namespace h9
#endif // BAR_SLICE_FACTORY_H
