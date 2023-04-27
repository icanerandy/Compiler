/**
  ******************************************************************************
  * @file           : DFA.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/27
  ******************************************************************************
  */

#ifndef GRAMMAR_DFA_H
#define GRAMMAR_DFA_H

#include "Cell.h"

/***************DFA的矩阵结构****************/

class DFA
{
public:
    explicit DFA();
    explicit DFA(const Cell& cell);

public:
    std::vector<Edge> edge_set_;
    size_t edge_count_{};  // 边数
    State start_state_;    // 开始状态
    std::set<State> end_state_;    // 结束状态集
    std::set<char> symbol_set_;  // 可接收符号集
};

/********************************************/


#endif //GRAMMAR_DFA_H
