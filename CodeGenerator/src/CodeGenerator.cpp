/**
  ******************************************************************************
  * @file           : CodeGenerator.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/9
  ******************************************************************************
  */


#include "../include/CodeGenerator.h"

int CodeGenerator::instruction_count = 0;
std::string CodeGenerator::program_name = "CSourceToJava";
std::fstream CodeGenerator::assembly_file("../CodeGenerator/" + program_name + ".j", std::ios::out);

CodeGenerator::CodeGenerator()
{
    if (!assembly_file.is_open())
    {
        std::cerr << "无法打开汇编输出文件" << std::endl;
        system("pause");
        exit(-1);
    }
}

void CodeGenerator::EmitDirective(Directive directive)
{
    assembly_file << directive.ToString() << "\n";
    assembly_file.flush();
    ++instruction_count;
}

void CodeGenerator::EmitDirective(Directive directive, const std::string& operand)
{
    assembly_file << directive.ToString() << " " << operand << "\n";
    assembly_file.flush();
    ++instruction_count;
}

void CodeGenerator::EmitDirective(Directive directive, int operand)
{
    assembly_file << directive.ToString() << " " << operand << "\n";
    assembly_file.flush();
    ++instruction_count;
}

void CodeGenerator::EmitDirective(Directive directive, std::string operand1, std::string operand2)
{
    assembly_file << directive.ToString() << " " << operand1 << " " << operand2 << "\n";
    assembly_file.flush();
    ++instruction_count;
}

void CodeGenerator::EmitDirective(Directive directive, std::string operand1, std::string operand2, std::string operand3)
{
    assembly_file << directive.ToString() << " " << operand1 << " " << operand2 << " " << operand3 << "\n";
    assembly_file.flush();
    ++instruction_count;
}

void CodeGenerator::Emit(Instruction opcode)
{
    assembly_file << "\t" << opcode.ToString() << "\n";
    assembly_file.flush();
    ++instruction_count;
}

void CodeGenerator::Emit(Instruction opcode, const std::string& operand)
{
    assembly_file << "\t" << opcode.ToString() << "\t" << operand << "\n";
    assembly_file.flush();
    ++instruction_count;
}

void CodeGenerator::EmitString(const std::string& str)
{
    assembly_file << str;
    assembly_file.flush();
    ++instruction_count;
}

void CodeGenerator::EmitBlankLine()
{
    assembly_file << "\n";
    assembly_file.flush();
}

void CodeGenerator::Finish()
{
    assembly_file.close();
}


