#include "bar_factory_item.h"
#include "bar_factory.h"
#include "market_events.h"
#include <memory>

using namespace h9;

BarFactoryItem::BarFactoryItem(InstrumentId iid, Bar::Type barType,
                               long barSize, BarInput barInput, ProviderId pid)
    : m_pid(pid), m_iid(iid), m_type(barType), m_input(barInput),
      m_size(barSize), m_session_enable(false), m_bar(nullptr) {}

BarFactoryItem(InstrumentId iid, Bar::Type barType, long barSize,
               Bar::Input barInput, time_duration session1,
               time_duration session2, ProviderId pid = -1)
    : m_pid(pid), m_iid(iid), m_type(barType), m_input(barInput),
      m_size(barSize), m_session_enable(true), m_session1(session1),
      m_session2(session2), m_bar(nullptr) {}

void BarFactoryItem::process(const Event::Pointer &e) {
  auto tick = event_cast<ETick>(e);
  if (m_pid != -1 && tick->provider_id() != m_pid)
    return;

  if (!in_session(tick->time()))
    return;

  on_event(e);
}

void BarFactoryItem::on_event(const Event::Pointer &e) {
  auto tick = event_cast<ETick>(e);
  if (!m_bar) {
    double price = tick->price();
    m_bar = std::make_shared<Bar>(get_bar_open_time(e), get_bar_time(e),
                                  tick->instrument_id(), m_type, m_size, price,
                                  price, price, price, tick->volume());
    m_bar->status = Bar::Status::kOpen;
    m_factory.framework().event_server().on_event(m_bar);
  } else {
    if (tick->price() > m_bar->high)
      m_bar->high = tick->price();

    if (tick->price() < m_bar->low)
      m_bar->low = tick->price();

    m_bar->close = tick->price();
    m_bar->volume += tick->size();
    m_bar->set_time(get_bar_time(e)); // Q: this update EBar time, not bar
    ++m_bar->N;
  }
}

bool BarFactoryItem::in_session(ptime time) {
  if (m_session_enbale) {
    auto td = time.time_of_day();
    if (td < m_session1 || td > m_session2)
      return false;
  }
  return true;
}

bool BarFactoryItem::add_reminder(ptime time) {
  m_factory->add_reminder(*this, ptime);
}

void BarFactoryItem::emit_bar() {
  m_bar->status = Bar::Status::kClose;
  // m_factory->framework()->event_server()->on_event(m_bar);
  m_bar.reset();
}

