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

using RegexType = bool;  // true代表为运算符号，false代表为普通符
using RegexSymbol = std::pair<char, RegexType>;

class FA
{
public:
    explicit FA();
    explicit FA(std::string regex);

public:
    DFA GetDFA();

private:
    bool CheckLegal();
    bool CheckCharacter();
    bool CheckParenthesis();
    void AddJoinSymbol();
    void PostFix();
    static bool IsOperator(char c);
    static int GetPriority(RegexSymbol symbol);

private:
    std::vector<RegexSymbol> regex_;
    NFA nfa_;
    DFA dfa_;
};


#endif //COMPILER_FA_H
