project(TestBed)
add_executable(TestBed WIN32 src/testbed.cpp)

if(WINDOWS_STORE OR WINDOWS_PHONE)
	set_target_properties(TestBed PROPERTIES VS_WINRT_COMPONENT "true")
endif()

target_link_libraries(TestBed PUBLIC Engine)
