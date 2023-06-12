cmake_minimum_required(VERSION 3.10)

project(httpserver)

set(SOURCES
	src/connection.cpp
	src/reply.cpp
	src/request_handler.cpp
	src/request_parser.cpp
	src/server.cpp
	src/thread_pool.cpp
        src/logger.cpp
	src/main.cpp
)

add_executable(httpserver ${SOURCES})

target_include_directories(httpserver
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)

