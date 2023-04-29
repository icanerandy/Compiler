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

#include <map>
#include <ostream>
#include <fstream>
#include <sstream>
#include "NFA.h"
#include "DFA.h"

using RegexType = bool;  // true代表为运算符号，false代表为普通符
using RegexSymbol = std::pair<char, RegexType>;

class FA
{
public:
    explicit FA();
    explicit FA(const std::string& FA_name);   // 从状态转换表文件中读取daf_table到dfa_中的dfa_table_
    explicit FA(std::string FA_name, std::string regex);    // 根据正则表达式生成dfa_table

public:
    std::string FA_name_;    // 状态表名称
    static std::string table_post_name_;  // dfa表输出文件名后缀
    DFA GetDFA();

private:
    void OutputDFATable();

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
