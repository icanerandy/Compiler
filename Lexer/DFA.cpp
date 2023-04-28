/**
  ******************************************************************************
  * @file           : DFA.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/27
  ******************************************************************************
  */

#include "DFA.h"

#include <utility>

DFA::DFA()
= default;

DFA::DFA(const NFA& nfa)
    :
    nfa_(nfa),
    start_state_(1)
{
    nfa.Display();

    NFA2DFA();

    Display();

    DFA2MFA();

    Display();
}

/*!
 *
 */
void DFA::NFA2DFA()
{
    Cell cell = WorkList();
    edge_set_ = cell.edge_set;
    edge_count_ = cell.edge_set.size();
    start_state_ = cell.start_state;
    end_state_ = cell.end_state_set;

    for (const auto& edge : edge_set_)
    {
        // ��״̬֮���ӳ������״̬ת����
        dfa_table_[edge.start_state][edge.trans_symbol] = edge.end_state;

        state_set_.emplace(edge.start_state);
        state_set_.emplace(edge.end_state);
        symbol_set_.emplace(edge.trans_symbol);
    }
}

/*!
 * �Ӽ������㷨���������㷨
 * @return - DFA״̬��
 */
Cell DFA::WorkList()
{
    // ���㿪ʼ״̬�ı߽�
    std::set<State> q0 = Closure(nfa_.start_state_);
    // DFA�����е�״̬��
    state_num_ = 0;
    std::map<std::set<State>, size_t> Q;    // �߽�״̬����DFA״̬��ŵ�ӳ��
    Q.emplace(q0, ++state_num_);
    // DFA��Ԫ
    Cell cell;
    cell.start_state = 1;
    // ���q0�к���NFA����̬���򽫶�Ӧ�ı߽�״̬���ı�ż��뵽DFA����̬����
    if (q0.find(nfa_.end_state_) != q0.end())
        cell.end_state_set.emplace(Q.find(q0)->second);

    // �����������״̬���˴���״̬ΪNFA״̬�ļ��ϣ�
    std::queue<std::set<State>> worklist;
    worklist.emplace(q0);

    while (!worklist.empty())
    {
        std::set<State> q = worklist.front();
        worklist.pop();

        // �������ż�
        for (const auto& symbol : nfa_.symbol_set_)
        {
            if (symbol == '#') continue;

            std::set<State> t;
            // �����߽�״̬��
            for (const auto& state : q)
            {
                // �����ߣ����ҿ�ͨ���߽�״̬�����߽�״̬���е�״̬state�����շ���symbol�����״̬
                for (const auto& edge : nfa_.edge_set_)
                {
                    if (edge.start_state == state && edge.trans_symbol == symbol)
                    {
                        std::set<State> temp = Closure(edge.end_state);  // ͨ����ǰ���ſɵ����ĳ��״̬��
                        t.insert(temp.begin(), temp.end());
                    }
                }
            }

            // ������������t��Ϊ֮ǰ�����������һ״̬��
            if (!t.empty())
            {
                if (Q.find(t) == Q.end())
                {
                    Q.emplace(t, ++state_num_);
                    worklist.emplace(t);

                    // ���t�к���NFA����̬���򽫶�Ӧ�ı߽�״̬���ı�ż��뵽DFA����̬����
                    if (t.find(nfa_.end_state_) != t.end())
                        cell.end_state_set.emplace(Q.find(t)->second);
                }

                cell.edge_set.emplace(Connect(Q, q, t, symbol));
            }
        }
    }

    return cell;
}

/*!
 * ��ǰ״̬�Ħ�-�հ�
 * @param state
 * @return
 */
std::set<State> DFA::Closure(const State &state)
{
    std::set<State> closure;
    std::queue<State> Q;
    Q.emplace(state);

    while (!Q.empty())
    {
        State q = Q.front();
        Q.pop();
        closure.emplace(q);

        for (const auto& edge : nfa_.edge_set_)
        {
            if (edge.start_state == q && edge.trans_symbol == '#')
                Q.emplace(edge.end_state);
        }
    }

    return closure;
}

/*!
 * ���ӹ������е�ǰ״̬�����±߽�״̬��
 * @param Q - �߽�״̬����DFA״̬��ŵ�ӳ��
 * @param from - �������е�ǰ��״̬��
 * @param to - ����һ�����ź󵽴���±߽�״̬��
 * @param symbol - ���յķ���
 * @return - һ�����ӹ������е�ǰ״̬�����±߽�״̬���ıߣ��ߵ�ȨΪsymbol
 */
Edge DFA::Connect(const std::map<std::set<State>, size_t>& Q,
                 const std::set<State>& from, const std::set<State>& to,
                 char symbol)
{
    Edge edge{};
    edge.start_state = Q.find(from)->second;
    edge.end_state = Q.find(to)->second;
    edge.trans_symbol = symbol;

    return edge;
}

/*!
 *
 */
void DFA::DFA2MFA()
{
    Cell cell = Hopcroft();
    edge_set_ = cell.edge_set;
    edge_count_ = cell.edge_set.size();
    start_state_ = cell.start_state;
    end_state_ = cell.end_state_set;

    for (const auto& edge : edge_set_)
    {
        // ��״̬֮���ӳ������״̬ת����
        dfa_table_[edge.start_state][edge.trans_symbol] = edge.end_state;

        state_set_.emplace(edge.start_state);
        state_set_.emplace(edge.end_state);
        symbol_set_.emplace(edge.trans_symbol);
    }
}

Cell DFA::Hopcroft()
{
    std::map<size_t, std::set<State>> categories;    // DFA��С����״̬���->״̬��������ӳ��
    // �Ƚ�����״̬������Ϊ����̬����̬״̬�����󼯺�
    for (const auto& state : state_set_)
    {
        if (end_state_.find(state) == end_state_.end())
            categories[1].emplace(state);    // �ǽ���̬����
        else
            categories[2].emplace(state);    // ����̬����
    }
    state_num_ = 3; // ��������״̬��Ŵ�3��ʼ

    bool still_changing = true;
    while (still_changing)
        still_changing = Split(categories);

    return CreateMFABySets(categories);
}

bool DFA::Split(std::map<size_t, std::set<State>>& categories)
{
    // һ������һ�����ŵļ�飻������ż�����ˣ��ټ�鵱ǰ���ļ��Ͻ�����һ�������Ƿ������˵�ǰ��𼯺ϣ���Ҫ���֣�
    for (auto& it : categories)
    {
        size_t included_category_no = it.first;  // ��ǰ������𼯱��

        // ��ǰ��𼯺��н�һ��Ԫ�أ��򲻱��ٻ��򣬼����һ����𼯺�
        if (it.second.size() == 1) continue;

        // ��ǰ��𼯺��в�ֹһ��Ԫ�أ���Ҫ����
        // ��·���������շ��ţ�

        for (const auto& symbol : symbol_set_)
        {
            std::set<State> reached_set;

            // ������ǰ����е�״̬���ҳ�ͨ����ǰ���ŵ����״̬��
            for (const auto& state : it.second)
            {
                // ��ǰ״̬���ܽ��ո÷��ţ���ǰ״̬�����ڵ�ǰ���
                if (dfa_table_[state].find(symbol) == dfa_table_[state].end())
                {
                    reached_set.emplace(included_category_no);
                    continue;
                }

                reached_set.emplace(ToWhichSet(categories, dfa_table_[state][symbol]));
            }

            if (reached_set.size() > 1)
            {
                // ������ǰ����е�״̬���ҳ�ͨ����ǰ���ŵ����״̬��
                for (const auto& state : it.second)
                {
                    reached_set.emplace(ToWhichSet(categories, state));

                    // ��ǰ״̬���ܽ��ո÷��ţ���ǰ״̬�����ڵ�ǰ���
                    if (dfa_table_[state].find(symbol) == dfa_table_[state].end())
                        continue;

                    // ͨ�����շ��ţ���ǰ״̬������һ��������������𼯺�
                    size_t reached_category_no = ToWhichSet(categories, dfa_table_[state][symbol]);
                    if (reached_category_no != included_category_no)
                    {
                        std::set<State> new_set;
                        new_set.emplace(state);
                        categories[included_category_no].erase(state);
                        // �������ҵ�ǰ��𼯺��У��Ƿ��б��״̬Ҳ��ͨ�����մ˷��ŵ���
                        // ������ͬ����𼯺�
                        for (auto tmp = it.second.begin(); tmp != it.second.end();)
                        {
                            // ��ǰ״̬���ܽ��ո÷��ţ���ǰ״̬�����ڵ�ǰ���
                            if (dfa_table_[*tmp].find(symbol) == dfa_table_[*tmp].end())
                            {
                                ++tmp;
                                continue;
                            }

                            if (ToWhichSet(categories, dfa_table_[*tmp][symbol]) == reached_category_no)
                            {
                                new_set.emplace(*tmp);
                                categories[included_category_no].erase(tmp++);
                            }
                            else
                            {
                                ++tmp;
                            }
                        }
                        categories[state_num_++] = (new_set);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

/*!
 * �ж�һ�������ļ����Ƿ�����������𼯺ϵ��Ӽ�
 * @param categories - DFA��С����״̬���->״̬��������ӳ��
 * @param cur_set - �����ļ���
 * @return - ������ڷ���true������false
 */
bool DFA::IsSubSet(std::map<size_t, std::set<State>>& categories, const std::set<State>& cur_set)
{
    // ����ÿһ�����ļ���
    for (const auto& it : categories)
    {
        // ���it.second����cur_set������֮ǰĳ�����ϵ��Ӽ�
        auto it1 = cur_set.begin();
        for (; it1 != cur_set.end(); ++it1)
        {
            if (it.second.find(*it1) == it.second.end()) break;
        }
        if (it1 == cur_set.end()) return true;
    }

    return false;
}

/*!
 * �жϸ�����״̬������һ����𼯱��
 * @param categories - DFA��С����״̬���->״̬��������ӳ��
 * @param state - ������״̬
 * @return - ��״̬������һ����𼯱��(DFA��С�����״̬���)
 */
size_t DFA::ToWhichSet(std::map<size_t, std::set<State>>& categories, State state)
{
    for (const auto& it : categories)
        if (it.second.find(state) != it.second.end())
            return it.first;
    return -1;  // ��ʾ����
}

/*!
 * ͨ��hopcroft�㷨�õ��ĵȼ��������µ�״̬ת��ͼ
 * @param categories - �ȼ���
 * @return
 */
Cell DFA::CreateMFABySets(std::map<size_t, std::set<State>> &categories)
{
    Cell cell;
    std::set<Edge> new_edge_set;

    for (auto& edge : edge_set_)
    {
        State start_state(ToWhichSet(categories, edge.start_state));
        State end_state(ToWhichSet(categories, edge.end_state));

        Edge new_edge;
        new_edge.start_state = start_state;
        new_edge.end_state = end_state;
        new_edge.trans_symbol = edge.trans_symbol;

        new_edge_set.emplace(new_edge);
    }

    cell.edge_set = new_edge_set;
    cell.start_state = ToWhichSet(categories, start_state_);
    std::set<State> new_end;
    for (auto& state : end_state_)
        new_end.emplace(ToWhichSet(categories, state));
    cell.end_state_set = new_end;

    return cell;
}

void DFA::Display()
{
    std::cout << "DFA �ı�����" << edge_count_ << std::endl;
    std::cout << "DFA ����ʼ״̬��" << start_state_ << std::endl;
    std::cout << "DFA ����ֹ״̬��";
    for (const auto& state : end_state_)
        std::cout << state << " ";
    std::cout << std::endl;

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

bool DFA::Judge(const std::string& str)
{
    State state = start_state_;
    for (const auto& ch : str)
    {
        state = dfa_table_[state][ch];
    }
    if (end_state_.find(state) == end_state_.end())
        return false;
    return true;
}
