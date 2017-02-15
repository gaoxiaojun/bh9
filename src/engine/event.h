#ifndef EVENT_H
#define EVENT_H

#include "common.h"
#include <boost/operators.hpp>
#include <memory>

namespace h9
{

class Event : public boost::less_than_comparable<Event>
{
public:
  typedef std::shared_ptr<Event> Pointer;

  enum Type : std::uint16_t
  {
    kEvent = 0,
    kTick = 1,
    kBid = 2,
    kAsk = 3,
    kTrade = 4,
    kQuote = 5,
    kBar = 6,
    kLevel2Snapshot = 8,
    kLevel2Update = 9,
    kNews = 11,

    kExecutionReport = 13,
    kExecutionCommand = 14,
    kReminder = 15,
    kStrategyEvent = 16,
    kText = 17,
    kDataSeries = 18,
    kFieldList = 19,
    kStrategyStatus = 20,
    kProviderError = 21,

    kPosition = 25,
    kPortfolio = 26,
    kOutput = 27,
    kGroup = 50,
    kGroupUpdate = 51,
    kGroupEvent = 52,
    kMessage = 53,
    kCommand = 54,
    kResponse = 55,
    kResponseEvent = 60,
    kOnFrameworkCleared = 99,
    kOnPositionOpened = 110,
    kOnPositionClosed = 111,
    kOnPositionChanged = 112,
    kOnFill = 113,
    kOnTransaction = 114,
    kOnExecutionCommand = 115,
    kOnExecutionReport = 116,
    kOnSendOrder = 117,
    kOnPendingNewOrder = 118,
    kOnNewOrder = 119,
    kOnOrderStatusChanged = 120,
    kOnOrderPartiallyFilled = 121,
    kOnOrderFilled = 122,
    kOnOrderReplaced = 123,
    kOnOrderCancelled = 124,
    kOnOrderRejected = 125,
    kOnOrderExpired = 126,
    kOnOrderCancelRejected = 127,
    kOnOrderReplaceRejected = 128,
    kOnOrderDone = 129,
    kOnOrderManagerCleared = 130,
    kOnInstrumentDefinition = 131,
    kOnInstrumentDefintionEnd = 132,
    kOnHistoricalData = 133,
    kOnHistoricalDataEnd = 134,
    kOnPortfolioAdded = 135,
    kOnPortfolioRemoved = 136,
    kOnPortfolioParentChanged = 137,
    kHistoricalData = 138,
    kHistoricalDataEnd = 139,
    kBarSlice = 140,
    kOnStrategyEvent = 141,
    kAccountData = 142,
    kAccountTransaction = 143,
    kOnStrategyAdded = 144,
    kOnPropertyChanged = 145,
    kUser = 146,

    kOnException = 161,
    kAccountReport = 162,
    kOnConnect = 201,
    kOnDisconnect = 202,
    kOnSubscribe = 203,
    kOnUnsubscribe = 204,

    kOnEventManagerStarted = 207,
    kOnEventManagerStopped = 208,
    kOnEventManagerPaused = 209,
    kOnEventManagerResumed = 210,
    kOnEventManagerStep = 211,
    kOnUserCommand = 212,

    kOnLogin = 215,
    kOnLogout = 216,
    kOnLoggedIn = 217,
    kOnLoggedOut = 218,
    kOnLoginRejected = 219,
    kOnHeartbeat = 220,
    kOnInstrumentAdded = 221,
    kOnInstrumentDeleted = 222,
    kOnProviderAdded = 223,
    kOnProviderRemoved = 224,
    kOnProviderConnected = 225,
    kOnProviderDisconnected = 226,
    kOnProviderStatusChanged = 227,
    kOnSimulatorStart = 228,
    kOnSimulatorStop = 229,
    kOnSimulatorProgress = 230,
  };

public:
  Event(Type type, ptime time) : m_time(time), m_type(type) {}
  virtual ~Event() {}

public:
  inline Type type() const { return m_type; }

  inline ptime time() const { return m_time; }

  friend inline bool operator<(const Event &lhs, const Event &rhs)
  {
    return lhs.m_time < rhs.m_time;
  }

private:
  ptime m_time;
  Type m_type;
};

template <class T, class... Ts>
Event::Pointer
make_event(Ts&&... xs)
{
  return std::static_pointer_cast<Event>(std::make_shared<T>(std::forward<Ts>(xs)...));
}

} // namespace h9

#endif // EVENT_H
