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
    Bsp/i2c/bsp_i2c.c
    Modules/Controller/PID.c
    Modules/remote/remote.c
    Modules/message_center/message_center.c
    Modules/k230/module_k230.c
    Modules/motor/module_motor.c
    Modules/motor/bsp_motor_iic.c
    Modules/servo/module_servo.c
    Modules/IR/module_ir.c
    Modules/imu/BMI088_driver.c
    Modules/imu/BMI088.c
    Modules/key/module_key.c
    Modules/step/module_step.c
    Modules/imu/ins_task.c
    Modules/algorithm/kalman_filter.c
    Modules/algorithm/QuaternionEKF.c
    Modules/algorithm/user_lib.c
    App/gimbal/app_gimbal.c
    App/cmd/app_cmd.c
    App/chassis/app_chassis.c
    App/task_init.c
    Middlewares/Third_Party/SEGGER/RTT/SEGGER_RTT_printf.c
    Middlewares/Third_Party/SEGGER/RTT/SEGGER_RTT.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    Modules/INS
    Bsp/dwt
    Bsp/usart
    Bsp/daemon
    Bsp/log
    Bsp/i2c   
    Modules
    Modules/Controller
    Modules/remote
    Modules/message_center
    Modules/k230
    Modules/motor
    Modules/step
    Modules/servo
    Modules/imu
    Modules/key
    Modules/IR
    Modules/algorithm
    Modules/INS
    App/chassis
    App/cmd
    App/gimbal
    App
    Middlewares/Third_Party/SEGGER/Config
    Middlewares/Third_Party/SEGGER/RTT
    
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

# SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(ELFFILE ${CMAKE_PROJECT_NAME}.elf)
SET(HEXFILE ${CMAKE_PROJECT_NAME}.hex)
SET(OBJCOPY arm-none-eabi-objcopy)
SET(JFLASH JFlash)
# Add custom commands to build the project
ADD_CUSTOM_COMMAND(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Building ${CMAKE_PROJECT_NAME}"
    COMMAND ${CMAKE_COMMAND} -E echo "Generating ELF file: ${ELFFILE}"
    COMMAND ${CMAKE_COMMAND} -E echo "Generating HEX file: ${HEXFILE}"
    COMMAND ${OBJCOPY} -O ihex ${ELFFILE} ${HEXFILE}
    COMMAND ${CMAKE_COMMAND} -E echo "Build complete!"
    COMMAND ${CMAKE_COMMAND} -E echo "Flashing ${HEXFILE} to the device"
    COMMAND ${JFLASH} -openprj ${CMAKE_SOURCE_DIR}/stm32.jflash -open ${HEXFILE}, -8000000 -auto -startapp -exit
    COMMAND ${CMAKE_COMMAND} -E echo "Flashing complete!"
)

