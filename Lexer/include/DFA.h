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

/***************DFA�ľ���ṹ****************/

class DFA
{
public:
    explicit DFA();
    explicit DFA(const NFA& nfa);
public:
    void Display();
    bool Judge(const std::string& str);
    State GetStartState() const;    // ��ȡ��ʼ״̬
    std::set<State> GetEndStateSet() const;    // ��ȡ����״̬��
    std::map<State, std::map<char, State>>& GetDFATable();   // ��ȡ״̬ת����

private:    // NFA-DFA
    void NFA2DFA();
    Cell WorkList();    // �Ӽ������㷨���������㷨
    std::set<State> Closure(const State& state);  // ����űհ�
    static Edge Connect(const std::map<std::set<State>, size_t>& Q,
                        const std::set<State>& from, const std::set<State>& to,
                        char symbol);

private:    // DFA��С��
    void DFA2MFA();
    Cell Hopcroft();
    bool Split(std::map<size_t, std::set<State>>& categories);
    static bool IsSubSet(std::map<size_t, std::set<State>>& categories, const std::set<State>& cur_set);
    static size_t ToWhichSet(std::map<size_t, std::set<State>>& categories, State state);
    Cell CreateMFABySets(std::map<size_t, std::set<State>>& categories);

public:
    std::set<Edge> edge_set_;
    size_t edge_count_{};  // ����
    std::set<State> state_set_;
    State start_state_;    // ��ʼ״̬
    std::set<State> end_state_;    // ����״̬��
    std::set<char> symbol_set_;  // �ɽ��շ��ż�

public:
    std::map<State, std::map<char, State>> dfa_table_;  // ״̬ת��ͼ��Ӧ��״̬ת����

private:
    NFA nfa_;   // ԭnfa
    size_t state_num_{};  // ��֤ȫ��Ψһ��
};

/********************************************/


#endif //COMPILER_DFA_H
