//
// Created by icaner on 2023/4/27.
//

#ifndef GRAMMAR_CELL_H
#define GRAMMAR_CELL_H

#include <iostream>
#include <set>
#include <vector>
#include <stack>
#include <queue>
#include <map>


// NFA或DFA的节点，定义成结构体，便于以后扩展
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

    size_t value{};

    bool operator==(const State& other) const
    {
        return value == other.value;
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


// NFA的边，空转换符用'#'表示
struct Edge
{
    State start_state;
    State end_state;
    char trans_symbol;  // 转换符号

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

// NFA或DFA单元，一个大的NFA或DFA单元可以是由很多小单元通过规则拼接起来
struct Cell
{
    std::vector<Edge> edge_set; // 这个NFA单元拥有的边
    State start_state;  // 开始状态
    State end_state;    // 结束状态，供NFA使用（本程序中正规式转NFA，转换后得到的NFA只有一个初态一个终态）
    std::set<State> end_state_set;  // 结束状态集，供DFA使用（本程序中DFA只有一个初态，但可以有多个终态）
};


#endif //GRAMMAR_CELL_H
