cmake_minimum_required(VERSION 3.6)
project(tg_remindmebot)

set(CMAKE_CXX_STANDARD 11)

set(Boost_USE_MULTITHREADED ON)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system iostreams date_time REQUIRED)
include_directories(/usr/local/include ${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR})

set(SOURCE_FILES main.cpp timeHandling.h  notificationCache.h)
add_executable(tg_remindmebot ${SOURCE_FILES})
target_link_libraries(tg_remindmebot /usr/local/lib/libTgBot.a ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES})