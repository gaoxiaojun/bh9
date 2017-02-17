#ifndef SESSION_BAR_FACTORY_ITEM_H
#define SESSION_BAR_FACTORY_ITEM_H

#include "../bar_factory_item.h"
#include "../bar_factory.h"

namespace h9 {

class SessionBarFactoryItem : public BarFactoryItem {
protected:
    void on_event(Event::Pointer e) override
    {
        bool barOpen = m_bar == nullptr;
        BarFactoryItem::on_event(e);
        if (barOpen)
            add_reminder(get_bar_close_time(e));
    }

    virtual void on_reminder(ptime time) override
    {
        //TODO:
        emit_bar();
    }

    virtual ptime get_bar_open_time(const Event::Pointer &e) override
    {
        return get_bar_time(e).date()+m_session1;
    }

    virtual ptime get_bar_close_time(const Event::Pointer &e) override
    {
        return get_bar_time(e).date()+m_session2;
    }
};

} // namespace h9

#endif // SESSION_BAR_FACTORY_ITEM_H
