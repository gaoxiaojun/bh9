#include "bar.h"
using namespace h9;

Bar::Bar(ptime open_time, ptime close_time, InstrumentId iid, Bar::Type type, long size, double open, double high, double low, double close, long volume, long openInt)
    : open_time(open_time), close_time(close_time), iid(iid), type(type),
      size(size), open(open), high(high), low(low), close(close),
      volume(volume), open_interest(openInt),
      status(open_time == close_time ? Bar::Status::kOpen
                                     : Bar::Status::kClose) {}

void Bar::set_time(ptime time) {
    time = time;
    close_time = time;
}
