cmake_minimum_required(VERSION 3.16)
project(ember CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ember
    
    #   --- MAIN DECLARATIONS
    src/main.cpp
    #   ----------------------
    
    src/ember.cpp
    src/memory.cpp
    src/dump.cpp
    src/display.cpp 
    src/cpu.cpp
    src/alu.cpp
    src/stack.cpp
    src/asm/lexer.cpp
    src/asm/lexer.cpp
    src/asm/assembler.cpp
)

add_executable(lexer_test
    src/asm/lexer_test.cpp
    src/asm/lexer.cpp
)

target_compile_options(lexer_test PRIVATE -Wall -Wextra -Werror)

target_compile_options(ember PRIVATE -Wall -Wextra -Werror)

# Sanitizers on Debug builds. They are extra checks compiled *into* the binary:
# walk off an array and the program prints a report and aborts, instead of
# quietly "working". They stay on all semester.

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT MSVC)
    target_compile_options(ember PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer -g)
    target_link_options(ember PRIVATE -fsanitize=address,undefined)
endif()
