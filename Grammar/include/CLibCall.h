/**
  ******************************************************************************
  * @file           : CLibCall.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/10
  ******************************************************************************
  */


#ifndef COMPILER_CLIBCALL_H
#define COMPILER_CLIBCALL_H

#include <string>
#include <list>
#include <set>

#include "include/Symbol.h"
#include "include/ProgramGenerator.h"
#include "include/AST.h"

class CLibCall
{
private:
    std::set<std::string> api_set;
    static CLibCall * instance;

private:
    CLibCall();

public:
    static CLibCall * GetInstance();
    bool IsApiCall(const std::string& func_name);
    bool InvokeAPI(const std::string& func_name, std::list<ASTNode *> args);

private:
    bool HandlePrintfCall(std::list<ASTNode *> args);
};


#endif //COMPILER_CLIBCALL_H
