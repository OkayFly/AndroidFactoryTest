# SPDX-License-Identifier: MIT

project(factorytest C)
cmake_minimum_required(VERSION 2.6)
add_executable(factorytest factorytest.c)
target_link_libraries(factorytest rt)
install(TARGETS factorytest DESTINATION bin)
