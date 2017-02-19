#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include "event_bus.h"
#include "event_manager.h"
#include "event_server.h"

namespace h9 {
class Configuration;
class AccountDataManager;
class StreamerManager;
class DataFileManager;
class DataManager;
class EventManager;
class EventLoggerManager;
class SubscriptionManager;
class ScenarioManager;
class GroupManager;
class GroupDispatcher;
class InstrumentManager;
class ICurrencyConverter;
class IDataProvider;
class IExecutionProvider;
class InstrumentServer;
class DataServer;
class OrderServer;
class PortfolioServer;
class OrderManager;
class ProviderManager;
class StatisticsManager;
class StrategyManager;
class UserServer;
class UserManager;
class EventServer;
class PortfolioManager;
class OutputManager;
class EventBus;


class Framework {
public:
  enum class Mode { kSimulation, kRealtime };

public:
  Framework();

public:
  EventBus &event_bus() { return m_bus; }
  EventManager &event_manager() { return m_event_manager; }
  EventServer &event_server() { return m_event_server; }

private:
  EventBus m_bus;
  EventManager m_event_manager;
  EventServer m_event_server;
};

} // namespace h9

#endif // FRAMEWORK_H
