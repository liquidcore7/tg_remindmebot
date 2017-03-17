cmake_minimum_required(VERSION 3.5)
project(reminderbot2)

set(CMAKE_CXX_STANDARD 14)

set(Boost_USE_MULTITHREADED ON)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system iostreams date_time REQUIRED)
include_directories(/usr/local/include ${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR})

link_libraries(pthread sqlite3)

set(SOURCE_FILES main.cpp)
add_executable(reminderbot2 ${SOURCE_FILES})
target_link_libraries(reminderbot2 /usr/local/lib/libTgBot.a ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES})