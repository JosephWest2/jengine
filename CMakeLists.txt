cmake_minimum_required(VERSION 4.1)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PROJECT_NAME jengine)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(${PROJECT_NAME})

include(FetchContent)

cmake_policy(SET CMP0135 NEW)

find_package(Vulkan REQUIRED)

FetchContent_Declare(
    sdl3
    URL https://github.com/libsdl-org/SDL/releases/download/release-3.2.26/SDL3-3.2.26.tar.gz
    URL_HASH SHA256=dad488474a51a0b01d547cd2834893d6299328d2e30f479a3564088b5476bae2
)

FetchContent_Declare(
    vkbootstrap
    URL https://github.com/charles-lunarg/vk-bootstrap/archive/refs/tags/v1.4.330.tar.gz
    URL_HASH SHA256=a3cfe83a297feea9c5229e6b373b48b0f4797da018217979794e1edc9686243f
)

FetchContent_Declare(
    abseil
    URL https://github.com/abseil/abseil-cpp/releases/download/20250814.1/abseil-cpp-20250814.1.tar.gz
    URL_HASH SHA256=1692f77d1739bacf3f94337188b78583cf09bab7e420d2dc6c5605a4f86785a1
)

FetchContent_Declare(
    vma
    URL https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/archive/refs/tags/v3.3.0.tar.gz
    URL_HASH SHA256=c4f6bbe6b5a45c2eb610ca9d231158e313086d5b1a40c9922cb42b597419b14e
)

FetchContent_MakeAvailable(
    sdl3
    vkbootstrap
    abseil
    vma
)

add_library(${PROJECT_NAME} 
    src/renderer/two_dimensional.cpp
    src/renderer/vulkan/initializers.cpp
    src/renderer/vulkan/graphics_queue.cpp
    src/renderer/vulkan/image.cpp
    src/renderer/vulkan/physical_device.cpp
    src/renderer/vulkan/swapchain.cpp
    src/renderer/vulkan/device.cpp
    src/renderer/vulkan/surface.cpp
    src/renderer/vulkan/instance.cpp
    src/renderer/vulkan/memory_allocator.cpp
    src/renderer/three_dimensional.cpp
    src/engine.cpp
    src/window.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(${PROJECT_NAME}
    PRIVATE src
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC Vulkan::Vulkan
    PUBLIC SDL3::SDL3
    PUBLIC vk-bootstrap::vk-bootstrap
    PUBLIC absl::flat_hash_map
    PUBLIC VulkanMemoryAllocator
)

add_executable(${PROJECT_NAME}_example example/main.cpp)
target_link_libraries(${PROJECT_NAME}_example ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME}_example PRIVATE src)

find_program(GLSL_VALIDATOR glslangValidator)

if(NOT GLSL_VALIDATOR)
    message(FATAL_ERROR "glslangValidator not found. Please install the Vulkan SDK or ensure it's in your PATH.")
endif()

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/shaders/*.frag"
    "${PROJECT_SOURCE_DIR}/shaders/*.vert"
    "${PROJECT_SOURCE_DIR}/shaders/*.comp"
)

set(SPIRV_OUTPUT_DIR "${PROJECT_BINARY_DIR}/shaders")

foreach(GLSL_FILE ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME_WITH_EXT "${GLSL_FILE}" NAME)
    
    set(SPIRV_FILE "${SPIRV_OUTPUT_DIR}/${FILE_NAME_WITH_EXT}.spv")
    
    add_custom_command(
        OUTPUT ${SPIRV_FILE}
        COMMAND ${GLSL_VALIDATOR} -V ${GLSL_FILE} -o ${SPIRV_FILE}
        DEPENDS ${GLSL_FILE}
        COMMENT "Compiling GLSL: ${FILE_NAME_WITH_EXT}"
        VERBATIM
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV_FILE})
endforeach()

add_custom_target(
    Shaders ALL
    DEPENDS ${SPIRV_BINARY_FILES}
)
