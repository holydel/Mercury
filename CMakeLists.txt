cmake_minimum_required(VERSION 3.28)

project(Mercury VERSION 0.01 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

if(WIN32)
set(CMAKE_EXE_LINKER_FLAGS "-static")
else()
    if(NOT APPLE)
        find_package(X11 REQUIRED)
        link_libraries(xcb xcb-keysyms)
    endif()
endif()



include_directories("${CMAKE_CURRENT_LIST_DIR}/engine/include")

if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "-framework Cocoa -framework AppKit -framework CoreData -framework Foundation -framework Metal -framework AppKit -framework MetalKit -framework GameController")
endif()

add_subdirectory(engine)

add_executable(TestBed testbed/src/testbed.cpp)
target_link_libraries(TestBed MercuryEngine)

#add_executable(TestBedXR testbed_xr/src/testbed_xr.cpp)
#add_executable(SolarBaker solar_baker/src/main.cpp)