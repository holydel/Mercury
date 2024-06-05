project(TestWebGPU)
add_executable(TestWebGPU main.cpp)

if(EMSCRIPTEN)
set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

#target_link_libraries(TestWebGPU PUBLIC MercuryEngine)