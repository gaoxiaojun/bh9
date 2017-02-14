#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#include "common.h"
#include "event.h"
#include <queue>

namespace h9
{

typedef std::function<void(ptime time)> ReminderCallback;

class EventBus : private noncopyable
{
public:
  enum class Mode
  {
    kRealtime,
    kSimulation
  };

public:
  explicit EventBus();
  ~EventBus() = default;

public:
  bool empty() const { m_queue.empty(); }
  std::size_t size() const { return m_queue.size(); }

  void enqueue(const T &e)
  {
    if (e->type() != Event::Type::kReminder)
      m_queue.push(e);
    else
      m_timer_queue.push(e);
  }

  void enqueue(T &&e)
  {
    if (e->type() != Event::Type::kReminder)
      m_queue.push(e);
    else
      m_timer_queue.push(e);
  }

  void add_timer(ptime time, const ReminderCallback &callback);

  const T &dequeue() const;

  void pop() { m_queue.pop(); }

  ptime time() const { return m_time; }

private:
  Mode m_mode;
  ptime m_time;
  min_priority_queue<Event::Pointer> m_queue;
  min_priority_queue<EReminder::Pointer> m_timer_queue;
};

class EReminder : public Event
{
public:
  Reminder(ptime time, const ReminderCallback &callback)
      : Event(Event::Type::Reminder, time), m_callback(callback) {}

  void operator() { m_callback(id, time()); }

private:
  ReminderCallback m_callback;
}

} // namespace h9

#endif // EVENT_BUS_H
