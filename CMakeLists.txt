cmake_minimum_required(VERSION 3.6)
project(ERE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES FA.cpp Automata.cpp BREParser.cpp Set.cpp ERE.cpp)
add_executable(ERE ${SOURCE_FILES})
target_link_libraries(ERE libboost_program_options-mt.a)