cmake_minimum_required(VERSION 3.28)
project(MercuryEngine)
set(CMAKE_EXE_LINKER_FLAGS "-static")

set(CMAKE_CXX_STANDARD 17)

add_library(MercuryEngine STATIC src/application.cpp
        src/engine.cpp
        src/platform.cpp
        src/windows/system_windows.cpp
        src/windows/system_winstore_app.cpp
        src/android/system_android.cpp)

if(WIN32)
target_link_libraries(MercuryEngine winmm.lib)
endif()