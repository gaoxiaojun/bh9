#include "time_item.h"

using namespace h9;


void TimeBarFactoryItem::on_tick(const Event::Pointer &e) {
  bool barOpen = m_bar == nullptr;
  BarFactoryItem::on_tick(e);
  std::cout << "timebar on_tick:" << barOpen << " closetime:" << get_bar_close_time(e) << " size:" << m_size << std::endl;
  if (barOpen) {
    add_reminder(get_bar_close_time(e));
  }
}

void TimeBarFactoryItem::on_reminder(ptime time)
{
    std::cout << "emit bar" << std::endl;
    emit_bar();
}

ptime TimeBarFactoryItem::get_bar_close_time(const Event::Pointer &e)
{
    return get_bar_open_time(e) + boost::posix_time::seconds(m_size);
}
