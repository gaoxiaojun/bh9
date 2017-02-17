#include "time_item.h"

using namespace h9;


void TimeBarFactoryItem::on_event(Event::Pointer e) {
  bool barOpen = m_bar == nullptr;
  BarFactoryItem::on_event(e);
  if (barOpne) {
    add_reminder(get_bar_close_time(e));
  }
}

void TimeBarFactoryItem::on_reminder(ptime time)
{
    emit_bar();
}

ptime TimeBarFactoryItem::get_bar_close_time(const Event::Pointer &e)
{
    return get_bar_open_time(e) += boost::posix_time::seconds(m_size);
}
