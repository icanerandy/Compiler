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
    void DisplayNFA();
    void DisplayDFA();

private:
    bool CheckLegal();
    bool CheckCharacter();
    bool CheckParenthesis();
    void AddJoinSymbol();
    void PostFix();
    static int Isp(char ch);
    static int Icp(char ch);

private:
    NFA ExpressToNFA();    // ���ʽתNFA
    Cell DoCell(char ch);  // ���� a
    Cell DoUnit(const Cell& left, const Cell& right); // ���� a|b
    Cell DoJoin(Cell& left, Cell& right); // ���� ab
    Cell DoStar(const Cell& cell); // ���� a*

private:
    DFA WorkList();    // �Ӽ������㷨���������㷨
    std::set<State> Closure(const State& state);  // ����űհ�
    static Edge Connect(const std::map<std::set<State>, size_t>& Q,
                 const std::set<State>& from, const std::set<State>& to,
                 char symbol);

private:
    State NewStateNode();   // ����һ���µ�״̬�ڵ㣬���ڹ���
    static void CellEdgeSetCopy(Cell& des, const Cell& src);

private:
    std::string regex_;
    size_t state_num_;  // ��֤ȫ��Ψһ��
    NFA nfa_;
    DFA dfa_;
};


#endif //GRAMMAR_FA_H
