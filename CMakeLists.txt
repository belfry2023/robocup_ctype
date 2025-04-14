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
set(CMAKE_PROJECT_NAME robocup_ctype)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

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

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    Bsp/dwt/bsp_dwt.c
    Bsp/usart/bsp_usart.c
    Bsp/daemon/daemon.c
    Bsp/log/bsp_log.c
    Bsp/can/bsp_can.c
    Modules/Controller/PID.c
    Modules/motor/motor.c
    Modules/remote/remote.c
    Modules/message_center/message_center.c
    Modules/Vision/master_process.c
    Modules/Vision/seasky_protocol.c
    Modules/Crc/crc8.c
    Modules/Crc/crc16.c
    Modules/can_comm/can_comm.c
    Modules/INS/BMI088_driver.c
    Modules/INS/BMI088.c
    Modules/INS/ins_task.c
    Modules/INS/kalman_filter.c
    Modules/INS/QuaternionEKF.c
    Modules/INS/user_lib.c
    Middlewares/Third_Party/SEGGER/RTT/SEGGER_RTT_printf.c
    Middlewares/Third_Party/SEGGER/RTT/SEGGER_RTT.c
    App/Init/init.c
    App/Car/car.c
    App/Cmd/cmd.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    Modules/INS
    Bsp/dwt
    Bsp/usart
    Bsp/daemon
    Bsp/can
    Bsp/log
    Modules/Controller
    Modules/motor
    Modules/remote
    Modules/message_center
    Modules/Vision
    Modules/Crc
    Modules/can_comm
    Middlewares/Third_Party/SEGGER/Config
    Middlewares/Third_Party/SEGGER/RTT
    App/Car
    App/Cmd
    App/Init
    App
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

set(ELF_FILE ${CMAKE_PROJECT_NAME}.elf)
set(HEX_FILE ${CMAKE_PROJECT_NAME}.hex)
set(elfPath arm-none-eabi-objcopy.exe)
set(downPath JFlash.exe)
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${elfPath} -O ihex ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.hex
)
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${downPath} -openprj ${PROJECT_SOURCE_DIR}/stm32.jflash -open ${CMAKE_PROJECT_NAME}.hex, 0x08000000 -auto -startapp -exit
)

