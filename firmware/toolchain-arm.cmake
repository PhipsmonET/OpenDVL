# ==============================================================================
# OpenDVL - CMake Toolchain file for ARM Cortex-M7 (STM32H7)
# ==============================================================================

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Force compiler paths if not automatically detected in system PATH
# (Can be overridden by environment variables)
if(NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER arm-none-eabi-gcc)
endif()

if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
endif()

set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

# Compiler flags for STM32H7xx (Cortex-M7, hardware double-precision FPU, thumb instructions)
set(CPU_FLAGS "-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard")

set(CMAKE_C_FLAGS "${CPU_FLAGS} -Wall -Wextra -fdata-sections -ffunction-sections" CACHE INTERNAL "C Compiler flags")
set(CMAKE_CXX_FLAGS "${CPU_FLAGS} -Wall -Wextra -fdata-sections -ffunction-sections -fno-exceptions -fno-rtti" CACHE INTERNAL "C++ Compiler flags")
set(CMAKE_ASM_FLAGS "${CPU_FLAGS} -x assembler-with-cpp" CACHE INTERNAL "ASM Compiler flags")

# Linker flags (garbage collection of unused sections, specs file for standard C library)
set(CMAKE_EXE_LINKER_FLAGS "${CPU_FLAGS} -Wl,--gc-sections --specs=nano.specs -Wl,-Map=${CMAKE_PROJECT_NAME}.map" CACHE INTERNAL "Linker flags")

# Adjust the search behavior of the find_xxx() commands:
# search programs in the host environment, but libraries and headers in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
