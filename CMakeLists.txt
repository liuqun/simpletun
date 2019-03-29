cmake_minimum_required(VERSION 3.1)
project(simpletun LANGUAGES C)

file(WRITE "${PROJECT_BINARY_DIR}/platform.h.in" "/*(Auto generated from CMakeList.txt)*/
#pragma once
")

include(CheckIncludeFiles)
configure_file(${PROJECT_BINARY_DIR}/platform.h.in ${PROJECT_BINARY_DIR}/platform.h)

include_directories(${PROJECT_BINARY_DIR})
aux_source_directory(./src simpletun_SOURCES)
add_executable(simpletun ${simpletun_SOURCES})
