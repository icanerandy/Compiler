/**
  ******************************************************************************
  * @file           : DFA.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/27
  ******************************************************************************
  */

#include "DFA.h"

DFA::DFA()
= default;

DFA::DFA(const Cell &cell)
    :
    edge_set_(cell.edge_set),
    edge_count_(cell.edge_set.size()),
    start_state_(1),
    end_state_(cell.end_state_set)
{

    for (const auto& edge : cell.edge_set)
        symbol_set_.emplace(edge.trans_symbol);
}
