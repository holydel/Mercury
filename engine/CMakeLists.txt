# Platform-specific sources
project(engine)

# Add engine executable or library
add_library(Engine STATIC
    src/engine.cpp
	src/application.cpp
	src/platform.cpp
)

if(WINDOWS_STORE OR WINDOWS_PHONE)
  set_target_properties(Engine PROPERTIES VS_WINRT_COMPONENT "true")
endif()

# Include headers
target_include_directories(Engine PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)