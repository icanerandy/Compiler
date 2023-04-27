cmake_minimum_required(VERSION 3.24)
project(Grammar)

set(CMAKE_CXX_STANDARD 14)

include_directories(Grammar)
include_directories(LL1Parser)

add_executable(Grammar
        Grammar/src/Grammar.cpp
        Grammar/include/Grammar.h
        Grammar/src/LL1Parser.cpp
        Grammar/include/LL1Parser.h
        main.cpp Lexer/FA.cpp Lexer/FA.h Lexer/NFA.cpp Lexer/NFA.h Lexer/Cell.h Lexer/DFA.cpp Lexer/DFA.h)
