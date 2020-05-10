cmake_minimum_required(VERSION 3.2)
project(yas6502)
set(CMAKE_CXX_STANDARD 14)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

bison_target(parser src/parser.yy ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp)
flex_target(scanner src/scanner.ll ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)

add_executable(yas6502 
    src/main.cpp

    src/assembler.cpp
    src/opcodes.cpp
    
    ${BISON_parser_OUTPUTS} 
    ${FLEX_scanner_OUTPUTS}
)
target_include_directories(yas6502 PRIVATE src ${CMAKE_CURRENT_BINARY_DIR})

