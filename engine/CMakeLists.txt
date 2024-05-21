cmake_minimum_required(VERSION 3.28)
project(MercuryEngine)
set(CMAKE_EXE_LINKER_FLAGS "-static")

set(CMAKE_CXX_STANDARD 17)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../third_party/Vulkan-Headers/include"
        "${CMAKE_CURRENT_LIST_DIR}/../third_party/VulkanMemoryAllocator/include"
)

add_library(MercuryEngine STATIC src/application.cpp
        src/engine.cpp
        src/platform.cpp
        src/logger.cpp
        src/windows/system_windows.cpp
        src/windows/system_winstore_app.cpp
        src/android/system_android.cpp
        src/linux/system_linux.cpp
        src/vulkan/mercury_vulkan.cpp
        src/vulkan/vk_llri.cpp
        src/vulkan/vk_swapchain.cpp
)

if(WIN32)
target_link_libraries(MercuryEngine winmm.lib)
endif()