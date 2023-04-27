/**
  ******************************************************************************
  * @file           : NFA.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/27
  ******************************************************************************
  */

#ifndef GRAMMAR_NFA_H
#define GRAMMAR_NFA_H

#include "Cell.h"

/***************NFA的矩阵结构****************/

class NFA
{
public:
    explicit NFA();
    explicit NFA(const Cell& cell);

public:
    std::vector<Edge> edge_set_;
    size_t edge_count_{};  // 边数
    State start_state_;    //  开始状态
    State end_state_;    // 结束状态
    std::set<char> symbol_set_;  // 可接收符号集
};

/********************************************/


#endif //GRAMMAR_NFA_H
