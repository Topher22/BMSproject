cmake_minimum_required(VERSION 3.12)
project(BMSSafetyController VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force executables directly into the root of the build folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# 1. Compile the core shared logic as a separate static link library
add_library(bms_core src/BMScontroller.cpp)
target_include_directories(bms_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 2. Build Target: Production Application (uses app/main.cpp)
add_executable(bms_app app/main.cpp)
target_link_libraries(bms_app PRIVATE bms_core)

# 3. Build Target: Isolated Safety Regression Test Engine (uses tests/TestBms.cpp)
add_executable(vde_tests tests/TestBMS.cpp)
target_link_libraries(vde_tests PRIVATE bms_core)