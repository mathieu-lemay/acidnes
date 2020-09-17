cmake_minimum_required(VERSION 3.16)
project(acidnes C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

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

target_compile_definitions(acidnes PRIVATE DEBUG)

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
