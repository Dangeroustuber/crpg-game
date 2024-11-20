set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/submodules/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

cmake_minimum_required(VERSION 3.20)
project(crpg-game VERSION 1.0.0 LANGUAGES CXX)

set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DUNICODE)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

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
find_package(d3d12-memory-allocator CONFIG REQUIRED)

add_executable(${PROJECT_NAME} main.cpp 
    "Source/pch.h"
    "Source/Game/Game.cpp"
    "Source/Engine/Engine.cpp"
    "Source/Engine/Subsystems/Logger.hpp"
    "Source/Engine/Subsystems/Window.cpp" 
    "Source/Utils/Utils.cpp" 
    "Source/Utils/IniFileParser.cpp" 
    "Source/Engine/Subsystems/Graphics/DX12/Device.cpp"
    "Source/Engine/Subsystems/Graphics/DX12/SwapChain.cpp"
    "Source/Engine/Subsystems/Graphics/DX12/Fence.cpp"
    "Source/Engine/Subsystems/Graphics/DX12/CommandQueue.cpp"
    "Source/Engine/Subsystems/Graphics/DX12/Allocator.cpp" 
    "Source/Engine/Subsystems/Graphics/DX12/GraphicsContext.cpp"
    "Source/Engine/Subsystems/Graphics/DX12/Shader.cpp"
    "Source/Engine/Subsystems/Graphics/DX12/DescriptorHeap.cpp" 
    "Source/Engine/Subsystems/Graphics/DX12/UploadHeap.cpp" 
    "Source/Engine/Subsystems/Graphics/DX12/Mesh.cpp"
    "Source/Engine/Subsystems/Graphics/DX12/ResourceManager.cpp"
    "Source/Engine/Camera/Camera.cpp"
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /utf-8 /MP)
    target_link_options(${PROJECT_NAME} PRIVATE /TIME)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/Source")

target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::DirectXShaderCompiler)
target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::DirectX-Headers)
target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::DirectX-Headers Microsoft::DirectX-Guids Microsoft::DirectX12-Agility)
target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::DirectXMath)
target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::DirectXTK)
target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::DirectXTK12)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE Taskflow::Taskflow)
target_link_libraries(${PROJECT_NAME} PRIVATE meshoptimizer::meshoptimizer)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)
target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::D3D12MemoryAllocator)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        d3d12.lib
        dxgi.lib
        # dxcompiler.lib
        dxguid.lib
    )
endif()

if(TARGET Microsoft::DirectX12-Agility)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CONFIG_D3D12_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PROPERTY:Microsoft::DirectX12-Core,IMPORTED_LOCATION_RELEASE> "${CONFIG_D3D12_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PROPERTY:Microsoft::DirectX12-Layers,IMPORTED_LOCATION_DEBUG> "${CONFIG_D3D12_DIR}"
        COMMAND_EXPAND_LISTS
    )
endif()

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/config.ini
        ${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/config.ini
    COMMAND ${CMAKE_COMMAND} -E echo "Copied engine configuration file to bin directory"
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Assets
        ${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/Source/Assets
    COMMAND ${CMAKE_COMMAND} -E echo "Copied Assets to the bin directory"
)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/External/dxil.dll
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/dxil.dll
        COMMAND ${CMAKE_COMMAND} -E echo "Copied dxil.dll"
)

target_precompile_headers(${PROJECT_NAME} 
    PRIVATE Source/pch.h
)