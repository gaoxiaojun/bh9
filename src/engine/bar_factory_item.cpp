#include "bar_factory_item.h"
#include "market_events.h"
#include "bar_factory.h"
using namespace h9;

BarFactoryItem::BarFactoryItem(InstrumentId iid, Bar::Type barType, long barSize, Bar::Input barInput, ProviderId pid)
    : m_pid(pid), m_iid(iid), m_type(barType), m_input(barInput),
      m_size(barSize), m_bar(nullptr) {}

void BarFactoryItem::process(const Event::Pointer &e) {
    auto tick = event_cast<ETick>(e);
    if (m_pid != -1 && tick->provider_id() != m_pid)
        return;

    if (!in_session(tick->time()))
        return;

    on_data(e);
}

void BarFactoryItem::on_data(const Event::Pointer& e) {
    auto tick = event_cast<ETick>(e);
    if (m_bar == nullptr) {
        double price = tick->price();
        m_bar = new Bar(get_bar_open_time(e), e->time(), tick->instrument_id(),
                        m_type, m_size, price, price, price, price, tick->volume());
        m_bar->setStatus(Bar::Status::kOpen);
        m_factory.framework().event_server().on_event(m_bar);
    } else {
        if (tick->price() > m_bar->high())
            m_bar->set_high(tick->price());

        if (tick->price() < m_bar->low())
            m_bar->set_low(tick->price());

        m_bar->set_close(tick->price());
        m_bar->set_volume(m_bar->volume() + tick->size());
        m_bar->set_time(e->time()); // Q: this update EBar time, not bar
    }
}

bool BarFactoryItem::add_reminder(ptime time)
{
    m_factory->add_reminder(*this, ptime);
}

void BarFactoryItem::emit_bar()
{
    m_bar->set_status(Bar::Status::kClose);
    m_factory->framework()->event_server()->on_event(m_bar);
    m_bar = nullptr;
}

void TimeBarFactoryItem::on_data(Event::Pointer e) {
    bool barOpen = m_bar == nullptr;
    BarFactoryItem::on_data(e);
    if (barOpne) {
        add_reminder(get_bar_close_time(e), m_type);
    }
}
