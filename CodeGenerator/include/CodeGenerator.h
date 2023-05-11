/**
  ******************************************************************************
  * @file           : CodeGenerator.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/9
  ******************************************************************************
  */


#ifndef COMPILER_CODEGENERATOR_H
#define COMPILER_CODEGENERATOR_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "Directive.h"
#include "Instruction.h"

class CodeGenerator
{
private:
    static int instruction_count;   // 指令条目数

protected:
    // 输出文件名
    static std::fstream assembly_file;

public:
    CodeGenerator();
    static void EmitDirective(Directive directive);
    static void EmitDirective(Directive directive, const std::string& operand);
    static void EmitDirective(Directive directive, int operand);
    static void EmitDirective(Directive directive, std::string operand1, std::string operand2);
    static void EmitDirective(Directive directive, std::string operand1, std::string operand2, std::string operand3);
    static void Emit(Instruction opcode);
    static void Emit(Instruction opcode, const std::string& operand);
    static void EmitString(const std::string& str);
    static void EmitBlankLine();
    static void Finish();

    static std::string program_name;
};


#endif //COMPILER_CODEGENERATOR_H
