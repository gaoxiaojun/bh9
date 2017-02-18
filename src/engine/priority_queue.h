#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <set>

namespace h9 {

template <typename T>
struct deref_greater
{
  bool operator()(const T &lhs, const T &rhs) const { return *lhs > *rhs; }
};

template <typename T>
struct deref_less
{
  bool operator()(const T &lhs, const T &rhs) const { return *lhs < *rhs; }
};

//template <class T>
//using min_priority_queue = std::priority_queue<T, std::vector<T>, deref_greater<T>>;
template<typename T>
using min_priority_queue =  std::multiset<T ,deref_less<T>>;

} // namespace h9

#endif //PRIORITY_QUEUE_H
