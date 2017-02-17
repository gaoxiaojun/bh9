#ifndef EVENT_SERVER_H
#define EVENT_SERVER_H

#include "common.h"
#include "event.h"
#include <vector>

namespace h9 {

class Framework;
class EventBus;
class AccountReport;
class ExecutionReport;
class IProvider;
class Provider;
class ProviderError;
class Portfolio;
class Instrument;
class InstrumentDefinition;
class InstrumentDefinitionEnd;
class Strategy;
class Position;
class Transaction;
class Fill;
class Order;
class ExecutionCommand;
class ExecutionReport;

class EventServer : public noncopyable {
public:
  EventServer(Framework *f, EventBus *bus);

public:
  void clear();

  void emit_queued();

  void on_event(const Event::Point &e);

  //void on_data(DataObject data);

  void on_account_report(AccountReport report);

  void on_execution_report(ExecutionReport report);

  void on_provider_added(IProvider provider);

  void on_provider_removed(Provider provider);

  void on_provider_connected(Provider provider);

  void on_provider_disconnected(Provider provider);

  void on_provider_error(ProviderError error);

  void on_provider_statusChanged(Provider provider);

  void on_portfolio_parent_changed(Portfolio portfolio, bool queued);

  void on_portfolio_added(Portfolio portfolio);

  void on_portfolio_removed(Portfolio portfolio);

  void on_instrument_added(Instrument instrument);

  void on_instrument_definition(InstrumentDefinition definition);

  void on_instrument_defintionEnd(InstrumentDefinitionEnd end);

  void on_instrument_deleted(Instrument instrument);

  void on_strategy_added(Strategy strategy);

  void on_order_manager_cleared();

  void on_position_opened(Portfolio portfolio, Position position, bool queued);

  void on_position_closed(Portfolio portfolio, Position position, bool queued);

  void on_position_changed(Portfolio portfolio, Position position, bool queued);

  void on_log(Event e);

  void on_transaction(Portfolio portfolio, Transaction transaction,
                      bool queued);

  void on_fill(Portfolio portfolio, Fill fill, bool queued);

  void on_framework_cleared(Framework *framework);

  void on_send_order(Order order);

  void on_execution_command(ExecutionCommand command);

  void on_pending_new_order(Order order, bool queued = true);

  void on_order_status_changed(Order order, bool queued = true);

  void on_new_order(Order order, bool queued = true);

  void on_order_rejected(Order order, bool queued = true);

  void on_order_done(Order order, bool queued = true);

  void on_order_expired(Order order, bool queued = true);

  void on_order_partially_filled(Order order, bool queued = true);

  void on_order_filled(Order order, bool queued = true);

  void on_order_cancelled(Order order, bool queued = true);

  void on_order_cancel_rejected(Order order, bool queued = true);

  void on_order_replace_rejected(Order order, bool queued = true);

  void on_order_replaced(Order order, bool queued = true);

private:
  Framework* m_framework;
  EventBus*  m_bus;
  std::vector<Event::Pointer> m_queue;
};

} // namespace h9

#endif // EVENT_SERVER_H
