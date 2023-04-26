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

// NFA�Ľڵ㣬����ɽṹ�壬�����Ժ���չ
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


// NFA�ıߣ���ת������'#'��ʾ
struct Edge
{
    State start_state;
    State end_state;
    char trans_symbol;  // ת������

    bool operator<(const Edge& other)
    {
        return start_state < other.start_state;
    }
};

// NFA��Ԫ��һ�����NFA��Ԫ�������ɺܶ�С��Ԫͨ������ƴ������
struct Cell
{
    std::vector<Edge> edge_vector; // ���NFAӵ�еı�
    size_t edge_count;  // ����
    State start_state;  // ��ʼ״̬
    State end_state;    // ����״̬
};

/***************NFA�ľ���ṹ****************/

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
    Cell ExpressToNFA();    // ���ʽתNFA
    Cell DoCell(char ch);  // ���� a
    Cell DoUnit(const Cell& left, const Cell& right); // ���� a|b
    Cell DoJoin(Cell& left, Cell& right); // ���� ab
    Cell DoStar(const Cell& cell); // ���� a*

private:
    State NewStateNode();   // ����һ���µ�״̬�ڵ㣬���ڹ���
    static void CellEdgeSetCopy(Cell& des, const Cell& src);

private:
    std::string regex_;
    size_t state_num_;  // ��֤ȫ��Ψһ��
    Cell cell_;
};


#endif //GRAMMAR_FA_H
