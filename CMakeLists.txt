cmake_minimum_required(VERSION 2.8)
#Déclaration du projet

project(Snake)
add_executable(Snake main.c)



#Déclaration de l'exécutable

find_package(SDL REQUIRED)
if (SDL_FOUND)
  include_directories(${SDL_INCLUDE_DIR})
  target_link_libraries (Snake ${SDL_LIBRARY})
endif (SDL_FOUND)

if( WITH_COREDEBUG )
  add_definitions(-ggdb3)
  message(STATUS "GCC: Debug-flags set (-ggdb3)")
endif()
