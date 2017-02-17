#include "bar_factory.h"
#include "market_events.h"
#include <algorithm>
#include <iostream>

using namespace h9;

void BarFactory::add(const BarFactoryItem &item) {
  if (item.factory() != nullptr)
    throw new std::logic_error(
        "BarFactoryItem is already added to another BarFactory instance.");

  item.set_factory(this);

  auto &list = m_item_lists[item.instrument_id()];

  auto it = std::find_if(
      list.begin(), list.end(),
      [item](const std::shared_ptr<BarFactoryItem> &i) { return *i == item; });

  if (it == list.end())
    list.add(item);
}

void BarFactory::remove(const BarFactoryItem &item) {
  auto mit = m_item_lists.find(item.instrument_id());
  if (mit == m_item_lists.end())
    return;
  auto list = *mit;

  auto it = std::find_if(
      list.begin(), list.end(),
      [item](const std::shared_ptr<BarFactoryItem> &i) { return *i == item; });

  if (it != list.end()) {
    list.erase(it);
  } else
    std::cout << "Warnging: remove BarFactoryItem not found" << std::endl;
}

void BarFactory::clear() {
  m_item_lists.clear();
  m_reminder_lists.clear();
}

void BarFactory::on_event(const Event::Pointer &e) {
  auto tick = event_cast<ETick>(e);
  auto items = m_item_lists[tick->instrument_id()]; // TOOD: if not exist, map
                                                    // would be create one

  int i = 0;
  while (i < items.cout()) {
    auto item = items[i];
    switch (item.input()) {
    case Bar::Input::kTrade:
      if (tick->type() == Event::Type::kTrade)
        process(e);
      break;
    case Bar::Input::kBid:
      if (tick->type() == Event::Type::kBid)
        process(e);
      break;
    case Bar::Input::kAsk:
      if (tick->type() == Event::Type::kAsk)
        process(e);
      break;
    case Bar::Input::kMiddle:
      // TODO
      switch (tick->type()) {
      case Event::Type::kBid:

        break;
      case Event::Type::kAsk:
        break;
      case Event::Type::kTrade:
        break;
      }
      break;
    case Bar::Input::kTick:
      break;
    case Bar::Input::kBidAsk:
      // TODO:
      break;
    default:
      std::cout << "BarFactory::OnData BarInput is not supported :"
                << item.input() << std::endl;
    }
  }
}

bool BarFactory::add_reminder(const BarFactoryItem &item, ptime time) {}

void BarFactory::on_reminder(ptime time, const BFItemPtr &item) {}
