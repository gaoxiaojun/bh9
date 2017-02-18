#ifndef UTIL_H
#define UTIL_H

#include "bar.h"
#include "bar_factory_item.h"

namespace h9 {

std::string bar_type_to_string(Bar::Type type);
std::string bar_input_to_string(BarInput input);
}

#endif // UTIL_H
