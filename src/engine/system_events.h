#ifndef SYSTEM_EVENTS_H
#define SYSTEM_EVENTS_H

#include "event.h"

namespace h9 {
class Framework;
class IProvider;
class EOnEventManagerStarted : public Event {
public:
  EOnEventManagerStarted()
      : Event(Event::Type::kOnEventManagerStarted, clock::local_time()) {}
};

class EOnEventManagerStep : public Event {
public:
  EOnEventManagerStep()
      : Event(Event::Type::kOnEventManagerStep, clock::local_time()) {}
};

class EOnEventManagerPaused : public Event {
public:
  EOnEventManagerPaused()
      : Event(Event::Type::kOnEventManagerPaused, clock::local_time()) {}
};
class EOnEventManagerResumed : public Event {
public:
  EOnEventManagerResumed()
      : Event(Event::Type::kOnEventManagerResumed, clock::local_time()) {}
};

class EOnEventManagerStopped : public Event {
public:
  EOnEventManagerStopped()
      : Event(Event::Type::kOnEventManagerStopped, clock::local_time()) {}
};

class EOnFrameworkCleared : public Event {
public:
  Framework *framework;

  EOnFrameworkCleared(Framework *framework)
      : Event(Event::Type::kOnEventManagerStopped), framework(framework) {}
};

class EOnSimulatorStop : public Event {
public:
  EOnSimulatorStop()
      : Event(Event::Type::kOnSimulatorStop, clock::local_time()) {}
};

class EOnOrderManagerCleared : public Event {
public:
  EOnOrderManagerCleared() : Event(Event::Type::kOnOrderManagerCleared) {}
};

class EOnProviderAdded : public Event {
public:
  ProviderId id;
  IProvider *provider;

  EOnProviderAdded(IProvider *provider)
      : Event(Event::Type::kOnProviderAdded), id(provider->id()),
        provider(provider) {}
};

class EOnProviderRemoved : public Event {
public:
  ProviderId id;
  IProvider *provider;

  EOnProviderRemoved(IProvider *provider)
      : Event(Event::Type::kOnProviderRemoved), id(provider->id()),
        provider(provider) {}
};

class EOnProviderConnected : public Event {
public:
  ProviderId id;
  IProvider *provider;

  EOnProviderConnected(ptime dateTime, ProviderId id)
      : Event(Event::Type::kOnProviderConnected, dateTime), id(id) {}

  EOnProviderConnected(ptime dateTime, IProvider *provider)
      : Event(Event::Type::kOnProviderConnected, dateTime), id(provider->id()),
        provider(provider) {}
};

class EOnProviderDisconnected : public Event {
public:
  ProviderId id;
  IProvider *provider;

  EOnProviderDisconnected(ptime dateTime, ProviderId id)
      : Event(Event::Type::kOnProviderDisconnected, dateTime), id(id) {}

  EOnProviderDisconnected(ptime dateTime, IProvider *provider)
      : Event(Event::Type::kOnProviderDisconnected, dateTime),
        id(provider->id()), provider(provider) {}
};

class EOnProviderStatusChanged : public Event {
public:
  ProviderId id;
  IProvider *provider;

  EOnProviderStatusChanged(IProvider *provider)
      : Event(Event::Type::kOnProviderStatusChanged), id(provider->id()),
        provider(provider) {}
};

/*class EOnLogin : public Event {
  EOnLogin() {}

  EOnLogin(ptime dateTime) : base(dateTime) {}

  std::string GUID;

  int Id;

  std::string Password;

  std::string ProductName;

  std::string UserName;
};

class EOnLogout : public Event {
  EOnLogout() {}

  EOnLogout(ptime dateTime) : base(dateTime) {}

  int Id;
  std::string ProductName;
  std::string Reason;
  std::string UserName;
};

class EOnLoggedIn : public Event {
  EOnLoggedIn() {}

  EOnLoggedIn(ptime dateTime) : base(dateTime) {}

  int DefaultAlgoId;

  ObjectTable Fields;

  int UserId;

  std::string UserName;
};

class EOnLoggedOut : public Event {
  EOnLoggedOut() {}

  EOnLoggedOut(ptime dateTime) : Event(Event::Type::kEOnLoggedOut, dateTime) {}
};

class EOnHeartbeat : public Event {
  EOnHeartbeat() {}

  EOnHeartbeat(ptime dateTime) : Event(Event::Type::kEOnHeartbeat, dateTime) {}
};*/

} // namespace h9
#endif // SYSTEM_EVENTS_H
