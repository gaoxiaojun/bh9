#ifndef TICK_BAR_FACTORY_ITEM_H
#define TICK_BAR_FACTORY_ITEM_H

#include "../bar_factory_item.h"
#include "../bar_factory.h"
#include "../bar.h"

namespace h9 {

class TickBarFactoryItem : public BarFactoryItem {
protected:
    void on_event(const Event::Poiner& e) override {
        BarFactoryItem::on_event(e);
        if (m_bar->N >=  m_size)
            emit_bar();
    }
};

} // namespace h9

#endif // TICK_BAR_FACTORY_ITEM_H
