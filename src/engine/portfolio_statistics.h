#ifndef PORTFOLIO_STATISTICS_H
#define PORTFOLIO_STATISTICS_H

#include "common.h"
#include "portfolio_statistics_item.h"
namespace h9 {

class PortfolioStatisticsCategory {
public:
  const std::string Summary = "Summary";
  const std::string Trades = "Trades";
  const std::string DailyAnnual = "Daily / Annual returns";
  const std::string UpsideDownside = "Upside / Downside returns";
};

enum class PortfolioStatisticsType {
  NetProfit = 1,
  GrossProfit = 2,
  GrossLoss = 3,
  Drawdown = 4,
  DrawdownPercent = 5,
  AvgDrawdown = 6,
  AvgDrawdownPercent = 7,
  MaxDrawdown = 8,
  MaxDrawdownPercent = 9,
  ProfitFactor = 10,
  RecoveryFactor = 11,
  NumOfTrades = 21,
  NumOfWinTrades = 22,
  NumOfLossTrades = 23,
  ProfitablePercent = 24,
  TradesPnL = 25,
  WinTradesPnL = 26,
  LossTradesPnL = 27,
  AvgTrade = 28,
  AvgWinTrade = 29,
  AvgLossTrade = 30,
  PayoffRatio = 31,
  MaxAdverseExcursion = 32,
  MaxFavorableExcursion = 33,
  EndOfTradeDrawdown = 34,
  AvgMaxAdverseExcursion = 35,
  AvgMaxFavorableExcursion = 36,
  AvgEndOfTradeDrawdown = 37,
  ConsecutiveWinTrades = 38,
  ConsecutiveLossTrades = 39,
  MaxConsecutiveWinTrades = 40,
  MaxConsecutiveLossTrades = 41,
  TradesDuration = 42,
  AvgTradesDuration = 43,
  AnnualReturn = 63,
  DailyReturnPercent = 64,
  AnnualReturnPercent = 65,
  AvgDailyReturnPercent = 66,
  AvgAnnualReturnPercent = 67,
  DailyReturnPercentStdDev = 68,
  AnnualReturnPercentStdDev = 69,
  DailyDownsideRisk = 70,
  AnnualDownsideRisk = 71,
  SharpeRatio = 72,
  SortinoRatio = 73,
  CompoundAnnualGrowthRate = 74,
  MARRatio = 75,
  ValueAtRisk = 76
};

class PortfolioStatistics {
public:
  PortfolioStatistics(Portfolio portfolio);

  void add(PortfolioStatisticsItem item);

  PortfolioStatisticsItem get(PortfolioStatisticsType type);

  void subscribe(PortfolioStatisticsItem item, PortfolioStatisticsType type);

  void unsubscribe(PortfolioStatisticsItem item, PortfolioStatisticsType type);

  void detect_trade(Fill fill);

  void on_fill(Fill fill);

  void on_transaction(Transaction transaction);
  void on_position_opened(Position position);

  void on_position_closed(Position position);

  void on_position_changed(Position position);

  void on_positionside_changed(Position position);

  void on_statistics(PortfolioStatisticsItem item);

  void on_statistics(Portfolio portfolio, PortfolioStatisticsItem item);

  void on_clear();

  void on_equity(double value);
};

} // namespace h9

#endif // PORTFOLIO_STATISTICS_H
