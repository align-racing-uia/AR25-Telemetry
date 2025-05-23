cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME Test2)
include_directories(${CMAKE_SOURCE_DIR}/Application)

# ...existing code...
# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")
include_directories(${CMAKE_SOURCE_DIR}/Application)


# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)
add_subdirectory(asl)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)


# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    asl
    asl_can
    # Add user defined libraries
)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined source files
    Core/Src/main.c
    Core/Src/app_freertos.c
    Core/Src/fdcan.c
    Core/Src/gpio.c
    Core/Src/usart.c
    #Core/Src/usb.c
    Core/Src/CAN_receive.c
    Core/Src/Transmitt.c # hvis den finnes
    Core/Src/VN-200.c
)
