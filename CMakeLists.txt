cmake_minimum_required (VERSION 3.10)
project(l)
set (CMAKE_CXX_STANDARD 20)

set(SOURCE main.cpp Printer.cpp InnerPrinter.cpp)
set(HEADER ConfigReader.h FileTimeChecker.h OptionParser.h Printer.h InnerPrinter.h)

IF (WIN32)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
ENDIF()

add_subdirectory(fmt)
include_directories(fmt)

add_executable(l ${SOURCE} ${HEADER})
target_link_libraries(l fmt)