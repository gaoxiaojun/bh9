#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <boost/exception/all.hpp>

namespace h9 {

class exception : public std::exception, public boost::exception
{

};

} // namespace h9

#endif // EXCEPTION_H
