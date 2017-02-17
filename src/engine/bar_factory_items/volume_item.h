#ifndef VOLUME_BAR_FACTORY_ITEM_H
#define VOLUME_BAR_FACTORY_ITEM_H

#include "../bar_factory_item.h"
#include "../bar_factory.h"

namespace h9 {

class VolumeBarFactoryItem : public BarFactoryItem {
protected:
    void on_event(const Event::Poiner& e) override {
        BarFactoryItem::on_event(e);
        if (m_bar->volume >=  m_size)
            emit_bar();
    }
};

} // namespace h9

#endif // VOLUME_BAR_FACTORY_ITEM_H
