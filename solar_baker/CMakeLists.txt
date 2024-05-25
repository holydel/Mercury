project(TestBed)

file(GLOB SolarBaker_SRC
        "src/*.h"
        "src/*.cpp"
)

include_directories("${CMAKE_CURRENT_LIST_DIR}/../third_party/imgui")

add_executable(SolarBaker WIN32 ${SolarBaker_SRC})
target_link_libraries(SolarBaker PUBLIC MercuryEngine)