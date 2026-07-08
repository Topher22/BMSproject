cmake_minimum_required(VERSION 3.12)
project(BmsSafetyController VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_library(bms_core src/BmsController.cpp)
target_include_directories(bms_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(bms_app app/main.cpp)
target_link_libraries(bms_app PRIVATE bms_core)

add_executable(vde_tests tests/TestBMS.cpp)
target_link_libraries(vde_tests PRIVATE bms_core)