#ifndef CLOCK_H
#define CLOCK_H

#include "common.h"
#include "event.h"
#include <boost/noncopyable.hpp>
#include <functional>

namespace h9 {

typedef std::function<void(long id, ptime time)> ReminderCallback;

class EventBus;

class Clock : privte boost::noncopyable {
public:
  enum class Mode { kRealtime, kSimulation };

public:
 explicit Clock(EventBus& bus);

public:
  long add_timer(ptime time, const ReminderCallback &callback);
  void remove_timer(long id);

  ptime time() const noexcept;

private:
  static long m_sid = {0};
  EventBus& bus;
  ptime m_time;
  Mode  m_mode;
};

class EReminder : public Event {
public:
  Reminder(long id, ptime time, const ReminderCallback &callback)
      : Event(Event::Type::Reminder, time), m_id(id), m_callback(callback) {}

  void operator() { m_callback(id, time()); }

private:
  long m_id;
  ReminderCallback m_callback;
}

} // namespace h9

#endif // CLOCK_H
