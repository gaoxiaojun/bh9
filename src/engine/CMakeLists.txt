cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(engine)
set (CMAKE_CXX_STANDARD 11)

set(ENGINE_HEADER
    common.h
    currency.h
    event.h
    event_bus.h
    market_events.h
    system_events.h
    bar.h
    bar_factory.h
    bar_factory_item.h
    bar_items/range_item.h
    bar_items/session_item.h
    bar_items/tick_item.h
    bar_items/time_item.h
    bar_items/volume_item.h
    util.h

    bar_slice_factory.h
)
set(ENGINE_SRC
  bar.cpp
  currency.cpp
  event_bus.cpp
  bar_factory.cpp
  bar_factory_item.cpp
  bar_items/range_item.cpp
  bar_items/session_item.cpp
  bar_items/tick_item.cpp
  bar_items/time_item.cpp
  bar_items/volume_item.cpp
  util.cpp
  bar_slice_factory.cpp
)

add_library(engine STATIC  ${ENGINE_HEADER} ${ENGINE_SRC})
#target_link_libraries(engine ${BOOST_LIBRARIES})

