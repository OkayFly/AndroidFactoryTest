# SPDX-License-Identifier: MIT

project(factory-test-server C)
cmake_minimum_required(VERSION 2.6)
add_executable(factory-test-server factory-test-server.c data.c)
target_link_libraries(factory-test-server rt)
install(TARGETS factory-test-server DESTINATION bin)
