set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(modmqttd 
    main.cpp 
)

#TODO cmake find for mosquitto libraries
target_link_libraries(modmqttd 
    modmqttsrv
    ${MOSQUITTO_LIBRARIES}
    yaml-cpp
    spdlog
    ${LIBMODBUS_LIBRARIES} 
    ${CMAKE_THREAD_LIBS_INIT}
)

if(SYSTEMD_FOUND  AND WITH_SYSTEMD)
    target_link_libraries(modmqttd systemd)
endif()