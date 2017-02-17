#ifndef BAR_FACTORY_ITEM_H
#define BAR_FACTORY_ITEM_H

#include "bar2.h"
#include "common.h"
#include "event.h"

namespace h9 {

class BarFactory;

enum class BarInput { kTrade, kBid, kAsk, kMiddle, kTick, kBidAsk };

class BarFactoryItem {

public:
  BarFactoryItem(InstrumentId iid, Bar::Type barType, long barSize,
                 BarInput barInput = BarInput::kTrade, ProviderId pid = -1);

  virtual ~BarFactoryItem();

public:
  ProviderId provider_id() const { return m_pid; }
  InstrumentId instrument_id() const { return m_iid; }

  const BarFactory *factory() const { return m_factory; }
  void set_factory(const BarFactory *f) const { m_factory = f; }

protected:
  void process(const Event::Pointer &e);

  virtual void on_event(const Event::Pointer &e);

  virtual void on_reminder(ptime time) {}
  virtual ptime get_bar_open_time(const Event::Pointer &e) { return e->time(); }
  virtual ptime get_bar_time(const Event::Pointer&e) { return e->time(); }
  virtual ptime get_bar_close_time(const Event::Pointer &e) {
    return e->time();
  }
  virtual bool in_session(ptime time);

  bool add_reminder(ptime time);

  void emit_bar();

  friend inline bool operator==(const BarFactoryItem &lhs,
                                const BarFactoryItem &rhs) const {
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
  time_duration m_session1;
  time_duration m_session2;
  std::shared_ptr<Bar> m_bar;
};

} // namespace h9

#endif // BAR_FACTORY_ITEM_H
