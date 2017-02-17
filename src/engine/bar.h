#ifndef BAR_H
#define BAR_H

#include "common.h"
#include "event.h"
#include "market_events.h"

namespace h9 {

// Bar or EBar { Bar }
class Bar {
public:
  enum class Type { kTime = 1, kTick, kVolume, kRange, kSession };

  enum class Status { kIncomplete, kComplete, kOpen, kHigh, kLow, kClose };

public:
  Bar(ptime open_time, ptime close_time, InstrumentId iid, Bar::Type type,
      long size, double open = 0.0, double high = 0.0, double low = 0.0,
      double close = 0.0, long volume = 0, long openInt = 0)
      : open_time(opent_time), close_time(close_time), iid(iid),
        type(type), size(size), open(open), high(high), low(low),
        close(close), vol(volume), open_interest(openInt),
        status(open_time == close_time ? Bar::Status::kOpen : Bar::Status::kClose) {
  }

public:
  ptime time() const { return close_time; }
  void set_time(ptime time) {
    time = time;
    close_time = time;
  }

public:
  double range() const { return high - low; }

  double median() const { return (high + low) / 2; }

  double typical() const { return (high + low + close) / 3; }

  double weighted() const { return (high + low + 2 * close) / 4; }

  double average() const { return (high + low + open + close) / 4; }

public:
  ptime open_time;
  ptime close_time;
  ProviderId pid;
  InstrumentId iid;
  Type type;
  long size;
  double open;
  double high;
  double low;
  double close;
  long volume;
  long open_interest;
  long N;
  Status status;
};

class EBar : public Event {
public:
    Bar bar;
};

} // namespace h9
#endif // BAR_H
