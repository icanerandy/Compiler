/**
  ******************************************************************************
  * @file           : FA.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/26
  ******************************************************************************
  */

#ifndef GRAMMAR_FA_H
#define GRAMMAR_FA_H

#include <iostream>
#include <set>
#include <vector>
#include <stack>

// NFA的节点，定义成结构体，便于以后扩展
struct State
{
    friend std::ostream& operator<<(std::ostream& out, const State& state)
    {
        out << state.value;
        return out;
    }

    size_t value;

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
};


// NFA的边，空转换符用'#'表示
struct Edge
{
    State start_state;
    State end_state;
    char trans_symbol;  // 转换符号

    bool operator<(const Edge& other)
    {
        return start_state < other.start_state;
    }
};

// NFA单元，一个大的NFA单元可以是由很多小单元通过规则拼接起来
struct Cell
{
    std::vector<Edge> edge_vector; // 这个NFA拥有的边
    size_t edge_count;  // 边数
    State start_state;  // 开始状态
    State end_state;    // 结束状态
};

/***************NFA的矩阵结构****************/

struct Node
{
    Edge* in_edges;
    Edge* out_edges;
    //State
};

/********************************************/

class FA
{
public:
    explicit FA(std::string regex);
    void Display();

private:
    bool CheckLegal();
    bool CheckCharacter();
    bool CheckParenthesis();
    void AddJoinSymbol();
    void PostFix();
    static int Isp(char ch);
    static int Icp(char ch);

private:
    Cell ExpressToNFA();    // 表达式转NFA
    Cell DoCell(char ch);  // 处理 a
    Cell DoUnit(const Cell& left, const Cell& right); // 处理 a|b
    Cell DoJoin(Cell& left, Cell& right); // 处理 ab
    Cell DoStar(const Cell& cell); // 处理 a*

private:
    State NewStateNode();   // 产生一个新的状态节点，便于管理
    static void CellEdgeSetCopy(Cell& des, const Cell& src);

private:
    std::string regex_;
    size_t state_num_;  // 保证全局唯一性
    Cell cell_;
};


#endif //GRAMMAR_FA_H
