#include "event_server.h"

using namespace h9;

void EventServer::on_order_replaced(Order order, bool queued) {
  auto e = new OnOrderReplaced(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_position_opened(Portfolio portfolio, Position position,
                                     bool queued) {
  auto e = new OnPositionOpened(portfolio, position);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_position_closed(Portfolio portfolio, Position position,
                                     bool queued) {
  auto e = new OnPositionClosed(portfolio, position);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_position_changed(Portfolio portfolio, Position position,
                                      bool queued) {
  auto e = new OnPositionChanged(portfolio, position);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_log(Event e) { on_event(e); }

void EventServer::on_transaction(Portfolio portfolio, Transaction transaction,
                                 bool queued) {
  auto e = new OnTransaction(portfolio, transaction);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_fill(Portfolio portfolio, Fill fill, bool queued) {
  auto e = new OnFill(portfolio, fill);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_framework_cleared(Framework *framework) {
  on_event(new OnFrameworkCleared(framework));
}

void EventServer::On_send_order(Order order) {
  on_event(new OnSendOrder(order));
}

void EventServer::on_execution_command(ExecutionCommand command) {
  on_event(command);
}

void EventServer::on_pending_new_order(Order order, bool queued) {
  auto e = new OnPendingNewOrder(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_order_status_changed(Order order, bool queued) {
  auto e = new OnOrderStatusChanged(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_new_order(Order order, bool queued) {
  auto e = new OnNewOrder(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_order_rejected(Order order, bool queued) {
  auto e = new OnOrderRejected(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_order_done(Order order, bool queued) {
  auto e = new OnOrderDone(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::OnOrderExpired(Order order, bool queued) {
  auto e = new OnOrderExpired(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_order_partially_filled(Order order, bool queued) {
  auto e = new OnOrderPartiallyFilled(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_order_filled(Order order, bool queued) {
  auto e = new OnOrderFilled(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_order_cancelled(Order order, bool queued) {
  auto e = new OnOrderCancelled(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_order_cancel_rejected(Order order, bool queued) {
  auto e = new OnOrderCancelRejected(order);
  if (queued)
    m_queue.push_back(e);
  else
    on_event(e);
}

void EventServer::on_order_replace_rejected(Order order, bool queued) {
  auto e = new OnOrderReplaceRejected(order);
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

//void EventServer::on_data(DataObject data) { on_event(data); }

void EventServer::on_account_report(AccountReport report) { on_event(report); }

void EventServer::on_execution_report(ExecutionReport report) {
  on_event(report);
}

void EventServer::on_provider_added(IProvider provider) {
  on_event(new OnProviderAdded(provider));
}

void EventServer::on_provider_removed(Provider provider) {
  on_event(new OnProviderRemoved(provider));
}

void EventServer::on_provider_connected(Provider provider) {
  on_event(new OnProviderConnected(this.framework.Clock.DateTime, provider));
}

void EventServer::on_provider_disconnected(Provider provider) {
  on_event(new OnProviderDisconnected(this.framework.Clock.DateTime, provider));
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
  on_event(new OnProviderStatusChanged(provider));
}

void EventServer::on_portfolio_parent_changed(Portfolio portfolio,
                                              bool queued) {
  if (queued)
    on_event(new OnPortfolioParentChanged(portfolio));
}

void EventServer::on_portfolio_added(Portfolio portfolio) {
  on_event(new OnPortfolioAdded(portfolio));
}

void EventServer::on_portfolio_removed(Portfolio portfolio) {
  on_event(new OnPortfolioRemoved(portfolio));
}

void EventServer::on_instrument_added(Instrument instrument) {
  on_event(new OnInstrumentAdded(instrument));
}

void EventServer::on_instrument_definition(InstrumentDefinition definition) {
  on_event(new OnInstrumentDefinition(definition));
}

void EventServer::on_instrument_defintionEnd(InstrumentDefinitionEnd end) {
  on_event(new OnInstrumentDefinitionEnd(end));
}

void EventServer::on_instrument_deleted(Instrument instrument) {
  on_event(new OnInstrumentDeleted(instrument));
}

void EventServer::on_strategy_added(Strategy strategy) {
  on_event(new OnStrategyAdded(strategy));
}

void EventServer::on_order_manager_cleared() {
  on_event(new OnOrderManagerCleared());
}
