#ifndef BAR_FACTORY_ITEM_H
#define BAR_FACTORY_ITEM_H

#include "event.h"

namespace h9 {

class BarFactory;

class BarFactoryItem {
public:
  BarFactoryItem(InstrumentId iid, Bar::Type barType, long barSize,
                 Bar::Input barInput = Bar::Input::Trade, ProviderId pid = -1);

  virtual ~BarFactoryItem();

public:
  ProviderId provider_id() const { return m_pid; }
  InstrumentId instrument_id() const { return m_iid; }

  const BarFactory *factory() const { return m_factory; }
  void set_factory(const BarFactory *f) const { m_factory = f; }

protected:
  void process(const Event::Pointer &e);

  virtual void on_data(const Event::Pointer &e);

  virtual void on_reminder(ptime time) {}
  virtual ptime get_bar_open_time(Event::Pointer e) { return e->time(); }
  virtual ptime get_bar_close_time(Event::Pointer e) { return e->time(); }

  bool add_reminder(ptime time);

  void emit_bar();

  friend inline bool operator==(const BarFactoryItem &lhs,
                                const BarFactoryItem &rhs) {
    return lhs.m_type == rhs.m_type && lhs.m_size == rhs.m_size &&
           lhs.m_input == rhs.m_input && lhs.m_pid == rhs.m_pid;
  }

private:
  mutable BarFactory *m_factory;
  ProviderId m_pid;
  InstrumentId m_iid;
  Bar::Type m_type;
  Bar::Input m_input;
  long m_size;
  bool m_session_enable;
  Bar *m_bar;
};

class TimeBarFactoryItem : public BarFactoryItem {
public:
  TimeBarFactoryItem(InstrumentId iid, long barSize, Bar::Input input,
                     Provider pid = -1)
      : BarFactoryItem(iid, Bar::Type::Time, barSize, input, pid) {}

protected:
  void on_data(Event::Pointer e) override;
};

class TickBarFactoryItem : public BarFactoryItem {};

class RangeBarFactoryItem : public BarFactoryItem {};

class VolumeBarFactoryItem : public BarFactoryItem {};

class SessionBarFactoryItem : public BarFactoryItem {};

} // namespace h9

#endif // BAR_FACTORY_ITEM_H
