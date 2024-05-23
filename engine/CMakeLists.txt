cmake_minimum_required(VERSION 3.28)
project(MercuryEngine)
set(CMAKE_EXE_LINKER_FLAGS "-static")

set(CMAKE_CXX_STANDARD 17)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../third_party/Vulkan-Headers/include"
        "${CMAKE_CURRENT_LIST_DIR}/../third_party/VulkanMemoryAllocator/include"
        "${CMAKE_CURRENT_LIST_DIR}/../third_party/imgui"
)

add_library(MercuryEngine STATIC src/application.cpp
        src/engine.cpp
        src/platform.cpp
        src/logger.cpp
        src/android/system_android.cpp
        src/llri_vulkan/mercury_vulkan.cpp
        src/llri_vulkan/vk_llri.cpp
        src/llri_vulkan/vk_swapchain.cpp
        src/rendering.cpp
        src/mercury_imgui.cpp
        ../third_party/imgui/imgui.cpp
        ../third_party/imgui/imgui_demo.cpp
        ../third_party/imgui/imgui_draw.cpp
        ../third_party/imgui/imgui_tables.cpp
        ../third_party/imgui/imgui_widgets.cpp
)

if(APPLE)
    target_sources(MercuryEngine PRIVATE src/macos/system_macos.cpp src/macos/cocoa_wrapper.mm src/macos/cocoa_wrapper.h)
else ()
    if(UNIX)
        target_sources(MercuryEngine PRIVATE src/linux/system_linux.cpp)
    endif()
endif()

if(WIN32)
    target_sources(MercuryEngine PRIVATE src/windows/system_windows.cpp src/windows/system_winstore_app.cpp)
    target_link_libraries(MercuryEngine winmm.lib dwmapi.lib)
endif()