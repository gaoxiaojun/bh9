#include "catch.hpp"

#include <iostream>

#include "event_bus.h"
#include "market_events.h"
#include "bar_factory.h"
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>

using namespace std;
using namespace h9;

TEST_CASE("BarFactory") {
  boost::random::mt19937 gen;
  // prepare test data set
  EventBus bus;
  boost::uniform_int<> random_gen(-10, 10);
  int index = 0;
  InstrumentId iid = 1;
  for (int i = 0; i < 200; ++i) {
    int sec = index + random_gen(gen);
    Event::Pointer e =
        make_event<EAsk>(boost::posix_time::second_clock::local_time() +
                             boost::posix_time::seconds(sec),
                         ++index/*pid*/, iid, 1.0 * random_gen(gen), 100);
    bus.enqueue(std::move(e));
  }

  SECTION("BarFactory gen bar") {
    bus.set_mode(EventBus::Mode::kSimulation);
    BarFactory factory(&bus);
    factory.add(iid, Bar::Type::kTime, 60, BarInput::kAsk);
    while (true) {
      Event::Pointer e = bus.dequeue();
      if (!e)
        break;
      if (e->type() == Event::Type::kReminder) {
          auto r = event_cast<EReminder>(e);
          r->operator()();
      }
      std::cout << "type:" << e->type() << " time:" << e->time() << std::endl;
      factory.on_tick(e);
    }
    //std::cout << "qsize:" << bus.queue_size() <<" rsize:" << bus.reminder_size() << std::endl;
    REQUIRE(bus.queue_size() == 0);
  }
}
