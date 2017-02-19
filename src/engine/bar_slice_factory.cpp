#include "bar_slice_factory.h"
using namespace h9;


void BarSliceFactory::on_bar(const Bar &bar)
{
    auto it = m_items.find(bar.size);
    if (it == m_items.end())
        return;

    auto& item = it->second;
    if (--item.m_bar_count == 0) {
        //m_framework->event_server().on_event(std::make_shared<EBarSlice>(bar));
        item.m_close_time = min_date_time;
        //for(auto& bar : item.m_bars)
        //    m_framework->event_server().on_event(b);

        item.m_bars.clear();
    }
}

bool BarSliceFactory::on_bar_open(const Bar &bar)
{
    auto& item = m_items[bar.size];
    if (item.m_close_time == bar.open_time) {
        item.m_bars.push_back(bar);
        return false;
    }
    if (item.m_bar_count == 0)
        item.m_close_time = bar.open_time + boost::posix_time::seconds(bar.size);

    ++item.m_bar_count;
    return true;
}
