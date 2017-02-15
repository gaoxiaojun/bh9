#include "catch.hpp"

#include <iostream>

#include "../src/engine/event_bus.h"
#include "../src/engine/market_events.h"
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
boost::random::mt19937 gen;

using namespace std;
using namespace h9;

void time_cb(ptime time) { std::cout << "time:" << time << std::endl; }

TEST_CASE("Test EventBus") {

  // prepare test data set
  EventBus bus;
  boost::uniform_int<> random_gen(-10, 10);
  int index = 0;
  for (int i = 0; i < 10000; ++i) {
    int sec = index + random_gen(gen);
    Event::Pointer e = make_event<EAsk>(
        boost::posix_time::second_clock::local_time() +
            boost::posix_time::seconds(sec),
        boost::posix_time::second_clock::local_time(), 0, ++index, 1.0, 100);
    bus.enqueue(std::move(e));
  }

  SECTION("realtime mode dequeue") {
    bus.set_mode(EventBus::Mode::kRealtime);
    int count = 0;

    for (int i = 0; i < 100; ++i) {
      bus.add_timer(boost::posix_time::second_clock::local_time(), time_cb);
    }

    while (true) {
      Event::Pointer e = bus.dequeue();
      if (!e)
        break;
      count = count + 1;
      if (e->type() == Event::Type::kAsk) {
        auto ask = event_cast<EAsk>(e);
        std::cout << "type:" << ask->type() << " time:" << ask->time()
                  << " index:" << ask->instrument_id() << std::endl;
      } else
        std::cout << "type:" << e->type() << " time:" << e->time() << std::endl;
    }
    std::cout << "count: " << count << std::endl;
    REQUIRE(count == 10000 + 100);
  }

  SECTION("simulation mode dequeue") {
    bus.set_mode(EventBus::Mode::kSimulation);
    int count = 0;

    for (int i = 0; i < 100; ++i) {
      bus.add_timer(boost::posix_time::second_clock::local_time() +
                        boost::posix_time::seconds(i * 10),
                    time_cb);
    }

    while (true) {
      Event::Pointer e = bus.dequeue();
      if (!e)
        break;
      count = count + 1;
      if (e->type() == Event::Type::kAsk) {
        auto ask = event_cast<EAsk>(e);
        std::cout << "type:" << ask->type() << " time:" << ask->time()
                  << " index:" << ask->instrument_id() << std::endl;
      } else
        std::cout << "type:" << e->type() << " time:" << e->time() << std::endl;
    }
    std::cout << "count: " << count << std::endl;
    REQUIRE(count == 10000 + 100);
  }

  SECTION("test enqueue insert order") {
    bus.set_mode(EventBus::Mode::kSimulation);
    int count = 0;
    ptime last_time = min_date_time;
    InstrumentId last_index = 0;
    while (true) {
      Event::Pointer e = bus.dequeue();
      if (!e)
        break;
      count = count + 1;
      auto ask = event_cast<EAsk>(e);
      std::cout << "type:" << ask->type() << " time:" << ask->time()
                << " index:" << ask->instrument_id() << std::endl;
      if (last_time == ask->time()) {
        REQUIRE(ask->instrument_id() > last_index);
      }

      last_index = ask->instrument_id();
      last_time = ask->time();
    }
    std::cout << "count: " << count << std::endl;
    REQUIRE(count == 10000);
  }
}
