#ifndef BAR_H
#define BAR_H

#include "common.h"
#include "event.h"
namespace h9 {

class Bar : public Event {
public:
  enum class Type { kTime = 1, kTick, kVolume, kRange, kSession };

  enum class Status { kIncomplete, kComplete, kOpen, kHigh, kLow, kClose };

  enum class Input { kTrade, kBid, kAsk, kMiddle, kTick, kBidAsk };

  enum class Data {
    kClose,
    kOpen,
    kHigh,
    kLow,
    kMedian,
    kTypical,
    kWeighted,
    kAverage,
    kVolume,
    kOpenInt,
    kRange,
    kMean,
    kVariance,
    kStdDev
  };

public:
  Bar(ptime open_time, ptime close_time, InstrumentId iid, BarType type,
      long size, double open = 0.0, double high = 0.0, double low = 0.0,
      double close = 0.0, long volume = 0, long openInt = 0)
      : m_open_time(opent_time), m_close_time(close_time), m_iid(iid),
        m_type(type), m_size(size), m_open(open), m_high(high), m_low(low),
        m_close(close), m_vol(volume), m_open_interest(openInt),
        m_status(open_time == close_time ? BarStatus::Open : BarStatus::Close) {
  }

public:
  ptime close_time() const { return m_close_time; }

  ptime open_time() const { return m_open_time; }

  double open() const { return m_open; }

  double close() const { return m_close; }

  double high() const { return m_high; }
  void set_high(double v) { m_high = v; }

  double low() const { return m_low; }
  void set_low(double v) { m_low = v; }

  long volume() const { return m_vol; }

  long openInt() const { return m_open_interest; }

  Status status() const { return m_status; }
  void set_status(Status status) { m_status = status; }

  Type type() const { return m_type; }

  InstrumentId iid() const { return m_iid; }

  ProviderId pid() const { return m_pid; }

  ptime time() const { return m_close_time; }
  void set_time(ptime time) {
    m_time = time;
    m_close_time = time;
  }

public:
  double range() const { return m_high - m_low; }

  double median() const { return (m_high + m_low) / 2; }

  double typical() const { return (m_high + m_low + m_close) / 3; }

  double weighted() const { return (m_high + m_low + 2 * m_close) / 4; }

  double average() const { return (m_high + m_low + m_open + m_close) / 4; }

private:
  ptime m_open_time;
  ptime m_close_time;
  ProvierId m_pid;
  InstrumentId m_iid;
  Type m_type;
  long m_size;
  double m_open;
  double m_high;
  double m_low;
  double m_close;
  long m_vol;
  long m_open_interest;
  Status m_status;
};

} // namespace h9
#endif // BAR_H
