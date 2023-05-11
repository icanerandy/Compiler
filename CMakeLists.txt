cmake_minimum_required(VERSION 3.24)
project(Compiler)

set(CMAKE_CXX_STANDARD 14)

include_directories(Compiler)
include_directories(Grammar)
include_directories(Lexer)
include_directories(Semantic)
include_directories(CodeGenerator)

add_executable(Compiler
        main.cpp Lexer/src/FA.cpp Lexer/include/FA.h Lexer/src/NFA.cpp Lexer/include/NFA.h Lexer/include/Cell.h Lexer/src/DFA.cpp Lexer/include/DFA.h
        Lexer/src/Lexer.cpp Lexer/include/Lexer.h
        Lexer/src/Util.cpp Lexer/include/Util.h
        Grammar/include/LR1Parser.h Grammar/src/LR1Parser.cpp Grammar/src/AST.cpp Grammar/include/AST.h
        Semantic/src/Symbol.cpp Semantic/include/Symbol.h Semantic/src/SymbolEntry.cpp Semantic/include/SymbolEntry.h Semantic/src/Declarator.cpp Semantic/include/Declarator.h Semantic/src/Specifier.cpp Semantic/include/Specifier.h Semantic/src/TypeLink.cpp Semantic/include/TypeLink.h Semantic/src/TypeSystem.cpp Semantic/include/TypeSystem.h
        CodeGenerator/src/CodeGenerator.cpp CodeGenerator/include/CodeGenerator.h CodeGenerator/src/Directive.cpp CodeGenerator/include/Directive.h CodeGenerator/src/Instruction.cpp CodeGenerator/include/Instruction.h CodeGenerator/src/ProgramGenerator.cpp CodeGenerator/include/ProgramGenerator.h Grammar/src/CLibCall.cpp Grammar/include/CLibCall.h)
