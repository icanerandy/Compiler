/**
  ******************************************************************************
  * @file           : NFA.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/27
  ******************************************************************************
  */

#include "NFA.h"

NFA::NFA()
= default;

NFA::NFA(const Cell& cell)
    :
    edge_set_(cell.edge_set),
    edge_count_(cell.edge_set.size()),
    start_state_(cell.start_state),
    end_state_(cell.end_state)
{
    for (const auto& edge : cell.edge_set)
        symbol_set_.emplace(edge.trans_symbol);
}

