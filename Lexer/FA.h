/**
  ******************************************************************************
  * @file           : FA.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/26
  ******************************************************************************
  */

#ifndef GRAMMAR_FA_H
#define GRAMMAR_FA_H

#include "NFA.h"
#include "DFA.h"

class FA
{
public:
    explicit FA(std::string regex);

private:
    bool CheckLegal();
    bool CheckCharacter();
    bool CheckParenthesis();
    void AddJoinSymbol();
    void PostFix();
    static int Isp(char ch);
    static int Icp(char ch);

private:
    std::string regex_;
    NFA nfa_;
    DFA dfa_;
};


#endif //GRAMMAR_FA_H
