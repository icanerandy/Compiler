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

/***************NFA�ľ���ṹ****************/

class NFA
{
public:
    explicit NFA();
    explicit NFA(const Cell& cell);

public:
    std::vector<Edge> edge_set_;
    size_t edge_count_{};  // ����
    State start_state_;    //  ��ʼ״̬
    State end_state_;    // ����״̬
    std::set<char> symbol_set_;  // �ɽ��շ��ż�
};

/********************************************/


#endif //GRAMMAR_NFA_H
