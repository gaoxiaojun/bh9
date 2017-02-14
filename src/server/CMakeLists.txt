
file (GLOB SRC_FILES *.cpp)
file (GLOB HDR_FILES *.hpp)


if (OPENSSL_FOUND)

add_executable (dtc_server ${SRC_FILES} ${HDR_FILES})
target_link_libraries(dtc_server ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})
#set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "server")
#target_compile_features(dtc_server PRIVATE cxx_range_for)
endif()
