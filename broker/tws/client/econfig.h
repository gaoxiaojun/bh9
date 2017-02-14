#ifndef ECONFIG_H
#define ECONFIG_H

#include <boost/config.hpp>

#if defined(TWS_LINK_DYNAMIC)
#  if defined(TWS_LIBRARY_COMPILATION)
#    define TWS_API BOOST_SYMBOL_EXPORT
# else
#    define TWS_API BOOST_SYMBOL_IMPORT
# endif
#else
# define TWS_API
#endif

#endif // ECONFIG_H
