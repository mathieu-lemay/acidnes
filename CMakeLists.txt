cmake_minimum_required(VERSION 3.16)
project(acidnes C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(acidnes
        src/cartridge.c
        src/cartridge.h
        src/common.c
        src/common.h
        src/cpu.c
        src/cpu.h
        src/main.c
        src/mapper.c
        src/mapper.h
        src/opcodes.c
        src/opcodes.h
        src/ppu.c
        src/ppu.h
        src/types.h)

target_link_libraries(acidnes ${SDL2_LIBRARIES})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(acidnes PRIVATE DEBUG)
endif()

add_executable(tests
        src/cartridge.c
        src/cartridge.h
        src/common.c
        src/common.h
        src/cpu.c
        src/cpu.h
        src/mapper.c
        src/mapper.h
        src/opcodes.c
        src/opcodes.h
        src/ppu.c
        src/ppu.h
        src/types.h
        tests/main.c)
target_link_libraries(tests ${SDL2_LIBRARIES})
