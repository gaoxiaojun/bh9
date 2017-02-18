#ifndef DATE_TIME_H
#define DATE_TIME_H

#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>
#include <type_traits>
#include <boost/assert.hpp>

#include "datetime.h"
#include "../export.h"

namespace h9
{
using noncopyable = boost::noncopyable;

using ProviderId = std::int16_t;
using InstrumentId = std::int32_t;

} // namespace h9

#endif // DATE_TIME_H
