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

using RegexType = bool;  // true����Ϊ������ţ�false����Ϊ��ͨ��
using RegexSymbol = std::pair<char, RegexType>;

class FA
{
public:
    explicit FA();
    explicit FA(const std::string& FA_name);   // ��״̬ת�����ļ��ж�ȡdaf_table��dfa_�е�dfa_table_
    explicit FA(std::string FA_name, std::string regex);    // ����������ʽ����dfa_table

public:
    std::string FA_name_;    // ״̬������
    static std::string table_post_name_;  // dfa������ļ�����׺
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
