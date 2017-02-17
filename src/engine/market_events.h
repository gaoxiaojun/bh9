#ifndef MARKET_EVENT_H
#define MARKET_EVENT_H

#include "event.h"

namespace h9 {

class EMarketData : public Event {
public:
  EMarketData(Event::Type type, ptime time, ProviderId pid, InstrumentId iid)
      : Event(type, time), m_pid(pid), m_iid(iid) {}
  ProviderId provider_id() const { return m_pid; }
  InstrumentId instrument_id() const { return m_iid; }

private:
  ProviderId m_pid;
  InstrumentId m_iid;
};

class ETick : public EMarketData {
public:
  ETick(Event::Type type, ptime time, ProviderId pid, InstrumentId iid,
        double price, long vol)
      : EMarketData(type, time, pid, iid), m_price(price), m_vol(vol) {}
  double price() const noexcept { return m_price; }
  long volume() const noexcept { return m_vol; }

private:
  double m_price;
  double m_vol;
};

class EAsk : public ETick {
public:
  EAsk(ptime time, ProviderId pid, InstrumentId iid, double price, long vol)
      : ETick(Event::Type::kAsk, time, pid, iid, price, vol) {}
};

class EBid : public ETick {
public:
  EBid(ptime time, ProviderId pid, InstrumentId iid, double price, long vol)
      : ETick(Event::Type::kBid, time, pid, iid, price, vol) {}
};

class ETrade : public ETick {
public:
  ETrade(ptime time, ProviderId pid, InstrumentId iid, double price, long vol)
      : ETick(Event::Type::kTrade, time, pid, iid, price, vol) {}
};

class EQuote : public EMarketData {
public:
  EQuote(ptime time, ProviderId pid, InstrumentId iid, double askprice,
         double askvol, double bidprice, double bidvol)
      : EMarketData(Event::Type::kQuote, time, pid, iid),
        m_askprice(askprice), m_askvol(askvol),
        m_bidprice(bidprice), m_bidvol(bidvol) {}

private:
  double m_askprice;
  double m_askvol;
  double m_bidprice;
  double m_bidvol;
};

class ENews : public EMarketData {
public:
  ENews(ptime time, ProviderId pid, InstrumentId iid, int urgency,
        std::string url, std::string headline, std::string text)
      : EMarketData(Event::Type::kNews, time, pid, iid), m_urgency(urgency),
        m_url(url), m_headline(headline), m_text(text) {}

private:
  int m_urgency;
  std::string m_url;
  std::string m_headline;
  std::string m_text;
};

enum Level2Side { Bid, Ask };
enum Level2Action { New, Change, Delete, Reset };
struct Level2_t {
  double price;
  int size;
  Level2Side side;
  Level2Action action;
  int position;
};

class Level2Update : public EMarketData {
public:
  Level2Update(ptime time, ProviderId pid, InstrumentId iid,
               const std::vector<Level2_t> &entries)
      : EMarketData(Event::Type::kLevel2Update, time, pid, iid),
        m_entries(entries) {}

private:
  std::vector<Level2_t> m_entries;
};

struct tick_t {
  double price;
  long vol;
};

class Level2Snapshot : public EMarketData {
public:
  Level2Snapshot(ptime time, ProviderId pid, InstrumentId iid,
                 const std::vector<tick_t> &bids,
                 const std::vector<tick_t> &asks)
      : EMarketData(Event::Type::kLevel2Snapshot, time, pid, iid),
        m_bids(bids), m_asks(asks) {}

private:
  std::vector<tick_t> m_bids;
  std::vector<tick_t> m_asks;
};

} // namespace h9

#endif // MARKET_EVENT_H
