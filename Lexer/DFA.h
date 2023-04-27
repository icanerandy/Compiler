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

/***************DFA�ľ���ṹ****************/

class DFA
{
public:
    explicit DFA();
    explicit DFA(const Cell& cell);

public:
    std::vector<Edge> edge_set_;
    size_t edge_count_{};  // ����
    State start_state_;    // ��ʼ״̬
    std::set<State> end_state_;    // ����״̬��
    std::set<char> symbol_set_;  // �ɽ��շ��ż�
};

/********************************************/


#endif //GRAMMAR_DFA_H
