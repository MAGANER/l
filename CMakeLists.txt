cmake_minimum_required (VERSION 3.10)
project(l)
set (CMAKE_CXX_STANDARD 17)

set(SOURCE main.cpp Printer.cpp)
set(HEADER Colorizer.h ConfigReader.h FileTimeChecker.h OptionParser.h Printer.h)

add_executable(l ${SOURCE} ${HEADER})