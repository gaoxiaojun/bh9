#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#include "common.h"
#include "event.h"

namespace h9
{
class Framework;

typedef std::function<void(ptime time)> ReminderCallback;

class EReminder : public Event
{
public:
  EReminder(ptime time, const ReminderCallback &callback)
      : Event(Event::Type::kReminder, time), m_callback(callback) {}

  void operator()() { m_callback(time()); }

private:
  ReminderCallback m_callback;
};

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
  Mode mode() const noexcept { return m_mode; }
  void set_mode(Mode mode) noexcept { m_mode = mode; }
  
  bool empty() const { return m_queue.empty() &&  m_local_clock_queue.empty(); }
  //std::size_t size() const { return m_queue.size(); }

  void enqueue(const Event::Pointer &e)
  {
    if (e->type() != Event::Type::kReminder)
      m_queue.push(e);
    else
       m_local_clock_queue.push(e);
  }

  void enqueue(Event::Pointer &&e)
  {
    if (e->type() != Event::Type::kReminder)
      m_queue.push(e);
    else
       m_local_clock_queue.push(e);
  }

  void add_timer(ptime time, const ReminderCallback &callback);

  Event::Pointer dequeue();

  ptime time() const;

private:
  Mode m_mode;
  ptime m_time;
 
  min_priority_queue<Event::Pointer> m_queue;
  // 为了减轻定时器的开销，同一个时间的定时器回调串成链表，向Bus仅仅发送一个定时器请求
  min_priority_queue<EReminder::Pointer>  m_local_clock_queue;
  min_priority_queue<EReminder::Pointer>  m_exchange_clock_queue;
};

} // namespace h9

#endif // EVENT_BUS_H
