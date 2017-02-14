
project (tws)
cmake_minimum_required(VERSION 2.8)

set(TWS_MAJOR_VERSION 0)
set(TWS_MINOR_VERSION 1)
set(TWS_PATCH_VERSION 0)
set (TWS_VERSION ${TWS_MAJOR_VERSION}.${TWS_MINOR_VERSION}.${TWS_PATCH_VERSION})

add_definitions(-DTWS_LIBRARY_COMPILATION)

set(HDR_FILES 
	client/CommissionReport.h
	client/CommonDefs.h
        client/Condition.h
        client/Contract.h
        client/DefaultEWrapper.h
        client/EClient.h
        client/EClientSocket.h
        client/econfig.h
        client/econstants.h
        client/EWrapper.h
        client/Execution.h
        client/Order.h
        client/ScannerSubscription.h
        client/SoftDollarTier.h
        client/TagValue.h
        client/TwsSocketClientErrors.h
)

set(SRC_FILES
        client/Condition.cpp
        client/EClient.cpp
        client/EClientSocket.cpp
   )

add_library (twsclient ${SRC_FILES} ${HDR_FILES})
target_link_libraries(twsclient ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
include_directories(client)
add_executable(test_tws test_tws.cpp)
target_link_libraries(test_tws twsclient ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES} Threads::Threads )