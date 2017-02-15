#include "event_bus.h"
#include <iostream>
using namespace h9;

//TODO: reminderOrder
EventBus::EventBus() : m_mode(Mode::kSimulation), m_time(min_date_time), m_reminder_order(ReminderOrder::kBefore) {}

inline bool is_market_event(Event::Type type) {
  return type == Event::Type::kAsk || type == Event::Type::kBid ||
         type == Event::Type::kTrade || type == Event::Type::kQuote;
}

inline void show_warning(Event::Type type, ptime etime, ptime ctime) {
  std::cout << "Warning event datetime less than current clock, the "
               "event would be ignored:["
            << type << "]: " << time << " < " << ctime << std::endl;
}

Event::Pointer EventBus::dequeue() {
  if (m_mode == Mode::kRealtime) {
    while (true) {
      // 1. check local clock timer
      if (! m_local_clock_queue.empty()) {
        auto reminder =  m_local_clock_queue.top();
        if (reminder->time() < time()) {
           m_local_clock_queue.pop();
          return reminder;
        }
      }
      // 2. check exchange clock timer
      // 3. check has event
      if (!m_queue.empty()) {
        auto event = m_queue.top();
        m_queue.pop();
        return event;
      } else // return nullptr to mark no event in queue
        return nullptr;
    }
  }      // end realtime mode
  else { // simulation mode
    while (true) {
      // 1.update local time
      auto event = m_queue.empty() ? nullptr : m_queue.top();
      if (event && is_market_event(event->type())) {
        if (event->time() < m_time) {
          show_warning(event->type(), event->time(), time());
          m_queue.pop(); // discard the event
          continue;
        } else {
          m_time = event->time();
        }
      }
      // 2. check local clock timer
      if (! m_local_clock_queue.empty() && event) {
        auto reminder =  m_local_clock_queue.top();
        if (reminder->time() < event->time()) {
           m_local_clock_queue.pop();
          return reminder;
        }
      }
      // 3. check exchange clock timer;
      // 4.
      if (event)
        m_queue.pop();
      return event;
    }
  }
}

/*void EventBus::add_timer(ptime time, const ReminderCallback &callback) {
  auto eptr = std::make_shared<EReminder>(time, callback);
   m_local_clock_queue.push(std::move(eptr));
}*/

ptime EventBus::time() const {
  if (m_mode == Mode::kRealtime)
    return clock::local_time();
  else
    return m_time;
}
