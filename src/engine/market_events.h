#ifndef MARKET_EVENT_H
#define MARKET_EVENT_H

#include "event.h"

namespace h9 {

using ProviderId = std::uint16_t;
using InstrumentId = std::uint32_t;

class EMarketData : public Event
{
  public:
    EMarketData(Event::Type type, ptime time, ProviderId pid, InstrumentId iid)
        : Event(type, time), m_pid(pid), m_iid(iid)
    {
    }

  private:
    ProviderId m_pid;
    InstrumentId m_iid;
};
class ETick : public EMarketData
{
  public:
    ETick(Event::Type type, ptime time, ProviderId pid, InstrumentId iid, double price, double vol)
        : EMarketData(type, time, pid, iid), m_price(price), m_vol(vol)
    {
    }

  private:
    double m_price;
    double m_vol;
}

class EAsk : public ETick
{
  public:
    EAsk(ptime time, ProviderId pid, InstrumentId iid, double price, double vol)
        : ETick(Event::Type::kAsk, time, pid, iid, price, vol)
    {
    }
}

class EBid : public ETick
{
  public:
    EBid(ptime time, ProviderId pid, InstrumentId iid, double price, double vol)
        : ETick(Event::Type::kBid, time, pid, iid, price, vol)
    {
    }
}

class ETrade : public ETick
{
  public:
    ETrade(ptime time, ProviderId pid, InstrumentId iid, double price, double vol)
        : ETick(Event::Type::kTrade, time, pid, iid, price, vol)
    {
    }
}

class EQuote : public EMarketData
{
  public:
    EQuote(ptime time, ProviderId pid, InstrumentId iid, double askprice, double askvol, double bidprice, double bidvol)
        : EMarketData(Event::Type::kQuote, time, pid, iid), m_askprice(askprice), m_askvol(askvol), m_bidprice(bidprice), m_bidvol(bidvol)
    {
    }

  private:
    double m_askprice;
    double m_askvol;
    double m_bidprice;
    double m_bidvol;
}

class ENews : public Event
{
  public:
    ENews(ptime time, ProviderId pid, InstrumentId iid, int urgency, std::string url, std::string headline, std::string text)
        : EMarketData(Event::Type::kNews, time, pid, iid), m_urgency(urgency), m_url(url), m_headline(headline), m_text(text)
    {
    }

  private:
    int m_urgency;
    std::string m_url;
    std::string mheadline;
    std::string m_text;
}

enum Level2Side {
    Bid,
    Ask
};
enum Level2Action
{
    New,
    Change,
    Delete,
    Reset
};
struct Level2_t
{
    double price;
    int size;
    Level2Side side;
    Level2Action action;
    int position;
};

class Level2Update : public EMarketData
{
  public:
    Level2Update(ptime time, ProviderId pid, InstrumentId iid, const std::vector<Level2_t> &entries)
        : EMarketData(Event::Type::kLevel2Update), m_entries(entries)
    {
    }

  private:
    std::vector<Level2_t> m_entries;
}

struct tick_t
{
    double price;
    double vol;
};

class Level2Snapshot : public EMarketData
{
  public:
    Level2Snapshot(ptime time, ptime exchange_time, ProviderId pid, InstrumentId iid, const std::vector<tick_t> &bids, const std::vector<tick_t> &asks)
        : EMarketData(Event::Type::Level2Snapshot, time, pid, iid), m_exchange_time(exchange_time), m_bids(bids), m_asks(asks)
    {
    }

  private:
    ptime m_exchange_time;
    std::vector<tick_t> m_bids;
    std::vector<tick_t> m_asks;
}

enum class BarType {
    Time = 1,
    Tick,
    Volume,
    Range,
    Session
};

enum class BarStatus
{
    Incomplete,
    Complete,
    Open,
    High,
    Low,
    Close
};

enum class BarInput
{
    Trade,
    Bid,
    Ask,
    Middle,
    Tick,
    BidAsk
};

enum class BarData
{
    Close,
    Open,
    High,
    Low,
    Median,
    Typical,
    Weighted,
    Average,
    Volume,
    OpenInt,
    Range,
    Mean,
    Variance,
    StdDev
};

class EBar : public Event
{
  public:
    Bar(ptime open_time, ptime close_time, InstrumentId iid, BarType type, long size,
        double open = 0.0, double high = 0.0, double low = 0.0, double close = 0.0, long volume = 0, long openInt = 0)
        : Event(Event::Type::kBar, close_time), m_iid(iid), m_type(type), m_size(size),
          m_open(open), m_high(high), m_low(low), m_close(close), m_vol(volume), m_open_interest(openInt),
          m_status(open_time == close_time ? BarStatus::Open : BarStatus::Close)
    {
    }

    ptime close_time() const { return time(); }
  private:
    ptime m_open_time;
    Instrument m_iid;
    BarType m_type;
    long m_size;
    double m_open;
    double m_high;
    double m_low;
    double m_close;
    long m_vol;
    long m_open_interest;
    BarStatus m_status;
};

} // namespace h9

#endif // MARKET_EVENT_H