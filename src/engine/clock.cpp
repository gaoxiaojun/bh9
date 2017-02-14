#include "clock.h"

using namespace h9;

Clock::Clock() : m_time(min_date_time), m_mode(Mode::kSimulation) {}

long Clock::add_timer(ptime time, const ReminderCallback &callback) {
    long tid = m_sid++;
    auto eptr = std::make_shared<EReminder>(tid, time, callback)
}

void Clock::remove_timer(long id) {}

ptime Clock::time() const {}

Clock::Clock
