#ifndef DATE_TIME_H
#define DATE_TIME_H

/**
 *  IMPORTANT: in h9 framework time means local timezone time
 */

#include <boost/date_time.hpp>
#include <boost/noncopyable.hpp>
#include <queue>

namespace h9
{

using ptime = boost::posix_time::ptime;
using time_duration = boost::posix_time::time_duration;
using time_period = boost::posix_time::time_period;
using time_zone_ptr = boost::local_time::time_zone_ptr;
using boost::date_time::special_values;
using boost::date_time::not_special;
using boost::date_time::neg_infin;
using boost::date_time::pos_infin;
using boost::date_time::not_a_date_time;
using boost::date_time::max_date_time;
using boost::date_time::min_date_time;
using clock = boost::posix_time::microsec_clock;

using noncopyable = boost::noncopyable;

template <typename T>
struct deref_greater
{
  bool operator()(const T &lhs, const T &rhs) { return *lhs > *rhs; }
};

template <typename T>
struct deref_less
{
  bool operator()(const T &lhs, const T &rhs) { return *lhs < *rhs; }
};

template <typename T>
using min_priority_queue = std::priority_queue<T, std::vector<T>, deref_greater<T>>;


} // namespace h9

#endif // DATE_TIME_H
