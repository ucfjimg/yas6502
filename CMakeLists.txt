#[[
   Copyright 2020 Jim Geist.
  
   Permission is hereby granted, free of charge, to any person obtaining a copy 
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is furnished to do 
   so, subject to the following conditions:
  
   The above copyright notice and this permission notice shall be included in all 
   copies or substantial portions of the Software.
  
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
   INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
   PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
   CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
   OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
]]

cmake_minimum_required(VERSION 3.2)
project(yas6502)
set(CMAKE_CXX_STANDARD 14)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

bison_target(parser src/parser.yy ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp)
flex_target(scanner src/scanner.ll ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)

add_executable(yas6502 
    src/main.cpp
)

target_link_libraries(yas6502 yas6502l ${CORES_LIBRARIES})
target_include_directories(yas6502 PRIVATE src ${CMAKE_CURRENT_BINARY_DIR})

add_library(yas6502l
    src/assembler.cpp
    src/ast.cpp
    src/except.cpp
    src/expr.cpp
    src/listing.cpp
    src/opcodes.cpp
    src/pass.cpp
    src/pass1.cpp
    src/pass2.cpp
    src/symtab.cpp
    src/utility.cpp

    ${BISON_parser_OUTPUTS} 
    ${FLEX_scanner_OUTPUTS}
)

target_include_directories(yas6502l PRIVATE src ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS yas6502 DESTINATION bin)
install(TARGETS yas6502l DESTINATION lib)
install(FILES 
    "${PROJECT_SOURCE_DIR}/src/assembler.h"
    "${PROJECT_SOURCE_DIR}/src/ast.h"
    "${PROJECT_SOURCE_DIR}/src/except.h"
    "${PROJECT_SOURCE_DIR}/src/pass.h"
    "${PROJECT_SOURCE_DIR}/src/pass1.h"
    "${PROJECT_SOURCE_DIR}/src/pass2.h"
    "${PROJECT_SOURCE_DIR}/src/opcodes.h"
    "${PROJECT_SOURCE_DIR}/src/symtab.h"
    "${CMAKE_CURRENT_BINARY_DIR}/location.hh"
    DESTINATION include/yas6502)


