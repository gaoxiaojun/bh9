#ifndef REPORT_ITEM_H
#define REPORT_ITEM_H

#include "common.h"

namespace h9 {

class ReportItem {
protected:
  int id;
  std::string name;
  std::tring description;
public:
  virtual void OnBid(Bid bid) = 0;
  virtual void OnAsk(Ask Ask) = 0;
  virtual void OnTrade(Trade trade) = 0;
  virtual void OnBar(Bar bar) = 0;
  virtual void OnExecutionReport(ExecutionReport report) = 0;
  virtual void Clear() = 0;
};

} // namespace h9

#endif // REPORT_ITEM_H
