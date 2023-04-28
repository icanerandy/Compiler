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

/***************NFA�ľ���ṹ****************/

class NFA
{
public:
    explicit NFA();
    explicit NFA(const std::string& regex);
    void Display() const;

private:
    Cell ExpressToNFA(const std::string& regex);    // ���ʽתNFA
    Cell DoCell(char ch);  // ���� a
    Cell DoUnit(const Cell& left, const Cell& right); // ���� a|b
    Cell DoJoin(Cell& left, Cell& right); // ���� ab
    Cell DoStar(const Cell& cell); // ���� a*

private:
    State NewStateNode();   // ����һ���µ�״̬�ڵ㣬���ڹ���
    static void CellEdgeSetCopy(Cell& des, const Cell& src);

public:
    std::set<Edge> edge_set_;
    size_t edge_count_{};  // ����
    State start_state_;    //  ��ʼ״̬
    State end_state_;    // ����״̬
    std::set<char> symbol_set_;  // �ɽ��շ��ż�

private:
    size_t state_num_{};  // ��֤ȫ��Ψһ��
};

/********************************************/


#endif //COMPILER_NFA_H
