cmake_minimum_required(VERSION 3.20)
project(Mercury)

# Set the C++ standard globally
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optionally set global compiler options
add_compile_options(-Wall)

# Depending on your compiler, you may need to add flags to enable module support
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-fmodules)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(WARNING "GCC may not fully support modules yet.")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/experimental:module)
endif()

# Graphics API options
# vulkan working evrywhere (via MoltenVK on Apple platforms)
option(USE_VULKAN "Use Vulkan as the graphics API" ON)

if(WIN32)
    add_definitions(-DWINDOWS_PLATFORM)
    option(USE_D3D12 "Use Direct3D 12 as the graphics API" OFF)	
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    add_definitions(-DANDROID_PLATFORM)
    # Link specific libraries for Android
    target_link_libraries(YourTarget PRIVATE log android)
elseif(APPLE)
     option(USE_METAL "Use Metal as the graphics API" OFF)
    if(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
        add_definitions(-DIOS_PLATFORM)
    else()
        add_definitions(-DMACOS_PLATFORM)
    endif()
endif()


# Ensure exactly one API is chosen
if(NOT ((USE_D3D12 AND NOT USE_VULKAN AND NOT USE_METAL) OR 
        (USE_VULKAN AND NOT USE_D3D12 AND NOT USE_METAL) OR 
        (USE_METAL AND NOT USE_D3D12 AND NOT USE_VULKAN)))
    message(FATAL_ERROR "Exactly one graphics API must be selected.")
endif()

# Include subdirectories
add_subdirectory(engine)
add_subdirectory(assets_packer)
add_subdirectory(testbed)