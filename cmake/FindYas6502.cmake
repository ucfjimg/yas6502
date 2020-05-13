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

set(YAS6502_ROOT_DIR
    "${YAS6502_ROOT_DIR}"
	CACHE
	PATH
	"Directory to search")

find_library(YAS6502_LIBRARY
	NAMES
	yas6502l
	HINTS
    "${YAS6502_ROOT_DIR}"
    PATH_SUFFIXES
    lib/)

find_path(YAS6502_INCLUDE_DIR
	NAMES
    yas6502/assembler.h
	PATHS
    "${YAS6502_ROOT_DIR}"
	PATH_SUFFIXES
	include/)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Yas6502
	DEFAULT_MSG
    YAS6502_LIBRARY
    YAS6502_INCLUDE_DIR)

if(YAS6502_FOUND)
    set(YAS6502_LIBRARIES ${YAS6502_LIBRARY})
	set(YAS6502_INCLUDE_DIRS "${YAS6502_INCLUDE_DIR}")
	mark_as_advanced(YAS6502_ROOT_DIR)
endif()

mark_as_advanced(YAS6502_INCLUDE_DIR YAS6502_LIBRARY)
