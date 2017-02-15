#ifndef BAR_FACTORY_H
#define BAR_FACTORY_H

#include "bar.h"
#include "event.h"

namespace h9 {

class BarFactory;

class BarFactoryItem {
public:
protected:
  void process(Event::Pointer e);
  virtual void on_data(Event::Pointer);
  virtual void on_reminder(ptime time);
  virtual ptime get_bar_open_time(Event::Pointer e);
  virtual ptime get_bar_close_time(Event::Pointer e);
  virtual ptime get_time(Event::Pointer e);
  bool add_reminder(ptime time);
  void emit_bar();

private:
  BarFactory &m_bar_factory;
  ProviderId m_pid;
  InstrumentId m_iid;
  Bar::Type m_type;
  Bar::Input m_input;
  long m_bar_size;
  bool m_session_enable;
};

class BarFactory {
public:
  explicit BarFactory();

public:
  void add(const BarFactoryItem &item);
  void remove(const BarFactoryItem &item);
  void clear();

protected:
  void on_data(const Event::Pointer &e);
  bool add_reminder(const BarFactoryItem &item, ptime time);
  void on_reminder(ptime time);
};

} // namespace h9
#endif // BAR_FACTORY_H
