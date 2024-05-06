project(SolarBaker)

add_executable(SolarBaker WIN32
    src/main.cpp
)

target_link_libraries(SolarBaker PUBLIC Engine)

if(WINDOWS_STORE OR WINDOWS_PHONE)
  set_target_properties(SolarBaker PROPERTIES VS_WINRT_COMPONENT "true")
endif()