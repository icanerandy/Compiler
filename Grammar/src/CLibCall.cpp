/**
  ******************************************************************************
  * @file           : CLibCall.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/10
  ******************************************************************************
  */
//
// Created by icaner on 2023/5/10.
//

#include <utility>

#include "include/CLibCall.h"

CLibCall * CLibCall::instance = nullptr;

CLibCall::CLibCall()
{
    api_set.emplace("printf");
}

CLibCall *CLibCall::GetInstance()
{
    if (instance == nullptr)
        instance = new CLibCall();

    return instance;
}

bool CLibCall::IsApiCall(const std::string& func_name)
{
    return api_set.find(func_name) != api_set.end();
}

bool CLibCall::InvokeAPI(const std::string& func_name, std::list<ASTNode *> args)
{
    if (func_name == "printf")
    {
        return HandlePrintfCall(std::move(args));
    }
    else
    {
        return false;
    }
}

bool CLibCall::HandlePrintfCall(std::list<ASTNode *> args)
{
    std::string arg_str = args.front()->s_value_;
    arg_str.erase(std::find(arg_str.begin(), arg_str.end(), '"'));
    arg_str.erase(std::find(arg_str.begin(), arg_str.end(), '"'));
    args.pop_front();
    std::string format_str;

    int i = 0;
    int idx = 0;
    while (i < arg_str.length())
    {
        if (arg_str.at(i) == '%' && (i+1) < arg_str.length()
            && arg_str.at(i+1) == 'd')
        {
            ProgramGenerator::Emit(Instruction::GETSTATIC, "java/lang/System/out Ljava/io/PrintStream;");
            ProgramGenerator::Emit(Instruction::LDC, "\"" + format_str + "\"");
            ProgramGenerator::Emit(Instruction::INVOKEVIRTUAL, "java/io/PrintStream/print(Ljava/lang/String;)V");

            format_str.clear();
            ASTNode * tmp = args.front();
            args.pop_front();

            if (tmp->type_ == "<变量>")
            {
                ProgramGenerator::Emit(Instruction::GETSTATIC, "java/lang/System/out Ljava/io/PrintStream;");
                ProgramGenerator::Emit(Instruction::ILOAD, std::to_string(idx++));
                std::string print_method = "java/io/PrintStream/print(I)V";
                ProgramGenerator::Emit(Instruction::INVOKEVIRTUAL, print_method);
            }
            else if (tmp->type_ == "<常数>")
            {
                ProgramGenerator::Emit(Instruction::GETSTATIC, "java/lang/System/out Ljava/io/PrintStream;");
                ProgramGenerator::Emit(Instruction::ILOAD, std::to_string(tmp->i_value_));
                ProgramGenerator::Emit(Instruction::INVOKEVIRTUAL, "java/io/PrintStream/print(I)V");
            }
            i+=2;
        }
        else
        {
            format_str.append(1, arg_str.at(i));
            ++i;
        }
    }

    ProgramGenerator::Emit(Instruction::GETSTATIC, "java/lang/System/out Ljava/io/PrintStream;");
    ProgramGenerator::Emit(Instruction::LDC, "\"\n\"");
    ProgramGenerator::Emit(Instruction::INVOKEVIRTUAL, "java/io/PrintStream/print(I)V");
}
