# SPDX-License-Identifier: MIT

project(factory-test-server C)
cmake_minimum_required(VERSION 2.6)
add_executable(factory_test_server factory_test_server.c data.c serial_test.c can_test.c factory_test_function.c)
target_link_libraries(factory_test_server rt pthread)
install(TARGETS factory_test_server DESTINATION bin)
