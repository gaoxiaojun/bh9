#include "framework.h"

using namespace h9;

Framework::Framework()
    : m_bus(this)
    , m_event_manager(this, &m_bus)
    , m_event_server(this, &m_bus)
{

}
