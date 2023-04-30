/**
  ******************************************************************************
  * @file           : DFA.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/27
  ******************************************************************************
  */

#ifndef COMPILER_DFA_H
#define COMPILER_DFA_H

#include "Cell.h"
#include "NFA.h"

/***************DFA的矩阵结构****************/

class DFA
{
public:
    explicit DFA();
    explicit DFA(const NFA& nfa);
public:
    void Display();
    bool Judge(const std::string& str);
    State GetStartState() const;    // 获取开始状态
    std::set<State> GetEndStateSet() const;    // 获取结束状态集
    std::map<State, std::map<char, State>>& GetDFATable();   // 获取状态转换表

private:    // NFA-DFA
    void NFA2DFA();
    Cell WorkList();    // 子集构造算法：工作表算法
    std::set<State> Closure(const State& state);  // 计算ε闭包
    static Edge Connect(const std::map<std::set<State>, size_t>& Q,
                        const std::set<State>& from, const std::set<State>& to,
                        char symbol);

private:    // DFA最小化
    void DFA2MFA();
    Cell Hopcroft();
    bool Split(std::map<size_t, std::set<State>>& categories);
    static bool IsSubSet(std::map<size_t, std::set<State>>& categories, const std::set<State>& cur_set);
    static size_t ToWhichSet(std::map<size_t, std::set<State>>& categories, State state);
    Cell CreateMFABySets(std::map<size_t, std::set<State>>& categories);

public:
    std::set<Edge> edge_set_;
    size_t edge_count_{};  // 边数
    std::set<State> state_set_;
    State start_state_;    // 开始状态
    std::set<State> end_state_;    // 结束状态集
    std::set<char> symbol_set_;  // 可接收符号集

public:
    std::map<State, std::map<char, State>> dfa_table_;  // 状态转换图对应的状态转换表

private:
    NFA nfa_;   // 原nfa
    size_t state_num_{};  // 保证全局唯一性
};

/********************************************/


#endif //COMPILER_DFA_H
