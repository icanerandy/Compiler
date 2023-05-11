/**
  ******************************************************************************
  * @file           : ProgramGenerator.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/9
  ******************************************************************************
  */


#ifndef COMPILER_PROGRAMGENERATOR_H
#define COMPILER_PROGRAMGENERATOR_H

#include "CodeGenerator.h"
#include <stack>

class ProgramGenerator : public CodeGenerator
{
private:
    static ProgramGenerator * instance;

private:
    std::string embedded;

public:
    int branch_count;
    int loop_count;
    std::stack<int> if_stmt;
    std::stack<int> loop_stmt;

public:
    void EmitBranchOut();
    std::string GetBranchOut();
    std::string GetBranch();
    void IncreaseBranch();
    void IncreaseLoop();
    std::string GetLoop();

public:
    static ProgramGenerator * GetInstance();

private:
    ProgramGenerator();

public:
    void Generate();
    void GenerateMainMethod();
    void finish();
};


#endif //COMPILER_PROGRAMGENERATOR_H
