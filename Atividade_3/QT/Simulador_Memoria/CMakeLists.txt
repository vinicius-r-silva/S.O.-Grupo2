cmake_minimum_required(VERSION 3.5)

project(Simulador_Memoria LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(Simulador_Memoria
  src/main.cpp
  src/simulador_memoria.cpp
  src/memory.cpp
  src/animation.cpp
  src/memory_management.cpp
  src/process.cpp
  src/interpreter.cpp
  include/memory.h
  include/simulador_memoria.h
  include/animation.h
  include/memory.h
  include/simulador_memoria.h
  include/memory_management.h
  include/defines.h
  include/process.h
  include/interpreter.h
  ui/simulador_memoria.ui
  resources/icons.qrc
)

target_link_libraries(Simulador_Memoria PRIVATE Qt5::Widgets)
