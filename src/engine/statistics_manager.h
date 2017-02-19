#ifndef STATISTICS_MANAGER_H
#define STATISTICS_MANAGER_H

#include "common.h"
#include "portfolio_statistics.h"
#include "portfolio_statistics_item.h"

namespace h9 {

class Framework;

class StatisticsManager {
public:
  explicit StatisticsManager(Framework *framework);

public:
  void add(PortfolioStatisticsItem item);
  bool contains(PortfolioStatisticsType type);
  void remove(PortfolioStatisticsType type);
  PortfolioStatisticsItem get(PortfolioStatisticsType type);
  PortfolioStatisticsItem clone(PortfolioStatisticsType type);
  std::vector<PortfolioStatisticsItem> clone_all();

private:
  Framework *m_framework;
};

} // namespace h9

#endif // STATISTICS_MANAGER_H
