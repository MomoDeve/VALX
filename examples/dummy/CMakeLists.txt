set(SOURCES 
"EntryPoint.cpp"
)

add_executable(dummy ${SOURCES})

target_link_directories(dummy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(dummy PUBLIC VALX)

target_include_directories(dummy PUBLIC ${VULKAN_ABSTRACTION_LAYER_INCLUDE_DIR})

target_compile_definitions(dummy PUBLIC -D APPLICATION_WORKING_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}")