#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include "bar_factory.h"
#include "bar_slice_factory.h"
#include "event_bus.h"

namespace h9 {
class Framework;
class EventBus;

class EventFilter {
public:
  virtual Event::Pointer filter(const Event::Pointer &e) { return e; }
};

class EventManager {
public:
  enum class Status { Running, Paused, Stopping, Stopped };

public:
  explicit EventManager(Framework *framework, EventBus *bus);
  ~EventManager();

public:
  void start();
  void stop();
  void pause();
  void pause(ptime time);
  void resume();
  void step(Event::Type type = Event::Type::kEvent);
  void clear();
protected:
  // event process entry
  void on_event(const Event::Pointer &e);

  // market data default handle
  void on_bid(const Event::Pointer &e);
  void on_ask(const Event::Pointer &e);
  void on_trade(const Event::Pointer &e);
  void on_queote(const Event::Pointer &e);
  void on_bar(const Event::Pointer &e);
  void on_bar_slice(const Event::Pointer &e);
  void on_level2snapshot(const Event::Pointer &e);
  void on_level2update(const Event::Pointer &e);
  void on_news(const Event::Pointer &e);
  void on_fundamental(const Event::Pointer &e);

  // order status default handle
  void on_new_order(const Event::Pointer &e);
  void on_send_order(const Event::Pointer &e);
  void on_pending_new_order(const Event::Pointer &e);
  void on_order_done(const Event::Pointer &e);
  void on_order_rejected(const Event::Pointer &e);
  void on_order_replace_rejected(const Event::Pointer &e);
  void on_order_cancel_rejected(const Event::Pointer &e);
  void on_order_expired(const Event::Pointer &e);
  void on_order_cancelled(const Event::Pointer &e);
  void on_order_replaced(const Event::Pointer &e);
  void on_order_filled(const Event::Pointer &e);
  void on_order_partially_filled(const Event::Pointer &e);
  void on_order_status_changed(const Event::Pointer &e);

  // execution report default handle
  void on_execution_report(const Event::Pointer &e);
  void on_fill(const Event::Pointer &e);
  void on_transaction(const Event::Pointer &e);

  // account info default handle
  void on_account_report(const Event::Pointer &e);
  void on_account_data(const Event::Pointer &e);

  // historical data default handle
  void on_historical_data_end(const Event::Pointer &e);
  void on_historical(const Event::Pointer &e);

  // group handle
  void on_group_event(const Event::Pointer &e);
  void on_group(const Event::Pointer &e);

  // portoflio handle
  void on_portfolio_error(const Event::Pointer &e);
  void on_portfolio_added(const Event::Pointer &e);
  void on_portfolio_parent_changed(const Event::Pointer &e);
  void on_portfolio_removed(const Event::Pointer &e);

  // position handle
  void on_position_opened(const Event::Pointer &e);
  void on_position_closed(const Event::Pointer &e);
  void on_position_changed(const Event::Pointer &e);

  // provider default handle
  void on_provider_error(const Event::Pointer &e);
  void on_provider_connected(const Event::Pointer &e);
  void on_provider_disconnected(const Event::Pointer &e);

  // simulator handle
  void on_simulator_start(const Event::Pointer &e);
  void on_simulator_stop(const Event::Pointer &e);
  void on_simulator_progress(const Event::Pointer &e) {}

  // misc
  void on_property_changed(const Event::Pointer &e);
  void on_exception(const Event::Pointer &e);

  // timer handle
  void on_reminder(const Event::Pointer &e);
  // user command handle
  void on_command(const Event::Pointer &e);
  void on_strategy_event(const Event::Pointer &e);

private:
  Framework *m_framework;
  EventBus *m_bus;
  Status m_status;
  BarFactory m_bar_factory;
  BarSliceFactory m_slice_factory;

  bool m_stepping;
  Event::Type m_step_event;
};

} // namespace h9

#endif // EVENT_MANAGER_H
