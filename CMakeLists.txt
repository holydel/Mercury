cmake_minimum_required(VERSION 3.28)

project(Mercury VERSION 0.01 LANGUAGES CXX)

if(WIN32)
set(CMAKE_EXE_LINKER_FLAGS "-static")
else()
    find_package(X11 REQUIRED)
    link_libraries(xcb)
endif()

include_directories("${CMAKE_CURRENT_LIST_DIR}/engine/include")

add_subdirectory(engine)

add_executable(TestBed testbed/src/testbed.cpp)
target_link_libraries(TestBed MercuryEngine)

#add_executable(TestBedXR testbed_xr/src/testbed_xr.cpp)
#add_executable(SolarBaker solar_baker/src/main.cpp)