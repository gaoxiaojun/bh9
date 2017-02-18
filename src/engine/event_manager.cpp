#include "event_manager.h"
#include "system_events.h"

using namespace h9;

EventManager::EventManager(Framework *framework, EventBus *bus)
    : m_framework(framework), m_bus(bus)
    ,m_stepping(false), m_step_event(Event::Type::kEvent)
{

}

EventManager::~EventManager()
{

}

void EventManager::start()
{

}

void EventManager::stop()
{

}

void EventManager::pause()
{

}

void EventManager::pause(ptime time)
{

}

void EventManager::resume()
{

}

void EventManager::step(Event::Type type)
{
    m_stepping = true;
    m_step_event = type;
    Event::Pointer e = make_event<EOnEventManagerStep>();
    on_event(e);
}

void EventManager::clear()
{
    m_bar_factory.clear();
}

void EventManager::on_event(const Event::Pointer &e)
{

}

void EventManager::on_bid(const Event::Pointer &e)
{
    auto bid = event_cast<EBid>(e);
                //SyncLocalClockWithDataObject(bid);
                //SyncExchangeClockWithTick(bid, nameof(OnBid));
    m_bar_factory.on_tick(e);
                /*this.framework.DataManager.OnBid(bid);
                this.framework.InstrumentManager.GetById(bid.InstrumentId).Bid = bid;
                this.framework.ProviderManager.ExecutionSimulator.OnBid(bid);
                this.framework.StrategyManager.OnBid(bid);*/
}

void EventManager::on_ask(const Event::Pointer &e)
{
m_bar_factory.on_tick(e);
}

void EventManager::on_trade(const Event::Pointer &e)
{
m_bar_factory.on_tick(e);
}

void EventManager::on_queote(const Event::Pointer &e)
{

}

void EventManager::on_bar(const Event::Pointer &e)
{

}

void EventManager::on_level2snapshot(const Event::Pointer &e)
{

}

void EventManager::on_level2update(const Event::Pointer &e)
{

}

void EventManager::on_news(const Event::Pointer &e)
{

}

void EventManager::on_fundamental(const Event::Pointer &e)
{

}

void EventManager::on_fill(const Event::Pointer &e)
{

}

void EventManager::on_transaction(const Event::Pointer &e)
{

}

void EventManager::on_account_report(const Event::Pointer &e)
{

}

void EventManager::on_account_data(const Event::Pointer &e)
{

}

void EventManager::on_historical_data_end(const Event::Pointer &e)
{

}

void EventManager::on_historical(const Event::Pointer &e)
{

}

void EventManager::on_order_done(const Event::Pointer &e)
{

}

void EventManager::on_order_replace_rejected(const Event::Pointer &e)
{

}

void EventManager::on_order_cancel_rejected(const Event::Pointer &e)
{

}

void EventManager::on_order_expired(const Event::Pointer &e)
{

}

void EventManager::on_order_rejected(const Event::Pointer &e)
{

}

void EventManager::on_order_cancelled(const Event::Pointer &e)
{

}

void EventManager::on_order_replaced(const Event::Pointer &e)
{

}

void EventManager::on_order_filled(const Event::Pointer &e)
{

}

void EventManager::on_order_partially_filled(const Event::Pointer &e)
{

}

void EventManager::on_order_status_changed(const Event::Pointer &e)
{

}

void EventManager::on_new_order(const Event::Pointer &e)
{

}

void EventManager::on_send_order(const Event::Pointer &e)
{

}

void EventManager::on_command(const Event::Pointer &e)
{

}

void EventManager::on_group_event(const Event::Pointer &e)
{

}

void EventManager::on_group(const Event::Pointer &e)
{

}

void EventManager::on_execution_report(const Event::Pointer &e)
{

}

void EventManager::on_reminder(const Event::Pointer &e)
{

}

void EventManager::on_provider_error(const Event::Pointer &e)
{

}

void EventManager::on_portfolio_error(const Event::Pointer &e)
{

}

void EventManager::on_portfolio_added(const Event::Pointer &e)
{

}

void EventManager::on_portfolio_parent_changed(const Event::Pointer &e)
{

}

void EventManager::on_portfolio_removed(const Event::Pointer &e)
{

}

void EventManager::on_property_changed(const Event::Pointer &e)
{

}

void EventManager::on_bar_slice(const Event::Pointer &e)
{

}

void EventManager::on_position_opened(const Event::Pointer &e)
{

}

void EventManager::on_position_closed(const Event::Pointer &e)
{

}

void EventManager::on_position_changed(const Event::Pointer &e)
{

}

void EventManager::on_exception(const Event::Pointer &e)
{

}

void EventManager::on_pending_new_order(const Event::Pointer &e)
{

}

void EventManager::on_strategy_event(const Event::Pointer &e)
{

}

void EventManager::on_provider_connected(const Event::Pointer &e)
{

}

void EventManager::on_provider_disconnected(const Event::Pointer &e)
{

}

void EventManager::on_simulator_start(const Event::Pointer &e)
{

}

void EventManager::on_simulator_stop(const Event::Pointer &e)
{

}
