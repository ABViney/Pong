cmake_minimum_required(VERSION 3.1)

set(proj Pong)

project(proj)

# add_subdirectory(external)
# include_directories(
#     external/SFML/include
#     .

# )

# set(All_LIBS
#     SFML

# )

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(${proj} main.cpp
    src/Pong.hpp
    src/Pong.cpp
)

target_link_libraries(${proj} sfml-graphics sfml-window sfml-system)#${ALL_LIBS})