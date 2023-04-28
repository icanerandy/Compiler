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
        // 将状态之间的映射填入状态转换表
        dfa_table_[edge.start_state][edge.trans_symbol] = edge.end_state;

        state_set_.emplace(edge.start_state);
        state_set_.emplace(edge.end_state);
        symbol_set_.emplace(edge.trans_symbol);
    }
}

/*!
 * 子集构造算法：工作表算法
 * @return - DFA状态表
 */
Cell DFA::WorkList()
{
    // 计算开始状态的边界
    std::set<State> q0 = Closure(nfa_.start_state_);
    // DFA中所有的状态机
    state_num_ = 0;
    std::map<std::set<State>, size_t> Q;    // 边界状态集到DFA状态编号的映射
    Q.emplace(q0, ++state_num_);
    // DFA单元
    Cell cell;
    cell.start_state = 1;
    // 如果q0中含有NFA集终态，则将对应的边界状态集的编号加入到DFA的终态集中
    if (q0.find(nfa_.end_state_) != q0.end())
        cell.end_state_set.emplace(Q.find(q0)->second);

    // 工作表，存放着状态（此处的状态为NFA状态的集合）
    std::queue<std::set<State>> worklist;
    worklist.emplace(q0);

    while (!worklist.empty())
    {
        std::set<State> q = worklist.front();
        worklist.pop();

        // 遍历符号集
        for (const auto& symbol : nfa_.symbol_set_)
        {
            if (symbol == '#') continue;

            std::set<State> t;
            // 遍历边界状态集
            for (const auto& state : q)
            {
                // 遍历边，查找可通过边界状态（即边界状态集中的状态state）接收符号symbol到达的状态
                for (const auto& edge : nfa_.edge_set_)
                {
                    if (edge.start_state == state && edge.trans_symbol == symbol)
                    {
                        std::set<State> temp = Closure(edge.end_state);  // 通过当前符号可到达的某个状态集
                        t.insert(temp.begin(), temp.end());
                    }
                }
            }

            // 如果计算出来的t不为之前计算出来的任一状态集
            if (!t.empty())
            {
                if (Q.find(t) == Q.end())
                {
                    Q.emplace(t, ++state_num_);
                    worklist.emplace(t);

                    // 如果t中含有NFA集终态，则将对应的边界状态集的编号加入到DFA的终态集中
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
 * 求当前状态的ε-闭包
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
 * 连接工作表中当前状态集和新边界状态集
 * @param Q - 边界状态集到DFA状态编号的映射
 * @param from - 工作表中当前的状态集
 * @param to - 接收一个符号后到达的新边界状态集
 * @param symbol - 接收的符号
 * @return - 一条连接工作表中当前状态集和新边界状态集的边，边的权为symbol
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
        // 将状态之间的映射填入状态转换表
        dfa_table_[edge.start_state][edge.trans_symbol] = edge.end_state;

        state_set_.emplace(edge.start_state);
        state_set_.emplace(edge.end_state);
        symbol_set_.emplace(edge.trans_symbol);
    }
}

Cell DFA::Hopcroft()
{
    std::map<size_t, std::set<State>> categories;    // DFA最小化后状态编号->状态集分类间的映射
    // 先将整个状态集划分为非终态和终态状态两个大集合
    for (const auto& state : state_set_)
    {
        if (end_state_.find(state) == end_state_.end())
            categories[1].emplace(state);    // 非接收态集合
        else
            categories[2].emplace(state);    // 接收态集合
    }
    state_num_ = 3; // 接下来的状态编号从3开始

    bool still_changing = true;
    while (still_changing)
        still_changing = Split(categories);

    return CreateMFABySets(categories);
}

bool DFA::Split(std::map<size_t, std::set<State>>& categories)
{
    // 一个符号一个符号的检查；这个符号检查完了，再检查当前类别的集合接收下一个符号是否跳出了当前类别集合（需要划分）
    for (auto& it : categories)
    {
        size_t included_category_no = it.first;  // 当前检查的类别集编号

        // 当前类别集合中仅一个元素，则不必再化简，检查下一个类别集合
        if (it.second.size() == 1) continue;

        // 当前类别集合中不止一个元素，需要化简
        // 找路径（即接收符号）

        for (const auto& symbol : symbol_set_)
        {
            std::set<State> reached_set;

            // 遍历当前类别中的状态，找出通过当前符号到达的状态集
            for (const auto& state : it.second)
            {
                // 当前状态不能接收该符号，则当前状态还属于当前类别集
                if (dfa_table_[state].find(symbol) == dfa_table_[state].end())
                {
                    reached_set.emplace(included_category_no);
                    continue;
                }

                reached_set.emplace(ToWhichSet(categories, dfa_table_[state][symbol]));
            }

            if (reached_set.size() > 1)
            {
                // 遍历当前类别中的状态，找出通过当前符号到达的状态集
                for (const auto& state : it.second)
                {
                    reached_set.emplace(ToWhichSet(categories, state));

                    // 当前状态不能接收该符号，则当前状态还属于当前类别集
                    if (dfa_table_[state].find(symbol) == dfa_table_[state].end())
                        continue;

                    // 通过接收符号，当前状态到达了一个不包含它的类别集合
                    size_t reached_category_no = ToWhichSet(categories, dfa_table_[state][symbol]);
                    if (reached_category_no != included_category_no)
                    {
                        std::set<State> new_set;
                        new_set.emplace(state);
                        categories[included_category_no].erase(state);
                        // 遍历查找当前类别集合中，是否还有别的状态也可通过接收此符号到达
                        // 和它相同的类别集合
                        for (auto tmp = it.second.begin(); tmp != it.second.end();)
                        {
                            // 当前状态不能接收该符号，则当前状态还属于当前类别集
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
 * 判断一个给定的集合是否属于已有类别集合的子集
 * @param categories - DFA最小化后状态编号->状态集分类间的映射
 * @param cur_set - 给定的集合
 * @return - 如果属于返回true，否则false
 */
bool DFA::IsSubSet(std::map<size_t, std::set<State>>& categories, const std::set<State>& cur_set)
{
    // 遍历每一个类别的集合
    for (const auto& it : categories)
    {
        // 如果it.second包含cur_set，则是之前某个集合的子集
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
 * 判断给出的状态属于哪一个类别集编号
 * @param categories - DFA最小化后状态编号->状态集分类间的映射
 * @param state - 给出的状态
 * @return - 该状态属于哪一个类别集编号(DFA最小化后的状态编号)
 */
size_t DFA::ToWhichSet(std::map<size_t, std::set<State>>& categories, State state)
{
    for (const auto& it : categories)
        if (it.second.find(state) != it.second.end())
            return it.first;
    return -1;  // 表示出错
}

/*!
 * 通过hopcroft算法得到的等价类生成新的状态转换图
 * @param categories - 等价类
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
    std::cout << "DFA 的边数：" << edge_count_ << std::endl;
    std::cout << "DFA 的起始状态：" << start_state_ << std::endl;
    std::cout << "DFA 的终止状态：";
    for (const auto& state : end_state_)
        std::cout << state << " ";
    std::cout << std::endl;

    size_t i = 0;
    for (const auto& edge : edge_set_)
    {
        std::cout << "第" << i + 1 << "条边的起始状态：" << edge.start_state
                  << "  结束状态：" << edge.end_state
                  << "  转换符：" << edge.trans_symbol << std::endl;
        ++i;
    }
    std::cout << "结束" << std::endl;
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
