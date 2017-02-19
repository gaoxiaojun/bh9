#ifndef PORTFOLIO_STATISTICS_ITEM_H
#define PORTFOLIO_STATISTICS_ITEM_H

#include "common.h"

namespace h9 {

class TimeSeries;
class Transaction;
class Position;
class TradeInfo;
class Portfolio;
class PortfolioStatistics;

class PortfolioStatisticsItem {
public:
  PortfolioStatisticsItem();

protected:
  double total_value;
  double long_value;
  double short_value;
  TimeSeries total_values;
  TimeSeries long_values;
  TimeSeries short_values;
  Portfolio portfolio;
  PortfolioStatistics statistics;

public:
  virtual int type() = 0;
  virtual std::string name() = 0;
  virtual std::string format() = 0;
  virtual std::string description() = 0;
  virtual std::string category() = 0;
  virtual bool Show() = 0;
  double total_value();
  double long_value();
  double short_value();

  TimeSeries TotalValues() = 0;

  TimeSeries LongValues() = 0;

  TimeSeries ShortValues() = 0;

  Clock Clock;

  void subscribe(int itemType);
  void unsubscribe(int itemType);
  void emit();


  virtual void on_init() {}
  virtual void on_equity(double equity) {}
  virtual void on_fill(Fill fill) {}
  virtual void on_transaction(Transaction transaction) {}
  virtual void on_position_opened(Position position) {}
  virtual void on_position_closed(Position position) {}
  virtual void on_position_changed(Position position) {}
  virtual void on_positionside_changed(Position position) {}
  virtual void on_roundtrip(TradeInfo trade) {}
  virtual void on_statistics(PortfolioStatisticsItem statistics) {}
  virtual void on_statistics(Portfolio portfolio,
                            PortfolioStatisticsItem statistics) {}
  virtual void on_clear() {}
};

} // namespace h9

#endif // PORTFOLIO_STATISTICS_ITEM_H
