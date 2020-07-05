cmake_minimum_required(VERSION 3.5)

project(Simulador_Memoria LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(Simulador_Memoria
  src/main.cpp
  src/simulador_memoria.cpp
  src/memory.cpp
  src/animation.cpp
  src/memory_management.cpp
  include/memory.h
  include/simulador_memoria.h
  include/animation.h
  include/memory.h
  include/simulador_memoria.h
  include/memory_management.h
  include/defines.h
  ui/simulador_memoria.ui
  resources/icons.qrc
)

target_link_libraries(Simulador_Memoria PRIVATE Qt5::Widgets)
