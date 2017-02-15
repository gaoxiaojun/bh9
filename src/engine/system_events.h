#ifndef SYSTEM_EVENTS_H
#define SYSTEM_EVENTS_H

#include "event.h"

namespace h9 {

class EOnEventManagerStarted : public Event
{
public:
    EOnEventManagerStarted() : Event(Event::Type::kOnEventManagerStarted, clock::local_time())
    {}
};


class EOnEventManagerStep : public Event
{
public:
    EOnEventManagerStep() : Event(Event::Type::kOnEventManagerStep, clock::local_time())
    {}
};

class EOnEventManagerPaused : public Event
{
public:
    EOnEventManagerPaused() : Event(Event::Type::kOnEventManagerPaused, clock::local_time())
    {}
};
class EOnEventManagerResumed : public Event
{
public:
    EOnEventManagerResumed() : Event(Event::Type::kOnEventManagerResumed, clock::local_time())
    {}
};

class EOnEventManagerStopped : public Event
{
public:
    EOnEventManagerStopped() : Event(Event::Type::kOnEventManagerStopped, clock::local_time())
    {}
};

class EOnSimulatorStop : public Event
{
public:
    EOnSimulatorStop() : Event(Event::Type::kOnSimulatorStop, clock::local_time())
    {}
};



} // namespace h9
#endif // SYSTEM_EVENTS_H
