set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(modmqttd 
    main.cpp 
)

#TODO cmake find for mosquitto libraries
target_link_libraries(modmqttd 
    modmqttsrv
    ${MOSQUITTO_LIBRARIES}
    yaml-cpp
    ${LIBMODBUS_LIBRARIES} 
    ${CMAKE_THREAD_LIBS_INIT}
)

if(WITH_SYSTEMD)
    target_link_libraries(modmqttd systemd)
endif()
if(WITH_SPDLOG_FMT_EXTERNAL AND NOT WITH_FMT_HEADER_ONLY)
    target_link_libraries(modmqttd fmt)
endif()
target_link_libraries(modmqttd spdlog)