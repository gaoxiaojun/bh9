#ifndef DATETIME_H
#define DATETIME_H

/**
 *  IMPORTANT: in h9 framework *time* means local timezone time
 */

#define BOOST_DATE_TIME_NO_LIB
#include <boost/date_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/functional/hash.hpp>

namespace h9 {

using ptime = boost::posix_time::ptime;
using gdate = boost::gregorian::date;
using date_duration = boost::gregorian::date_duration;
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

} // namespace h9

namespace std
{
    template<>
    class hash<boost::posix_time::ptime>
    {
    public:
        size_t operator()(const boost::posix_time::ptime& t) const
        {
            std::size_t seed = 0;
            boost::hash_combine(seed, t.date().julian_day());
            boost::hash_combine(seed, t.time_of_day().ticks());
            return seed;
        }
    };
}

#endif // DATETIME_H
