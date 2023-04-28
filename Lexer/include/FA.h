/**
  ******************************************************************************
  * @file           : FA.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/26
  ******************************************************************************
  */

#ifndef COMPILER_FA_H
#define COMPILER_FA_H

#include "NFA.h"
#include "DFA.h"

class FA
{
public:
    explicit FA(std::string regex);
    bool Judge(const std::string& str);

private:
    bool CheckLegal();
    bool CheckCharacter();
    bool CheckParenthesis();
    void AddJoinSymbol();
    void PostFix();
    bool IsOperator(char c);
    int GetPriority(char ch);

private:
    std::string regex_;
    NFA nfa_;
    DFA dfa_;
};


#endif //COMPILER_FA_H
