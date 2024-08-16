project(SolarBaker)

file(GLOB SolarBaker_SRC
        "src/*.h"
        "src/*.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/../third_party/libdeflate/lib/gdeflate_compress.c"
        "${CMAKE_CURRENT_LIST_DIR}/../third_party/libdeflate/lib/utils.c"
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../third_party/imgui"
        "${CMAKE_CURRENT_LIST_DIR}/../third_party/pugixml/src"
        "${CMAKE_CURRENT_LIST_DIR}/../third_party/libdeflate"
        "${CMAKE_CURRENT_LIST_DIR}/../third_party/glslang")


add_executable(SolarBaker WIN32 ${SolarBaker_SRC})
target_link_libraries(SolarBaker PUBLIC MercuryEngine)