#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#include "clock.h"
#include "common.h"
#include "event.h"
#include <boost/noncopyable.hpp>
#include <queue>

namespace h9 {

class EventBus : private boost::noncopyable {
public:
  enum class Mode { kRealtime, kSimulation };

public:
  explicit EventBus();
  ~EventBus() = default;

public:
  bool empty() const { m_queue.empty(); }
  std::size_t size() const { return m_queue.size(); }

  void enqueue(const T &e) {
    if (e->type() == Event::Type::kReminder)
      m_timer_queue.push(e);
    else
      m_queue.push(e);
  }

  void enqueue(T &&e) {
    if (e->type() == Event::Type::kReminder)
      m_timer_queue.push(e);
    else
      m_queue.push(e);
  }

  const T &dequeue() const;

  void pop() { m_queue.pop(); }

  ptime time() const noexcept;

private:
  Mode m_mode;
  ptime m_time;
  min_priority_queue<Event::Pointer> m_queue;
  min_priority_queue<EReminder::Pointer> m_timer_queue;
};

} // namespace h9

#endif // EVENT_BUS_H
