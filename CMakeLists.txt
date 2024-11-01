set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/submodules/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

cmake_minimum_required(VERSION 3.20)
project(cprg-game VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DUNICODE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>")

set(CMAKE_OBJECT_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/obj/$<CONFIG>")

# Define configuration-specific output paths
set(CONFIG_RUNTIME_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>")
set(CONFIG_D3D12_DIR "${CONFIG_RUNTIME_OUTPUT_DIR}/D3D12")

find_package(directx-dxc CONFIG REQUIRED)
find_package(directx-headers CONFIG REQUIRED)
find_package(directx-headers CONFIG REQUIRED)
find_package(directx12-agility CONFIG REQUIRED)
find_package(directxmath CONFIG REQUIRED)
find_package(directxtk CONFIG REQUIRED)
find_package(directxtk12 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Taskflow CONFIG REQUIRED)
find_package(meshoptimizer CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

add_executable(${PROJECT_NAME} main.cpp "Source/Engine/Engine.hpp" "Source/Engine/Engine.cpp" "Source/Engine/Subsystems/Logger.hpp" "Source/Engine/Subsystems/Window.hpp" "Source/Engine/Subsystems/Window.cpp")

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /utf-8 /MP)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

target_include_directories(cprg-game PRIVATE "${CMAKE_SOURCE_DIR}/Source")

target_link_libraries(cprg-game PRIVATE Microsoft::DirectXShaderCompiler)
target_link_libraries(cprg-game PRIVATE Microsoft::DirectX-Headers)
target_link_libraries(cprg-game PRIVATE Microsoft::DirectX-Headers Microsoft::DirectX-Guids Microsoft::DirectX12-Agility)
target_link_libraries(cprg-game PRIVATE Microsoft::DirectXMath)
target_link_libraries(cprg-game PRIVATE Microsoft::DirectXTK)
target_link_libraries(cprg-game PRIVATE Microsoft::DirectXTK12)
target_link_libraries(cprg-game PRIVATE spdlog::spdlog)
target_link_libraries(cprg-game PRIVATE Taskflow::Taskflow)
target_link_libraries(cprg-game PRIVATE meshoptimizer::meshoptimizer)
target_link_libraries(cprg-game PRIVATE assimp::assimp)

if(TARGET Microsoft::DirectX12-Agility)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CONFIG_D3D12_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PROPERTY:Microsoft::DirectX12-Core,IMPORTED_LOCATION_RELEASE> "${CONFIG_D3D12_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PROPERTY:Microsoft::DirectX12-Layers,IMPORTED_LOCATION_DEBUG> "${CONFIG_D3D12_DIR}"
        COMMAND_EXPAND_LISTS
    )
endif()