cmake_minimum_required(VERSION 3.20)
project(Mercury)

# Set the C++ standard globally
set(CMAKE_CONFIGURATION_TYPES "Debug;Develope;Profile;Shipping" CACHE STRING "Project configurations" FORCE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_SHIPPING "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "Flags used by the C++ compiler during Shipping builds." FORCE)
set(CMAKE_C_FLAGS_SHIPPING "${CMAKE_C_FLAGS_RELEASE}" CACHE STRING "Flags used by the C compiler during Shipping builds." FORCE)
set(CMAKE_EXE_LINKER_FLAGS_SHIPPING "${CMAKE_EXE_LINKER_FLAGS_RELEASE}" CACHE STRING "Linker flags used for Shipping executables." FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_SHIPPING "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}" CACHE STRING "Linker flags used for Shipping shared libraries." FORCE)

set(CMAKE_CXX_FLAGS_DEVELOPE "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "Flags used by the C++ compiler during Shipping builds." FORCE)
set(CMAKE_C_FLAGS_DEVELOPE "${CMAKE_C_FLAGS_RELEASE}" CACHE STRING "Flags used by the C compiler during Shipping builds." FORCE)
set(CMAKE_EXE_LINKER_FLAGS_DEVELOPE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}" CACHE STRING "Linker flags used for Shipping executables." FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_DEVELOPE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}" CACHE STRING "Linker flags used for Shipping shared libraries." FORCE)

set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "Flags used by the C++ compiler during Shipping builds." FORCE)
set(CMAKE_C_FLAGS_PROFILE "${CMAKE_C_FLAGS_RELEASE}" CACHE STRING "Flags used by the C compiler during Shipping builds." FORCE)
set(CMAKE_EXE_LINKER_FLAGS_PROFILE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}" CACHE STRING "Linker flags used for Shipping executables." FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_PROFILE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}" CACHE STRING "Linker flags used for Shipping shared libraries." FORCE)

# Optionally set global compiler options
add_compile_options(-Wall)

# Include subdirectories
add_subdirectory(engine)
add_subdirectory(solar_baker)
add_subdirectory(testbed)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TestBed)