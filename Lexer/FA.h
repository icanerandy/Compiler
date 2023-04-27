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
    NFA ExpressToNFA();    // 表达式转NFA
    Cell DoCell(char ch);  // 处理 a
    Cell DoUnit(const Cell& left, const Cell& right); // 处理 a|b
    Cell DoJoin(Cell& left, Cell& right); // 处理 ab
    Cell DoStar(const Cell& cell); // 处理 a*

private:
    DFA WorkList();    // 子集构造算法：工作表算法
    std::set<State> Closure(const State& state);  // 计算ε闭包
    static Edge Connect(const std::map<std::set<State>, size_t>& Q,
                 const std::set<State>& from, const std::set<State>& to,
                 char symbol);

private:
    State NewStateNode();   // 产生一个新的状态节点，便于管理
    static void CellEdgeSetCopy(Cell& des, const Cell& src);

private:
    std::string regex_;
    size_t state_num_;  // 保证全局唯一性
    NFA nfa_;
    DFA dfa_;
};


#endif //GRAMMAR_FA_H
