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

using RegexType = bool;  // true代表为运算符号，false代表为普通符
using RegexSymbol = std::pair<char, RegexType>;

/***************NFA的矩阵结构****************/

class NFA
{
public:
    explicit NFA();
    explicit NFA(const std::vector<RegexSymbol>& regex);
    void Display() const;

private:
    Cell ExpressToNFA(const std::vector<RegexSymbol>& regex);
    Cell DoCell(char ch);
    Cell DoUnit(const Cell& left, const Cell& right);
    Cell DoJoin(Cell& left, Cell& right);
    Cell DoStar(const Cell& cell);

private:
    State NewStateNode();
    static void CellEdgeSetCopy(Cell& des, const Cell& src);

public:
    std::set<Edge> edge_set_;
    size_t edge_count_{};
    State start_state_;
    State end_state_;
    std::set<char> symbol_set_;

private:
    size_t state_num_{};  // 保证全局唯一性
};

/********************************************/


#endif //COMPILER_NFA_H
