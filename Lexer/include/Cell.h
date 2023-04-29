//
// Created by icaner on 2023/4/27.
//

#ifndef COMPILER_CELL_H
#define COMPILER_CELL_H

#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <stack>
#include <queue>
#include <map>


// NFA��DFA�Ľڵ㣬����ɽṹ�壬�����Ժ���չ
struct State
{
    State()
    = default;

    explicit State(size_t i) : value(i)
    {

    }

    friend std::ostream& operator<<(std::ostream& out, const State& state)
    {
        out << state.value;
        return out;
    }

    friend std::ofstream& operator<<(std::ofstream& out, const State& state)
    {
        out << state.value;
        return out;
    }

    size_t value{};

    bool operator==(const State& other) const
    {
        return value == other.value;
    }

    bool operator!=(int i) const
    {
        return value != i;
    }

    bool operator<(const State& other) const
    {
        return value < other.value;
    }

    State& operator=(const State& other)
    = default;

    State& operator=(size_t i)
    {
        value = i;
        return *this;
    }
};


// NFA�ıߣ���ת������'#'��ʾ
struct Edge
{
    State start_state;
    State end_state;
    char trans_symbol;  // ת������

    bool operator<(const Edge& other) const
    {
        if (start_state == other.start_state)
        {
            if (end_state == other.end_state)
                return trans_symbol < other.trans_symbol;
            else
                return end_state < other.end_state;
        }
        return start_state < other.start_state;
    }
};

// NFA��DFA��Ԫ��һ�����NFA��DFA��Ԫ�������ɺܶ�С��Ԫͨ������ƴ������
struct Cell
{
    std::set<Edge> edge_set; // ���NFA��Ԫӵ�еı�
    State start_state;  // ��ʼ״̬
    State end_state;    // ����״̬����NFAʹ�ã�������������ʽתNFA��ת����õ���NFAֻ��һ����̬һ����̬��
    std::set<State> end_state_set;  // ����״̬������DFAʹ�ã���������DFAֻ��һ����̬���������ж����̬��
};


#endif //COMPILER_CELL_H
