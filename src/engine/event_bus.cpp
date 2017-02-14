#include "event_bus.h"
#include <iostream>
using namespace h9;

EventBus::EventBus()
    : m_mode(Mode::kSimulation), m_time(min_date_time) {}

bool is_market_event(Event::Type type) {
  return type == Event::Type::kAsk || type == Event::Type::kBid ||
         type == Event::Type::kTrade || type == Event::Type::kQuote;
}

const Event::Pointer &EventBus::dequeue() const {

  if (m_mode == Mode::kRealtime) {
  } else {
    while (true) {
        // 1.update local time
      auto &e = m_queue.top();
      if (is_market_event(e->type())) {
        if (e->time() < m_time) {
          std::cout << "Warning invalid datetime" << std::endl;
          m_queue.pop(); // discard the event
          continue;
        }
        else {
            m_time = e->time();
        }
      }
      // 2. check timer
      
    }
  }
}

ptime EventBus::time() const {
    if (m_mode == Mode::kRealtime)
        return boost::posix_time::microsec_clock::local_time();
    else
        return m_time;
}
