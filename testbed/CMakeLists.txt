project(TestBed)
add_executable(TestBed src/testbed.cpp)

if(WINDOWS_STORE OR WINDOWS_PHONE)
	set_target_properties(TestBed PROPERTIES VS_WINRT_COMPONENT "true")
endif()

if(EMSCRIPTEN)
set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

target_link_libraries(TestBed PUBLIC MercuryEngine)

if(EMSCRIPTEN)
	target_link_options(TestBed PRIVATE
			-sUSE_WEBGPU # Handle WebGPU symbols
			-sASYNCIFY # Required by WebGPU-C++
	)
endif()

