/*
 * As Test result: event_bus choose multi set as priority queue underlying container.
 * prority queue can't keep insert order, but multi set can!
 * why prority queue would not fast than multi set?
 * because the value_type is shared_ptr.
 * priorty queue locally advantage not adressed in this case
 */

#ifdef TEST_PERFORMANCE
#include "catch.hpp"

#include <iostream>

#include "../src/engine/event_bus.h"
#include "../src/engine/market_events.h"
#include <boost/heap/priority_queue.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <chrono>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <set>


using namespace std;
using namespace h9;

typedef std::chrono::high_resolution_clock chrono_clock_t;
typedef std::chrono::time_point<chrono_clock_t> chrono_time_t;

static int test_count = 500000;// 10000000;//50000000;
void print_perf(const chrono_time_t &start, const chrono_time_t &end,
                long count, const std::string &message) {
  auto seconds = (end - start).count() *
                 ((double)std::chrono::high_resolution_clock::period::num /
                  std::chrono::high_resolution_clock::period::den);
  std::cout << message << " " << seconds << " seconds. "
            << ((long double)count / seconds) / 1000000
            << " million per second." << std::endl;
}

TEST_CASE("performance") {

  // prepare test data set
  boost::random::mt19937 gen;
  boost::uniform_int<> random_gen(-10, 10);

  SECTION("test std priority_queue") {
    std::cout << "-------------------------------------------------------------"
                 "------------------"
              << std::endl;
    std::priority_queue<Event::Pointer, std::vector<Event::Pointer>, deref_greater<Event::Pointer>> q_;

    auto g_start = chrono_clock_t::now();

    // insert data set
    int index = 0;
    for (int i = 0; i < test_count; ++i) {
      int sec = index + random_gen(gen);
      Event::Pointer e =
          make_event<EAsk>(boost::posix_time::second_clock::local_time() +
                               boost::posix_time::seconds(sec),
                           0, ++index, 1.0, 100);
      q_.push(std::move(e));
    }
    auto insert_end = chrono_clock_t::now();

    int count = 0;

    while (!q_.empty()) {
      ++count;
      q_.top();
      q_.pop();
    }
    auto g_end = chrono_clock_t::now();
    print_perf(g_start, insert_end, test_count, "insert:");
    print_perf(insert_end, g_end, test_count, "pop:");
    print_perf(g_start, g_end, test_count, "std priority_queue");
    std::cout << "-------------------------------------------------------------"
                 "------------------"
              << std::endl;
    REQUIRE(count == test_count);
  }

  SECTION("test boost priority_queue") {
    boost::heap::priority_queue<
        Event::Pointer, boost::heap::compare<deref_greater<Event::Pointer>>>
        q_;
    auto g_start = chrono_clock_t::now();

    // insert data set
    int index = 0;
    for (int i = 0; i < test_count; ++i) {
      int sec = index + random_gen(gen);
      Event::Pointer e =
          make_event<EAsk>(boost::posix_time::second_clock::local_time() +
                               boost::posix_time::seconds(sec),
                           0, ++index, 1.0, 100);
      q_.push(std::move(e));
    }
    auto insert_end = chrono_clock_t::now();

    int count = 0;

    while (!q_.empty()) {
      ++count;
      q_.top();
      q_.pop();
    }
    auto g_end = chrono_clock_t::now();
    print_perf(g_start, insert_end, test_count, "insert:");
    print_perf(insert_end, g_end, test_count, "pop:");
    print_perf(g_start, g_end, test_count, "boost priority_queue");
    std::cout << "-------------------------------------------------------------"
                 "------------------"
              << std::endl;
    REQUIRE(count == test_count);
  }

  SECTION("test multi_index") {
      boost::multi_index_container<
            Event::Pointer,
            boost::multi_index::indexed_by<
              boost::multi_index:: ordered_non_unique<
                boost::multi_index::const_mem_fun<Event,ptime,&Event::time>
              >
            >
          >  q_;
    auto g_start = chrono_clock_t::now();

    // insert data set
    int index = 0;
    for (int i = 0; i < test_count; ++i) {
      int sec = index + random_gen(gen);
      Event::Pointer e =
          make_event<EAsk>(boost::posix_time::second_clock::local_time() +
                               boost::posix_time::seconds(sec),
                           0, ++index, 1.0, 100);
      q_.insert(std::move(e));
    }
    auto insert_end = chrono_clock_t::now();

    int count = 0;

    while (!q_.empty()) {
      ++count;
      q_.erase(q_.begin());
    }
    auto g_end = chrono_clock_t::now();
    print_perf(g_start, insert_end, test_count, "insert:");
    print_perf(insert_end, g_end, test_count, "pop:");
    print_perf(g_start, g_end, test_count, "boost multi_index");
    std::cout << "-------------------------------------------------------------"
                 "------------------"
              << std::endl;
    REQUIRE(count == test_count);
  }

  SECTION("test std multi set") {
      std::multiset<Event::Pointer,deref_greater<Event::Pointer>> q_;
    auto g_start = chrono_clock_t::now();

    // insert data set
    int index = 0;
    for (int i = 0; i < test_count; ++i) {
      int sec = index + random_gen(gen);
      Event::Pointer e =
          make_event<EAsk>(boost::posix_time::second_clock::local_time() +
                               boost::posix_time::seconds(sec),
                           0, ++index, 1.0, 100);
      q_.insert(std::move(e));
    }
    auto insert_end = chrono_clock_t::now();

    int count = 0;

    while (!q_.empty()) {
      ++count;
      q_.erase(q_.begin());
    }
    auto g_end = chrono_clock_t::now();
    print_perf(g_start, insert_end, test_count, "insert:");
    print_perf(insert_end, g_end, test_count, "pop:");
    print_perf(g_start, g_end, test_count, "boost multi set");
    std::cout << "-------------------------------------------------------------"
                 "------------------"
              << std::endl;
    REQUIRE(count == test_count);
  }

  SECTION("test std multi insert order") {
      std::multiset<Event::Pointer,deref_greater<Event::Pointer>> q_;
    auto g_start = chrono_clock_t::now();

    // insert data set
    int index = 0;
    for (int i = 0; i < test_count; ++i) {
      int sec = index + random_gen(gen);
      Event::Pointer e =
          make_event<EAsk>(boost::posix_time::second_clock::local_time() +
                               boost::posix_time::seconds(sec),
                           0, ++index, 1.0, 100);
      q_.insert(std::move(e));
    }
    auto insert_end = chrono_clock_t::now();

    int count = 0;

    while (!q_.empty()) {
      ++count;
      q_.erase(q_.begin());
    }
    auto g_end = chrono_clock_t::now();
    print_perf(g_start, insert_end, test_count, "insert:");
    print_perf(insert_end, g_end, test_count, "pop:");
    print_perf(g_start, g_end, test_count, "boost multi set");
    std::cout << "-------------------------------------------------------------"
                 "------------------"
              << std::endl;
    REQUIRE(count == test_count);
  }
}
#endif
