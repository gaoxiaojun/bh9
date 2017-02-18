#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include "common.h"
#include <string>
#include "currency.h"

namespace h9
{

struct AltId
{
  int providerId;
  std::string symbol;
  std::string exchange;
  Currency::Id currency;
};

struct Leg
{
};

class Instrument : private boost::noncopyable
{
public:
  enum Type
  {
    kStock,
    kFuture,
    kOption,
    kFutureOption,
    kBond,
    kFX,
    kIndex,
    kETF,
    kMultiLeg,
    kSynthetic
  };
  enum PutCall
  {
    kPut,
    kCall
  };

  Instrument() {}

  Instrument(int id, Type type, const std::string &symbol,
             const std::string &description, std::uint8_t currencyId,
             std::string exchange)
      : this(type, symbol, description, currencyId)
  {
    Id = id;
    Exchange = exchange;
  }

  Instrument(int id, Type synthetic, std::string v1, std::string v2, int v3)
  {
    Id = id;
    this.synthetic = synthetic;
    this.v1 = v1;
    this.v2 = v2;
    this.v3 = v3;
  }
  Instrument(Instrument instrument) { throw NotImplementedException(); }

  Instrument(Type type, std::string symbol, std::string description = "",
             std::uint8_t currencyId = 148 /* USD */)
  {
    m_type = type;
    m_symbol = symbol;
    m_description = description;
    m_currencyId = currencyId;
  }

  std::uint8_t GetCurrencyId(std::uint8_t providerId)
  {
    var altId = AltId.Get(providerId);
    if (altId != null && altId.CurrencyId != 0)
      return altId.CurrencyId;
    else
      return CurrencyId;
  }

  Instrument Clone(std::string symbol = null)
  {
    var other = new Instrument(this);
    other.Symbol = symbol ? ? other.Symbol;
    return other;
  }

  std::string ToString();

#region Extra
  void Init(Framework framework)
  {
    this.framework = framework;
    foreach (var leg in Legs)
      leg.Init(framework);
  }

private:
  Framework framework;
  InstrumentType synthetic;
  std::string v1;
  std::string v2;
  int v3;
  bool Loaded;

  int Id;
  Bid Bid;

  Ask Ask;
  Bar Bar;

  Trade Trade;

  ObjectTable Fields;
  ;

  List<Leg> Legs;

  AltIdList AltId;
  bool DeleteCached;

  double Factor;

  std::string Symbol;

  std::string Description;

  std::string Exchange;

  std::uint8_t CCY1;

  std::uint8_t CCY2;

  double Margin;

  DateTime Maturity;

  Instrument Parent;

  PutCall PutCall;

  double Strike;

  double TickSize;

  InstrumentType Type;
  std::uint8_t CurrencyId;

  std::string Formula;

  std::string PriceFormat;
  //= "F2";

  IDataProvider DataProvider;

  IExecutionProvider ExecutionProvider;
}

class InstrumentList : IEnumerable<Instrument>
{
private
  readonly GetByList<Instrument> list =
      new GetByList<Instrument>("Id", "Symbol");

  int Count = > this.list.Count;

  Instrument this[std::string symbol] = > Get(symbol);

  bool Contains(std::string symbol) = > this.list.Contains(symbol);

  bool Contains(Instrument instrument) = > this.list.Contains(instrument);

  bool Contains(int id) = > this.list.Contains(id);

  Instrument Get(std::string symbol) = > this.list.GetByName(symbol);

  Instrument GetByIndex(int index) = > this.list.GetByIndex(index);

  IEnumerator<Instrument> GetEnumerator() = > this.list.GetEnumerator();

  IEnumerator IEnumerable.GetEnumerator() = > GetEnumerator();

  void Add(Instrument instrument)
  {
    if (this.list.GetById(instrument.Id) == null)
      this.list.Add(instrument);
    else
      Console.WriteLine($ "InstrumentList::Add Instrument {instrument.Symbol} "
                          "with Id = {instrument.Id} is already in the list");
  }

  void Remove(Instrument instrument) = > this.list.Remove(instrument);

  void Clear() = > this.list.Clear();

  Instrument GetById(int id) = > this.list.GetById(id);

  std::string ToString();
}

} // namespace h9

#endif // INSTRUMENT_H
