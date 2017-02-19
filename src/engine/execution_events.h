#ifndef ORDER_EVENTS_H
#define ORDER_EVENTS_H

#include "event.h"

namespace h9 {
class Order;
class Portfolio;
class Fill;
class Strategy;
class Transaction;
class Subscription;
class Instrument;
class Position;
class InstrumentDefinition;

class EOnException : public Event {
public:
  Event event;
  std::exception exception;
  std::string source;

  EOnException(std::string source, Event ev, Exception e)
      : Event(Event::Type::kOnException), event(ev), exception(e),
        source(source) {}
};

class EOnConnect : public Event {
public:
  EOnConnect() : Event(Event::Type::kOnConnect) {}
};

class EOnDisconnect : public Event {
public:
  EOnDisconnect() : Event(Event::Type::kOnDisconnect) {}
};

class EOnSubscribe : public Event {
public:
  std::string symbol;

  std::shared_ptr<Subscription> subscription;

  std::shared_ptr<Instrument> instrument;

  std::vector<Instrument> Instruments;

  ptime ptime1{min_date_time};

  ptime ptime2{max_date_time};

  EOnSubscribe(const std::shared_ptr<Subscription> &subscription)
      : Event(Event::Type::kOnSubscribe), subscription(subscription),
        symbol(subscription->symbol()) {}

  EOnSubscribe(const std::string &symbol)
      : Event(Event::Type::kOnSubscribe), symbol(symbol) {}

  EOnSubscribe(const std::vector<Instrument> &instruments)
      : Event(Event::Type::kOnSubscribe) {
    Instruments = instruments;
  }
};

class EOnUnsubscribe : public Event {
public:
  std::string symbol;

  std::shared_ptr<Subscription> subscription;

  std::shared_ptr<Instrument> instrument;

  std::vector<Instrument> Instruments;

  EOnUnsubscribe(const std::shared_ptr<Subscription> &subscription)
      : Event(Event::Type::kOnUnsubscribe), subscription(subscription),
        symbol(subscription->symbol()) {}

  EOnUnsubscribe(const std::string &symbol)
      : Event(Event::Type::kOnUnsubscribe), symbol(symbol) {}

  EOnUnsubscribe(const std::vector<Instrument> &instruments)
      : Event(Event::Type::kOnUnsubscribe) {
    Instruments = instruments;
  }
};

class EOnPositionOpened : public Event {
public:
  std::shared_ptr<Portfolio> portfolio;
  std::shared_ptr<Position> position;

  EOnPositionOpened(const std::shared_ptr<Portfolio> &portfolio,
                    const std::shared_ptr<Position> &position)
      : Event(Event::Type::kOnPositionOpened), portfolio(portfolio),
        position(position) {}
};

class EOnPositionClosed : public Event {
public:
  std::shared_ptr<Portfolio> portfolio;
  std::shared_ptr<Position> position;
  EOnPositionClosed(const std::shared_ptr<Portfolio> &portfolio,
                    const std::shared_ptr<Position> &position)
      : Event(Event::Type::kOnPositionClosed), portfolio(portfolio),
        position(position) {}
};

class EOnPositionChanged : public Event {
public:
  std::shared_ptr<Portfolio> portfolio;
  std::shared_ptr<Position> position;

  EOnPositionChanged(const std::shared_ptr<Portfolio> &portfolio,
                     const std::shared_ptr<Position> &position)
      : Event(Event::Type::kOnPositionChanged), portfolio(portfolio),
        position(position) {}
};

class EOnTransaction : public Event {
public:
  std::shared_ptr<Transaction> transaction;

  std::shared_ptr<Portfolio> portfolio;

  EOnTransaction(const std::shared_ptr<Portfolio> &portfolio,
                 const std::shared_ptr<Transaction> &transaction)
      : Event(Event::Type::kOnTransaction), transaction(transaction),
        portfolio(portfolio) {}
};

class EOnStrategyAdded : public Event {
public:
  std::shared_ptr<Strategy> strategy;

  EOnStrategyAdded(const std::shared_ptr<Strategy> &strategy)
      : Event(Event::Type::kOnStrategyAdded), strategy(strategy) {}
};

class EOnStrategyEvent : public Event {
public:
  Event::Pointer e;

  EOnStrategyEvent(const Event::Pointer &e)
      : Event(Event::Type::kOnStrategyEvent), e(e) {}
};

class EOnInstrumentAdded : public Event {
public:
  std::shared_ptr<Instrument> instrument;

  EOnInstrumentAdded(const std::shared_ptr<Instrument> &instrument)
      : Event(Event::Type::kOnInstrumentAdded), instrument(instrument) {}
};

class EOnInstrumentDeleted : public Event {
public:
  std::shared_ptr<Instrument> instrument;

  EOnInstrumentDeleted(const std::shared_ptr<Instrument> &instrument)
      : Event(Event::Type::kOnInstrumentDeleted), instrument(instrument) {}
};

class EOnInstrumentDefinition : public Event {
public:
  std::shared_ptr<InstrumentDefinition> definition;

  EOnInstrumentDefinition(
      const std::shared_ptr<InstrumentDefinition> &definition)
      : Event(Event::Type::kOnInstrumentDefinition), definition(definition) {}
};

class EOnInstrumentDefinitionEnd : public Event {
public:
  std::shared_ptr<InstrumentDefinitionEnd> end;

  EOnInstrumentDefinitionEnd(
      const std::shared_ptr<InstrumentDefinitionEnd> &end)
      : Event(Event::Type::kOnInstrumentDefintionEnd), end(end) {}
};

class EOnPortfolioAdded : public Event {
public:
  std::shared_ptr<Portfolio> portfolio;

  EOnPortfolioAdded(const std::shared_ptr<Portfolio> &portfolio)
      : Event(Event::Type::kOnPortfolioAdded), portfolio(portfolio) {}
};

class EOnPortfolioRemoved : public Event {
public:
  std::shared_ptr<Portfolio> portfolio;

  EOnPortfolioRemoved(const std::shared_ptr<Portfolio> &portfolio)
      : Event(Event::Type::kOnPortfolioRemoved), portfolio(portfolio) {}
};

class EOnPortfolioParentChanged : public Event {
public:
  EOnPortfolioParentChanged(const std::shared_ptr<Portfolio> &portfolio)
      : Event(Event::Type::kOnPortfolioParentChanged), portfolio(portfolio) {}

  std::shared_ptr<Portfolio> portfolio;
};

class EOnPendingNewOrder : public Event {
public:
  std::shared_ptr<Order> order;

  EOnPendingNewOrder(const std::shared_ptr<Order> &order)
      : Event(Event::Type::kOnPendingNewOrder), order(order) {}
};

class EOnNewOrder : public Event {
public:
  std::shared_ptr<Order> order;

  EOnNewOrder(const std::shared_ptr<Order> &order)
      : Event(Event::Type::kOnNewOrder), order(order) {}
};

class EOnSendOrder : public Event {
public:
  std::shared_ptr<Order> order;

  EOnSendOrder(const std::shared_ptr<Order> &order)
      : Event(Event::Type::kOnSendOrder), order(order) {}
};

class EOnOrderStatusChanged : public Event {
public:
  std::shared_ptr<Order> order;

  EOnOrderStatusChanged(const std::shared_ptr<Order> &order)
      : Event(Event::Type::kOnOrderStatusChanged), order(order) {}
};

class EOnOrderCancelled : public Event {
public:
  std::shared_ptr<Order> order;

  EOnOrderCancelled(const std::shared_ptr<Order> &order)
      : Event(Event::Type::kOnOrderCancelled), order(order) {}
};

class EOnOrderCancelRejected : public Event {
public:
  std::shared_ptr<Order> order;

  EOnOrderCancelRejected(const std::shared_ptr<Order> &order)
      : Event(Event::Type::kOnOrderCancelRejected), order(order) {}
};

class EOnOrderDone : public Event {
public:
  std::shared_ptr<Order> order;

  EOnOrderDone(const std::shared_ptr<Order> &order)
      : Event(Event::Type::kOnOrderDone), order(order) {}
};

class EOnOrderExpired : public Event {
public:
  std::shared_ptr<Order> order;

  EOnOrderExpired(const std::shared_ptr<Order> &order)
      : Event(Event::Type::kOnOrderExpired), order(order) {}
};

class EOnOrderFilled : public Event {
public:
  std::shared_ptr<Order> order;

  EOnOrderFilled(const std::shared_ptr<Order> &order)
      : Event(Event::Type::kOnOrderFilled), order(order) {}
};

class EOnOrderPartiallyFilled : public Event {
public:
  std::shared_ptr<Order> order;

  EOnOrderPartiallyFilled(const std::shared_ptr<Order> &order)
      : Event(Event::Type::kOnOrderPartiallyFilled), order(order) {}
};

class EOnOrderRejected : public Event {
public:
  std::shared_ptr<Order> order;

  EOnOrderRejected(const std::shared_ptr<Order> &order)
      : Event(Event::Type::kOnOrderRejected), order(order) {}
};

class EOnOrderReplaced : public Event {
public:
  std::shared_ptr<Order> order;

  EOnOrderReplaced(const std::shared_ptr<Order> &order)
      : Event(Event::Type::kOnOrderReplaced), order(order) {}
};

class EOnOrderReplaceRejected : public Event {
public:
  std::shared_ptr<Order> order;

  EOnOrderReplaceRejected(const std::shared_ptr<Order> &order)
      : Event(Event::Type::kOnOrderReplaceRejected), order(order) {}
};

class EOnFill : public Event {
public:
  std::shared_ptr<Fill> fill;
  std::shared_ptr<Portfolio> portfolio;

  EOnFill(const std::shared_ptr<Portfolio> &portfolio,
          const std::shared_ptr<Fill> &fill)
      : Event(Event::Type::kOnFill), fill(fill), portfolio(portfolio) {}
};

} // namespace h9

#endif // ORDER_EVENTS_H
