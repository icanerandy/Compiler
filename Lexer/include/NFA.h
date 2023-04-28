/**
  ******************************************************************************
  * @file           : NFA.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/27
  ******************************************************************************
  */

#ifndef COMPILER_NFA_H
#define COMPILER_NFA_H

#include "Cell.h"

/***************NFA的矩阵结构****************/

class NFA
{
public:
    explicit NFA();
    explicit NFA(const std::string& regex);
    void Display() const;

private:
    Cell ExpressToNFA(const std::string& regex);    // 表达式转NFA
    Cell DoCell(char ch);  // 处理 a
    Cell DoUnit(const Cell& left, const Cell& right); // 处理 a|b
    Cell DoJoin(Cell& left, Cell& right); // 处理 ab
    Cell DoStar(const Cell& cell); // 处理 a*

private:
    State NewStateNode();   // 产生一个新的状态节点，便于管理
    static void CellEdgeSetCopy(Cell& des, const Cell& src);

public:
    std::set<Edge> edge_set_;
    size_t edge_count_{};  // 边数
    State start_state_;    //  开始状态
    State end_state_;    // 结束状态
    std::set<char> symbol_set_;  // 可接收符号集

private:
    size_t state_num_{};  // 保证全局唯一性
};

/********************************************/


#endif //COMPILER_NFA_H
