cmake_minimum_required(VERSION 3.22)
project(PhotoBeautyEngine VERSION 0.2.0 LANGUAGES C CXX)

option(PBE_ENABLE_VULKAN "Enable Vulkan backend when headers/libs are available" OFF)
option(PBE_ENABLE_NCNN "Enable ncnn AI backend integration" OFF)
option(PBE_BUILD_TESTS "Build native tests" ON)
option(PBE_BUILD_SHARED "Build shared library" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_library(pbe_core OBJECT
    src/api/pbe_api.cpp
    src/core/engine.cpp
    src/core/image.cpp
    src/core/color.cpp
    src/core/pipeline.cpp
    src/core/render_graph.cpp
    src/core/analyzer.cpp
    src/effects/basic.cpp
    src/effects/beauty.cpp
    src/effects/mask.cpp
    src/effects/local.cpp
    src/beauty/face_warp.cpp
    src/beauty/skin.cpp
    src/mask/mask.cpp
    src/ai/ai_backend.cpp
    src/gpu/gpu_backend.cpp
)

target_include_directories(pbe_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_target_properties(pbe_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(pbe_core PRIVATE PBE_BUILDING_LIBRARY=1)

if(ANDROID)
    target_compile_definitions(pbe_core PRIVATE PBE_ANDROID=1)
endif()

if(PBE_ENABLE_VULKAN)
    find_package(Vulkan QUIET)
    if(Vulkan_FOUND)
        target_sources(pbe_core PRIVATE src/gpu/vulkan_backend.cpp)
        target_compile_definitions(pbe_core PRIVATE PBE_ENABLE_VULKAN=1)
    else()
        message(FATAL_ERROR "PBE_ENABLE_VULKAN=ON but Vulkan was not found.")
    endif()
endif()

if(PBE_ENABLE_NCNN)
    target_compile_definitions(pbe_core PRIVATE PBE_ENABLE_NCNN=1)
    target_sources(pbe_core PRIVATE src/ai/ncnn_backend.cpp)
    # Expected integration:
    #   ncnn_DIR=<path containing ncnnConfig.cmake>
    #   or provide NCNN_INCLUDE_DIRS / NCNN_LIBRARIES.
    find_package(ncnn CONFIG REQUIRED)
endif()

if(PBE_BUILD_SHARED)
    add_library(PhotoBeautyEngine SHARED $<TARGET_OBJECTS:pbe_core>)
    target_include_directories(PhotoBeautyEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
else()
    add_library(PhotoBeautyEngine STATIC $<TARGET_OBJECTS:pbe_core>)
    target_include_directories(PhotoBeautyEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()

if(ANDROID)
    target_link_libraries(PhotoBeautyEngine PRIVATE log android)
endif()

if(PBE_ENABLE_VULKAN AND Vulkan_FOUND)
    target_link_libraries(PhotoBeautyEngine PRIVATE Vulkan::Vulkan)
endif()

if(PBE_ENABLE_NCNN)
    target_link_libraries(PhotoBeautyEngine PRIVATE ncnn)
endif()

set_target_properties(PhotoBeautyEngine PROPERTIES
    OUTPUT_NAME "photo_beauty"
    POSITION_INDEPENDENT_CODE ON
)

if(MSVC)
    target_compile_options(pbe_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(pbe_core PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

if(PBE_BUILD_TESTS AND NOT ANDROID)
    enable_testing()
    add_executable(pbe_tests tests/test_main.cpp)
    target_link_libraries(pbe_tests PRIVATE pbe_core)
    target_include_directories(pbe_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_test(NAME pbe_tests COMMAND pbe_tests)
endif()

install(TARGETS PhotoBeautyEngine
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)
