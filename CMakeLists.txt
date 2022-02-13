cmake_minimum_required(VERSION 3.12)

project(VALX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(VALX_BUILD_EXAMPLES "build examples" ON)

set(SOURCES
"backend/vulkan/VulkanContext.cpp"
"backend/vulkan/ExternalFunctions.cpp"
"api/Context.cpp" 
"api/Logger.cpp"  
"window/Window.cpp" 
"window/vulkan/VulkanSurface.cpp" 
"backend/vulkan/VulkanSwapChain.cpp" 
"backend/vulkan/VulkanCommandBuffer.cpp"  
"backend/vulkan/VulkanTexture.cpp" 
"backend/vulkan/VulkanFormat.cpp" 
"api/Utilities.h" 
"backend/vulkan/VulkanBuffer.cpp" 
"backend/vulkan/VulkanShader.cpp" "backend/vulkan/VulkanShaderLoader.cpp" "api/TextureLoader.cpp" "api/Format.cpp" "backend/vulkan/VulkanSampler.cpp")

find_package(Vulkan REQUIRED FATAL_ERROR)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/glslang)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/glfw)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/submodules/fmt)

set(GLM_INCLUDE_DIR           ${CMAKE_CURRENT_SOURCE_DIR}/submodules/glm)
set(GLFW_INCLUDE_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/submodules/glfw/include)
set(IMGUI_INCLUDE_DIR         ${CMAKE_CURRENT_SOURCE_DIR}/submodules/imgui)
set(GLSLANG_INCLUDE_DIR       ${CMAKE_CURRENT_SOURCE_DIR}/submodules/glslang/glslang/Public {CMAKE_CURRENT_SOURCE_DIR}/submodules/glslang/SPIRV)
set(VMA_INCLUDE_DIR           ${CMAKE_CURRENT_SOURCE_DIR}/submodules/VulkanMemoryAllocator/include)
set(TINYOBJLOADER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/submodules/tinyobjloader)
set(SPAN_INCLUDE_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/submodules/span/include)
set(SPIRV_REFLECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/submodules/spirv-reflect)
set(STB_INCLUDE_DIR           ${CMAKE_CURRENT_SOURCE_DIR}/submodules/stb)
set(TINYGLTF_INCLUDE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/submodules/tinygltf)
set(TINYDDSLOADER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/submodules/tinyddsloader)
set(FMT_INCLUDE_DIR           ${CMAKE_CURRENT_SOURCE_DIR}/submodules/fmt/include)

if(WIN32)
    set_target_properties(glfw PROPERTIES IMPORT_SUFFIX ".lib")
endif()

add_library(VALX STATIC ${SOURCES})

set(VALX_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Vulkan_INCLUDE_DIR} 
    ${GLM_INCLUDE_DIR} 
    ${GLFW_INCLUDE_DIR} 
    ${IMGUI_INCLUDE_DIR}
    ${VMA_INCLUDE_DIR}
    ${GLSLANG_INCLUDE_DIR}
    ${TINYOBJLOADER_INCLUDE_DIR}
    ${SPAN_INCLUDE_DIR}
    ${SPIRV_REFLECT_INCLUDE_DIR}
    ${STB_INCLUDE_DIR}
    ${TINYGLTF_INCLUDE_DIR}
    ${TINYDDSLOADER_INCLUDE_DIR}
    ${FMT_INCLUDE_DIR}
)

get_directory_property(HAS_PARENT PARENT_DIRECTORY)
if(HAS_PARENT)
    set(VALX_INCLUDE_DIR ${VALX_INCLUDE_DIR} PARENT_SCOPE)
endif()

target_include_directories(VALX PUBLIC ${VALX_INCLUDE_DIR})
target_link_libraries(VALX PUBLIC ${Vulkan_LIBRARIES} glfw MachineIndependent SPIRV fmt)

# examples
if(VALX_BUILD_EXAMPLES)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
endif()