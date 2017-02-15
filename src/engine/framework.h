#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include "event_manager.h"
#include "event_bus.h"

namespace h9 {

class Framework
{
public:
    Framework();

public:
    EventManager& event_manager() { return m_event_manager; }
    EventBus& event_bus() { return m_event_bus; }

private:
    EventBus m_bus;
    EventManager m_event_manager;

};

} // namespace h9

#endif // FRAMEWORK_H
