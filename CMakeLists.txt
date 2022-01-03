cmake_minimum_required(VERSION 3.21)
project(Progetto_SO___Thread C)

set(CMAKE_C_STANDARD 99)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
add_executable(${PROJECT_NAME}  main.c area_menu.c collisioni.c collisioni.h stampa.c stampa.h generazione_movimenti.c generazione_movimenti.h macro.h)
target_link_libraries(${PROJECT_NAME} -lncursesw -lpthread)
