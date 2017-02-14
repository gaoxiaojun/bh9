#include "currency.h"

#include <boost/bimap.hpp>
#include <type_traits>

namespace h9 {

typedef std::underlying_type<Currency>::type currency_type;
typedef boost::bimap<std::string, currency_type> CurrencyMap;
CurrencyMap _gCurrencyMap = {
    #define H9_CURRENCY_INIT(name, index) {#name, index},
        CURRENCY_DEF(H9_CURRENCY_INIT)
    #undef H9_CURRENCY_INIT
};

Currency currency_from_string(const std::string &name) {
  auto pos = _gCurrencyMap.left.find(name);
  if (pos == _gCurrencyMap.left.end()) {
    // not found
    return Currency::NONE;
  }
  return static_cast<Currency>(pos->second);
}

std::string currency_to_string(Currency currency) {
  static const std::string empty_string = "";
  auto pos = _gCurrencyMap.right.find(static_cast<currency_type>(currency));
  if (pos == _gCurrencyMap.right.end()) {
    // not found
    return empty_string;
  }
  return pos->second;
}

} // namespace h9
