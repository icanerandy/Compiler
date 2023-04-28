cmake_minimum_required(VERSION 3.24)
project(Compiler)

set(CMAKE_CXX_STANDARD 14)

include_directories(Compiler)
include_directories(Grammar)
include_directories(Lexer)

add_executable(Compiler
        Grammar/src/Grammar.cpp
        Grammar/include/Grammar.h
        Grammar/src/LL1Parser.cpp
        Grammar/include/LL1Parser.h
        main.cpp Lexer/src/FA.cpp Lexer/include/FA.h Lexer/src/NFA.cpp Lexer/include/NFA.h Lexer/include/Cell.h Lexer/src/DFA.cpp Lexer/include/DFA.h Lexer/src/Lexer.cpp Lexer/include/Lexer.h Lexer/include/Type.h)
