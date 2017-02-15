#ifndef CLOCK_H
#define CLOCK_H

#include "event.h"
#include "event_bus.h"

namespace h9 {

class Framework;

class Clock {
public:
    enum Type {
        kLocal,
        kExchange
    };

public:
    Clock(Framework *framework, Type mode);

public:
    void add_reminder(const EReminder& reminder);
    void remove_reminder(const EReminder& reminder);

    ptime time() const;

private:
    Type m_mode;
};

} // namespace h9

#endif // CLOCK_H
