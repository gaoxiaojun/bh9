#include "bar_factory.h"
#include "bar_items/range_item.h"
#include "bar_items/session_item.h"
#include "bar_items/tick_item.h"
#include "bar_items/time_item.h"
#include "bar_items/volume_item.h"
#include "event_bus.h"
#include "market_events.h"
#include "util.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>

using namespace h9;

template <class T, class... Ts>
std::shared_ptr<BarFactoryItem> make_bar_factory_item(Ts &&... xs) {
  return ::std::static_pointer_cast<BarFactoryItem>(
      std::make_shared<T>(std::forward<Ts>(xs)...));
}

BarFactory::BarFactory(EventBus *bus) : m_bus(bus) {}

void BarFactory::add(InstrumentId iid, Bar::Type barType, long barSize,
                     BarInput barInput) {
  BarFactoryItemPtr item;
  switch (barType) {
  case Bar::Type::kTime:
    item = make_bar_factory_item<TimeBarFactoryItem>(iid, barSize, barInput);
    break;
  case Bar::Type::kTick:
    item = make_bar_factory_item<TickBarFactoryItem>(iid, barSize, barInput);
    break;
  case Bar::Type::kVolume:
    item = make_bar_factory_item<VolumeBarFactoryItem>(iid, barSize, barInput);
    break;
  case Bar::Type::kRange:
    item = make_bar_factory_item<RangeBarFactoryItem>(iid, barSize, barInput);
    break;
  case Bar::Type::kSession:
    throw std::logic_error("BarFactory::Add Can not create "
                           "SessionBarFactoryItem without session "
                           "parameters");
    break;
  default:
    throw std::logic_error("Unknown bar type -" + bar_type_to_string(barType));
  }
  add(item);
}

void BarFactory::add(InstrumentId iid, Bar::Type barType, long barSize,
                     BarInput barInput, time_duration session1,
                     time_duration session2) {
  BarFactoryItemPtr item;
  switch (barType) {
  case Bar::Type::kTime:
    item = make_bar_factory_item<TimeBarFactoryItem>(iid, barSize, barInput,
                                                     session1, session2);
    break;
  case Bar::Type::kTick:
    item = make_bar_factory_item<TickBarFactoryItem>(iid, barSize, barInput,
                                                     session1, session2);
    break;
  case Bar::Type::kVolume:
    item = make_bar_factory_item<VolumeBarFactoryItem>(iid, barSize, barInput,
                                                       session1, session2);
    break;
  case Bar::Type::kRange:
    item = make_bar_factory_item<RangeBarFactoryItem>(iid, barSize, barInput,
                                                      session1, session2);
    break;
  case Bar::Type::kSession:
    item = make_bar_factory_item<SessionBarFactoryItem>(iid, barInput, session1,
                                                        session2);
    break;
  default:
    throw std::logic_error("Unknown bar type -" + bar_type_to_string(barType));
  }
  add(item);
}

void BarFactory::add(const BarFactoryItemPtr &item) {
  if (item->m_factory != nullptr)
    throw std::logic_error(
        "BarFactoryItem is already added to another BarFactory instance.");

  item->m_factory = this;

  m_item_map.emplace(item->instrument_id(), item);
}

void BarFactory::remove(const BarFactoryItemPtr &item) {
  auto range = m_item_map.equal_range(item->instrument_id());
  for (auto& it = range.first; it != range.second; ++it) {
    auto &sp_item = (*it).second;
    if (*sp_item == *item) {
      m_item_map.erase(it);
      return;
    }
  }

  std::cout << "Warnging: remove BarFactoryItem not found" << std::endl;
}

void BarFactory::clear() {
  m_item_map.clear();
  m_reminder_map.clear();
}

void BarFactory::on_tick(const Event::Pointer &e) {
  const auto tick = event_cast<ETick>(e);
  auto range = m_item_map.equal_range(tick->instrument_id());
  //std::cout << "factory:on_tick:" << (range.first == range.second) << std::endl;
  for (auto& it = range.first; it != range.second; ++it) {
    auto &item = (*it).second; // value
    switch (item->m_input) {
    case BarInput::kTrade:
      if (tick->type() != Event::Type::kTrade)
        continue;
      break;
    case BarInput::kBid:
      if (tick->type() != Event::Type::kBid)
        continue;
      break;
    case BarInput::kAsk:
      if (tick->type() != Event::Type::kAsk)
        continue;
      break;
    case BarInput::kMiddle:
      // TODO
      switch (tick->type()) {
      case Event::Type::kBid:

        break;
      case Event::Type::kAsk:
        break;
      case Event::Type::kTrade:
        continue;
      }
      // if (e->type() == Event::Type::kAsk)
      //      return;
      break;
    case BarInput::kTick:
      break;
    case BarInput::kBidAsk:
      if (tick->type() == Event::Type::kTrade)
        return;
      break;
    default:
      throw std::logic_error("BarFactory::OnData BarInput is not supported :" +
                             bar_input_to_string(item->m_input));
      continue;
    }
    item->process(e);
  }
}

/* void BarFactory::show()
{
    std::cout << "item_map: ";
    for(auto& it : m_item_map) {
        std::cout << "key:" << it.first << " value:" << it.second->instrument_id() << ":" << it.second->provider_id()<< std::endl;
    }
    std::cout << std::endl;
}*/

bool BarFactory::add_reminder(ptime time, const BarFactoryItemPtr &item) {
  bool is_exist = true;
  auto mit = m_reminder_map.find(time);
  if (mit == m_reminder_map.end())
    is_exist = false;

  m_reminder_map.emplace(time, item);

  if (!is_exist) {
    Event::Pointer reminder = make_event<EReminder>(
        time, std::bind(&BarFactory::on_reminder, this, std::placeholders::_1));
    m_bus->enqueue(reminder);
  }
}

void BarFactory::on_reminder(ptime time) {

  auto range = m_reminder_map.equal_range(time);
 std::cout << "on reminder in bar factory:" << (range.first == range.second) << std::endl;
  if (range.first == range.second)
    throw std::logic_error(
        "BarFactory::on_reminder, cannot found reminder_list");

  try {
    for (auto& it = range.first; it != range.second; ++it) {
      auto &item = (*it).second;
      item->on_reminder(time);
    }
  } catch (...) {
    m_reminder_map.erase(time);
  }
}
