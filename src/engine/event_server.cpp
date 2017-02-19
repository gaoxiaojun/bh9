#include "event_server.h"
#include "execution_events.h"
#include "market_events.h"
#include "system_events.h"
using namespace h9;

EventServer::EventServer(Framework *f, EventBus *bus)
    : m_framework(f), m_bus(bus) {}

void EventServer::on_order_replaced(Order order, bool queued) {
  auto e = make_event<EOnOrderReplaced>(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_position_opened(Portfolio portfolio, Position position,
                                     bool queued) {
  auto e = make_event<EOnPositionOpened>(portfolio, position);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_position_closed(Portfolio portfolio, Position position,
                                     bool queued) {
  auto e = make_event<EOnPositionClosed>(portfolio, position);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_position_changed(Portfolio portfolio, Position position,
                                      bool queued) {
  auto e = make_event<EOnPositionChanged>(portfolio, position);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_log(Event e) { on_event(e); }

void EventServer::on_transaction(Portfolio portfolio, Transaction transaction,
                                 bool queued) {
  auto e = make_event<EOnTransaction>(portfolio, transaction);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_fill(Portfolio portfolio, Fill fill, bool queued) {
  auto e = make_event<EOnFill>(portfolio, fill);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_framework_cleared(Framework *framework) {
  on_event(make_event<EOnFrameworkCleared>(framework));
}

void EventServer::On_send_order(Order order) {
  on_event(make_event < EOnSendOrder(order));
}

void EventServer::on_execution_command(ExecutionCommand command) {
  on_event(command);
}

void EventServer::on_pending_new_order(Order order, bool queued) {
  auto e = make_event<EOnPendingNewOrder>(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_order_status_changed(Order order, bool queued) {
  auto e = make_event<EOnOrderStatusChanged>(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_new_order(Order order, bool queued) {
  auto e = make_event<EOnNewOrder>(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_order_rejected(Order order, bool queued) {
  auto e = make_event<EOnOrderRejected>(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_order_done(Order order, bool queued) {
  auto e = make_event<EOnOrderDone>(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::OnOrderExpired(Order order, bool queued) {
  auto e = make_event<EOnOrderExpired>(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_order_partially_filled(Order order, bool queued) {
  auto e = make_event<EOnOrderPartiallyFilled>(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_order_filled(Order order, bool queued) {
  auto e = make_event<EOnOrderFilled>(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_order_cancelled(Order order, bool queued) {
  auto e = make_event<EOnOrderCancelled>(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_order_cancel_rejected(Order order, bool queued) {
  auto e = make_event<EOnOrderCancelRejected>(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_order_replace_rejected(Order order, bool queued) {
  auto e = make_event<EOnOrderReplaceRejected>(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_event(Event e) {
  m_framework->event_manager()->on_event(e);
}

void EventServer::on_event(const Event::Point &e) {
  m_framework->event_manager()->on_event(e);
}

// void EventServer::on_data(DataObject data) { on_event(data); }

void EventServer::on_account_report(AccountReport report) { on_event(report); }

void EventServer::on_execution_report(ExecutionReport report) {
  on_event(report);
}

void EventServer::on_provider_added(IProvider provider) {
  on_event(make_event<EOnProviderAdded>(provider));
}

void EventServer::on_provider_removed(Provider provider) {
  on_event(make_event<EOnProviderRemoved>(provider));
}

void EventServer::on_provider_connected(Provider provider) {
  on_event(make_event<EOnProviderConnected>(this.framework.Clock.DateTime,
                                            provider));
}

void EventServer::on_provider_disconnected(Provider provider) {
  on_event(make_event<EOnProviderDisconnected>(this.framework.Clock.DateTime,
                                               provider));
}

void EventServer::on_provider_error(ProviderError error) { on_event(error); }

void EventServer::on_provider_status_changed(Provider provider) {
  switch (provider.Status) {
  case ProviderStatus.Connected:
    OnProviderConnected(provider);
    break;
  case ProviderStatus.Disconnected:
    OnProviderDisconnected(provider);
    break;
  }
  on_event(make_event<EOnProviderStatusChanged>(provider));
}

void EventServer::on_portfolio_parent_changed(Portfolio portfolio,
                                              bool queued) {
  if (queued)
    on_event(make_event<EOnPortfolioParentChanged>(portfolio));
}

void EventServer::on_portfolio_added(Portfolio portfolio) {
  on_event(make_event<EOnPortfolioAdded>(portfolio));
}

void EventServer::on_portfolio_removed(Portfolio portfolio) {
  on_event(make_event<EOnPortfolioRemoved>(portfolio));
}

void EventServer::on_instrument_added(Instrument instrument) {
  on_event(make_event<EOnInstrumentAdded>(instrument));
}

void EventServer::on_instrument_definition(InstrumentDefinition definition) {
  on_event(make_event<EOnInstrumentDefinition>(definition));
}

void EventServer::on_instrument_defintionEnd(InstrumentDefinitionEnd end) {
  on_event(make_event<EOnInstrumentDefinitionEnd>(end));
}

void EventServer::on_instrument_deleted(Instrument instrument) {
  on_event(make_event<EOnInstrumentDeleted>(instrument));
}

void EventServer::on_strategy_added(Strategy strategy) {
  on_event(make_event<EOnStrategyAdded>(strategy));
}

void EventServer::on_order_manager_cleared() {
  on_event(make_event<EOnOrderManagerCleared>());
}
