#include "bar_factory_item.h"

using namespace h9;

BarFactoryItem::BarFactoryItem(InstrumentId iid, Bar::Type barType, long barSize, Bar::Input barInput, ProviderId pid)
    : m_pid(pid), m_iid(iid), m_type(barType), m_input(barInput),
      m_size(barSize), m_bar(nullptr) {}

void BarFactoryItem::process(const Event::Pointer &e) {
    auto tick = event_cast<ETick>(e);
    if (m_pid != -1 && tick->pid() != m_pid)
        return;

    if (!in_session(tick->time()))
        return;

    on_data(e);
}

void BarFactoryItem::on_data(Event::Pointer e) {
    auto tick = event_cast<ETick>(e);
    if (m_bar == nullptr) {
        doubel price = tick->price();
        m_bar = new Bar(get_open_time(e), get_event_time(e, Clock::kLocal), tick->iid(),
                        m_type, m_size, price, price, price, price, tick->size());
        m_bar->setStatus(Bar::Status::kOpen);
        m_factory.framework().event_server().on_event(m_bar);
    } else {
        if (tick->price() > m_bar->high())
            m_bar->set_high(tick->price());

        if (tick->price() < m_bar->low())
            m_bar->set_low(tick->price());

        m_bar->set_close(tick->price());
        m_bar->set_volume(m_bar->volume() + tick->size());
        m_bar->set_time(get_event_time(e, Clock::Type::kLocal)); // Q: this update EBar time, not bar
    }
}

ptime BarFactoryItem::get_event_time(Event::Pointer e, Clock::Type type)
{
    ETick *tick = static_cast<ETick*>(e.get());
    return type == Clock::Type::kLocal ? tick->time() : tick->exchange_time();
}

bool BarFactoryItem::add_reminder(ptime time, Clock::Type type)
{
    m_factory->add_reminder(*this, ptime, type);
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
