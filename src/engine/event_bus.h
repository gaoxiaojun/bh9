#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#include "common.h"
#include "event.h"

namespace h9 {
class Framework;

typedef std::function<void(ptime time)> ReminderCallback;

class EReminder : public Event {
public:
  EReminder(ptime time, const ReminderCallback &callback)
      : Event(Event::Type::kReminder, time), m_callback(callback) {}

  void operator()() { m_callback(time()); }

private:
  ReminderCallback m_callback;
};

enum class ReminderOrder { kBefore, kAfter };

class EventBus : private noncopyable {
public:
  enum class Mode { kRealtime, kSimulation };

public:
  explicit EventBus();
  ~EventBus() = default;

public:
  ptime time() const;

  Mode mode() const noexcept { return m_mode; }
  void set_mode(Mode mode) noexcept { m_mode = mode; }

  ReminderOrder reminder_order() const { return m_reminder_order; }
  void set_reminder_order(ReminderOrder v) { m_reminder_order = v; }

  bool empty() const { return m_queue.empty() && m_local_clock_queue.empty(); }

  void enqueue(const Event::Pointer &e);
  void enqueue(Event::Pointer &&e);

  Event::Pointer dequeue();

  void clear();

private:
  Mode m_mode;
  ptime m_time;
  ReminderOrder m_reminder_order;
  min_priority_queue<Event::Pointer> m_queue;
  min_priority_queue<EReminder::Pointer> m_local_clock_queue;
};

} // namespace h9

#endif // EVENT_BUS_H
