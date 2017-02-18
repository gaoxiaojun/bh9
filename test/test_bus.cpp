#include "catch.hpp"

#include <iostream>

#include "event_bus.h"
#include "market_events.h"
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>

using namespace std;
using namespace h9;

TEST_CASE("EventBus") {
  boost::random::mt19937 gen;
  // prepare test data set
  EventBus bus;
  boost::uniform_int<> random_gen(-10, 10);
  int index = 0;
  for (int i = 0; i < 10000; ++i) {
    int sec = index + random_gen(gen);
    Event::Pointer e =
        make_event<EAsk>(boost::posix_time::second_clock::local_time() +
                             boost::posix_time::seconds(sec),
                         0, ++index, 1.0, 100);
    bus.enqueue(std::move(e));
  }

  SECTION("realtime mode dequeue") {
    bus.set_mode(EventBus::Mode::kRealtime);
    int count = 0;

    for (int i = 0; i < 100; ++i) {
      auto r = make_event<EReminder>(
          boost::posix_time::second_clock::local_time(), nullptr);
      bus.enqueue(r);
    }

    while (true) {
      Event::Pointer e = bus.dequeue();
      if (!e)
        break;
      count = count + 1;
      if (e->type() == Event::Type::kAsk) {
        auto ask = event_cast<EAsk>(e);
        // std::cout << "type:" << ask->type() << " time:" << ask->time()
        //          << " index:" << ask->instrument_id() << std::endl;
      } // else
      // std::cout << "type:" << e->type() << " time:" << e->time() <<
      // std::endl;
    }
    // std::cout << "count: " << count << std::endl;
    REQUIRE(count == 10000 + 100);
    REQUIRE(bus.empty());
  }

  SECTION("simulation mode dequeue") {
    bus.set_mode(EventBus::Mode::kSimulation);
    int count = 0;

    for (int i = 0; i < 100; ++i) {
      auto r =
          make_event<EReminder>(boost::posix_time::second_clock::local_time() +
                                    boost::posix_time::seconds(i * 10),
                                nullptr);
      bus.enqueue(r);
    }

    while (true) {
      Event::Pointer e = bus.dequeue();
      if (!e)
        break;
      count = count + 1;
      if (e->type() == Event::Type::kAsk) {
        auto ask = event_cast<EAsk>(e);
        // std::cout << "type:" << ask->type() << " time:" << ask->time()
        //          << " index:" << ask->instrument_id() << std::endl;
      } // else
      // std::cout << "type:" << e->type() << " time:" << e->time() <<
      // std::endl;
    }
    // std::cout << "count: " << count << std::endl;
    REQUIRE(count == 10000 + 100);
    REQUIRE(bus.empty());
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
      // std::cout << "type:" << ask->type() << " time:" << ask->time()
      //          << " index:" << ask->instrument_id() << std::endl;
      if (last_time == ask->time()) {
        REQUIRE(ask->instrument_id() > last_index);
      }

      last_index = ask->instrument_id();
      last_time = ask->time();
    }
    // std::cout << "count: " << count << std::endl;
    REQUIRE(count == 10000);
  }
}
