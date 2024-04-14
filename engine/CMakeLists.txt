# Platform-specific sources
if(WIN32)
    set(PLATFORM_SOURCES src/windows/system_impl.cpp)
    message(STATUS "Adding Windows specific sources")
elseif(APPLE)
    if(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
        set(PLATFORM_SOURCES src/ios/system_impl.cpp)
        message(STATUS "Adding iOS specific sources")
    else()
        set(PLATFORM_SOURCES src/macos/system_impl.cpp)
        message(STATUS "Adding macOS specific sources")
    endif()
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    set(PLATFORM_SOURCES src/android/system_impl.cpp)
    message(STATUS "Adding Android specific sources")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(PLATFORM_SOURCES src/linux/system_impl.cpp)
    message(STATUS "Adding Linux specific sources")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# Add engine executable or library
add_library(Engine STATIC
    ${PLATFORM_SOURCES}
    src/engine.cpp
    src/mercury.ixx
    src/system.ixx
)

# Include headers
target_include_directories(Engine PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)