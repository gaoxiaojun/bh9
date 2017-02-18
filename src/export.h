#ifndef EXPORT_H
#define EXPORT_H

#include <boost/config.hpp>

#if defined(H9_LINK_DYNAMIC)
#  if defined(H9_LIBRARY_COMPILATION)
#    define H9_API BOOST_SYMBOL_EXPORT
# else
#    define H9_API BOOST_SYMBOL_IMPORT
# endif
#else
# define H9_API
#endif

#endif // EXPORT_H
