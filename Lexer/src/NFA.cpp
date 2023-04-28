/**
  ******************************************************************************
  * @file           : NFA.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/27
  ******************************************************************************
  */

#include "../include/NFA.h"

NFA::NFA()
= default;

NFA::NFA(const std::vector<RegexSymbol>& regex)
    :
    state_num_{0}
{
    Cell cell = ExpressToNFA(regex);
    edge_set_ = cell.edge_set;
    edge_count_ = cell.edge_set.size();
    start_state_ = cell.start_state;
    end_state_ = cell.end_state;

    for (const auto& edge : edge_set_)
        symbol_set_.emplace(edge.trans_symbol);
}

/*!
 * ���ʽתNFA���������������յ�NFA
 * @return
 */
Cell NFA::ExpressToNFA(const std::vector<RegexSymbol>& regex)
{
    size_t length = regex.size();
    RegexSymbol element;
    Cell cell, left, right;

    std::stack<Cell> s;
    for (size_t i = 0; i < length; ++i)
    {
        element = regex.at(i);
        if (element.second) // �ǲ�����
        {
            switch (element.first)
            {
                case '|':
                    right = s.top();
                    s.pop();
                    left = s.top();
                    s.pop();
                    cell = DoUnit(left, right);
                    s.push(cell);
                    break;
                case '*':
                    left = s.top();
                    s.pop();
                    cell = DoStar(left);
                    s.push(cell);
                    break;
                case '+':
                    right = s.top();
                    s.pop();
                    left = s.top();
                    s.pop();
                    cell = DoJoin(left, right);
                    s.push(cell);
                    break;
            }
        }
        else
        {
            cell = DoCell(element.first);
            s.push(cell);
        }

    }

    std::cout << "������ϣ�" << std::endl;
    cell = s.top();
    s.pop();

    return cell;
}

Cell NFA::DoCell(char ch)
{
    Cell new_cell;
    Edge new_edge{};

    // ��ȡ��״̬�ڵ�
    State start_state = NewStateNode();
    State end_state = NewStateNode();

    // ������
    new_edge.start_state = start_state;
    new_edge.end_state = end_state;
    new_edge.trans_symbol = ch;

    // ������Ԫ
    new_cell.edge_set.emplace(new_edge);
    new_cell.start_state = start_state;
    new_cell.end_state = end_state;

    return new_cell;
}

Cell NFA::DoUnit(const Cell& left, const Cell& right)
{
    Cell new_cell;
    Edge edge1{}, edge2{}, edge3{}, edge4{};

    // ��ȡ��״̬�ڵ�
    State start_state = NewStateNode(); // s
    State end_state = NewStateNode();   // e

    // ������
    edge1.start_state = start_state;
    edge1.end_state = left.start_state;
    edge1.trans_symbol = '#';

    edge2.start_state = start_state;
    edge2.end_state = right.start_state;
    edge2.trans_symbol = '#';

    edge3.start_state = left.end_state;
    edge3.end_state = end_state;
    edge3.trans_symbol = '#';

    edge4.start_state = right.end_state;
    edge4.end_state = end_state;
    edge4.trans_symbol = '#';

    // ������Ԫ
    // �Ƚ�left��right��edge_set���Ƶ�new_cell
    CellEdgeSetCopy(new_cell, left);
    CellEdgeSetCopy(new_cell, right);

    // ���¹����������߼���edge_set
    new_cell.edge_set.emplace(edge1);
    new_cell.edge_set.emplace(edge2);
    new_cell.edge_set.emplace(edge3);
    new_cell.edge_set.emplace(edge4);

    // ����new_cell����ʼ״̬�ͽ���״̬
    new_cell.start_state = start_state;
    new_cell.end_state = end_state;

    return new_cell;
}

Cell NFA::DoJoin(Cell &left, Cell &right)
{
    // ��left�Ľ���״̬��right�Ŀ�ʼ״̬�ϲ�����right�ı߸��Ƹ�left����left����
    // ��right��������С��nfa��start_state��ͷ�ı�ȫ���޸�
    for (auto it = right.edge_set.begin(); it != right.edge_set.end();)
    {
        Edge edge = *it;
        if (edge.start_state == right.start_state)
        {
            Edge new_edge;
            new_edge.start_state = left.end_state;
            new_edge.end_state = edge.end_state;
            new_edge.trans_symbol = edge.trans_symbol;

            right.edge_set.erase(it++);
            right.edge_set.emplace(new_edge);
        }
        else if (edge.end_state == right.start_state)
        {
            Edge new_edge;
            new_edge.start_state = edge.start_state;
            new_edge.end_state = left.end_state;
            new_edge.trans_symbol = edge.trans_symbol;

            right.edge_set.erase(it++);
            right.edge_set.emplace(new_edge);
        }
        else
        {
            ++it;
        }
    }

    CellEdgeSetCopy(left, right);

    left.end_state = right.end_state;
    return left;
}

Cell NFA::DoStar(const Cell& cell)
{
    Cell new_cell;
    Edge edge1{}, edge2{}, edge3{}, edge4{};

    // ��ȡ�µ�״̬�ڵ�
    State start_state = NewStateNode();
    State end_state = NewStateNode();

    // ������
    edge1.start_state = start_state;
    edge1.end_state = end_state;
    edge1.trans_symbol = '#';

    edge2.start_state = cell.end_state;
    edge2.end_state = cell.start_state;
    edge2.trans_symbol = '#';

    edge3.start_state = start_state;
    edge3.end_state = cell.start_state;
    edge3.trans_symbol = '#';

    edge4.start_state = cell.end_state;
    edge4.end_state = end_state;
    edge4.trans_symbol = '#';

    // ������Ԫ
    // �Ƚ�cell��edge_set���Ƶ�new_cell��
    CellEdgeSetCopy(new_cell, cell);
    // ���¹����������߼���edge_set��
    new_cell.edge_set.emplace(edge1);
    new_cell.edge_set.emplace(edge2);
    new_cell.edge_set.emplace(edge3);
    new_cell.edge_set.emplace(edge4);

    // ����new_cell����ʼ״̬����ֹ״̬
    new_cell.start_state = start_state;
    new_cell.end_state = end_state;

    return new_cell;
}

/*!
 * ����µ�״̬�ڵ㣬ͳһ���������ڹ������ܲ����ظ���״̬
 * ����ӵ�state_set������
 * @return
 */
State NFA::NewStateNode()
{
    State new_state{++state_num_};

    return new_state;
}

void NFA::CellEdgeSetCopy(Cell& des, const Cell& src)
{
    des.edge_set.insert(src.edge_set.begin(), src.edge_set.end());
}

void NFA::Display() const
{
    std::cout << "NFA �ı�����" << edge_count_ << std::endl;
    std::cout << "NFA ����ʼ״̬��" << start_state_ << std::endl;
    std::cout << "NFA ����ֹ״̬��Ϊ��" << end_state_ << std::endl;

    size_t i = 0;
    for (const auto& edge : edge_set_)
    {
        std::cout << "��" << i + 1 << "���ߵ���ʼ״̬��" << edge.start_state
                  << "  ����״̬��" << edge.end_state
                  << "  ת������" << edge.trans_symbol << std::endl;
        ++i;
    }
    std::cout << "����" << std::endl;
}