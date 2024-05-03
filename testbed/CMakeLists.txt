project(TestBed)

add_executable(TestBed
    src/testbed.cpp
)

target_link_libraries(TestBed PUBLIC Engine)