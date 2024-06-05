cmake_minimum_required(VERSION 3.28)
project(MercuryEngine)
set(CMAKE_EXE_LINKER_FLAGS "-static")

set(CMAKE_CXX_STANDARD 17)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../third_party/Vulkan-Headers/include"
        "${CMAKE_CURRENT_LIST_DIR}/../third_party/VulkanMemoryAllocator/include"
        "${CMAKE_CURRENT_LIST_DIR}/../third_party/imgui"
        "${CMAKE_CURRENT_LIST_DIR}/../third_party/metal-cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../third_party/metal-cpp-extensions"
        "${CMAKE_CURRENT_LIST_DIR}/../third_party/DirectX-Headers/include"
)

if (EMSCRIPTEN)
    include_directories("${CMAKE_CURRENT_LIST_DIR}/../third_party/wasm_webgpu/lib")
endif()

add_library(MercuryEngine STATIC src/application.cpp
        src/engine.cpp
        src/platform.cpp
        src/logger.cpp
        src/android/system_android.cpp
        src/llri_vulkan/mercury_vulkan.cpp
        src/llri_vulkan/vk_llri.cpp
        src/llri_vulkan/vk_swapchain.cpp
        src/llri_d3d12/d3d12_llri.cpp
        src/llri_d3d12/d3d12_swapchain.cpp
        src/rendering.cpp
        src/mercury_imgui.cpp
        ../third_party/imgui/imgui.cpp
        ../third_party/imgui/imgui_demo.cpp
        ../third_party/imgui/imgui_draw.cpp
        ../third_party/imgui/imgui_tables.cpp
        ../third_party/imgui/imgui_widgets.cpp
        src/llri_d3d12/d3d12_context.cpp
        src/llri_vulkan/vk_context.cpp
        src/emscripten/imgui_impl_emscripten.cpp
        src/emscripten/imgui_impl_emscripten.h
)

if(EMSCRIPTEN)
    target_sources(MercuryEngine PRIVATE src/llri_webgpu/webgpu_context.cpp
            src/llri_webgpu/webgpu_llri.cpp
            src/llri_webgpu/webgpu_llri.h
            src/llri_webgpu/webgpu_swapchain.cpp
            src/llri_webgpu/webgpu_swapchain.h
            src/emscripten/system_emscripten.cpp
            ../third_party/wasm_webgpu/lib/lib_webgpu.cpp
            ../third_party/wasm_webgpu/lib/lib_webgpu_cpp20.cpp)

endif()

if(APPLE)
    target_sources(MercuryEngine PRIVATE src/macos/system_macos.mm
            src/macos/cocoa_wrapper.mm
            src/macos/cocoa_wrapper.h
            src/macos/imgui_wrapper.mm
            src/llri_metal/metal_context.cpp
            src/llri_metal/metal_llri.cpp
            src/llri_metal/metal_llri.h
            src/llri_metal/metal_swapchain.cpp
            src/llri_metal/metal_swapchain.h
            src/macos/metal_impl.cpp
    )
else ()
    if(UNIX)
        target_sources(MercuryEngine PRIVATE src/linux/system_linux.cpp)
    endif()
endif()

if(WIN32)
    target_sources(MercuryEngine PRIVATE src/windows/system_windows.cpp src/windows/system_winstore_app.cpp)
    target_link_libraries(MercuryEngine winmm.lib dwmapi.lib)
endif()