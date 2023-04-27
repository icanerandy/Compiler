/**
  ******************************************************************************
  * @file           : FA.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/26
  ******************************************************************************
  */

#include "FA.h"

#include <utility>
#include <map>

FA::FA(std::string regex)
    :
    regex_(std::move(regex)),
    state_num_{0}
{
    CheckLegal();
    AddJoinSymbol();

    PostFix();

    nfa_ = ExpressToNFA();
    DisplayNFA();

    dfa_ = WorkList();
    DisplayDFA();
}

/*!
 * 检测输入的正则表达式是否合法
 * @return
 */
bool FA::CheckLegal()
{
    return CheckCharacter() && CheckParenthesis();
}

/*!
 * 检查输入的字符是否合适 () * | a~z A~Z
 * @return
 */
bool FA::CheckCharacter()
{
    for (char check : regex_)
    {
        if (std::isalpha(check))//小写和大写之间有5个字符，故不能连续判断
        {
            //cout<<"字母 合法";
        }
        else if (check == '(' || check == ')' || check == '*' || check == '|')
        {
            //cout<<"操作符 合法";
        }
        else
        {
            std::cout << "含有不合法的字符!" << std::endl;
            std::cout << "请重新输入:" << std::endl;
            return false;
        }
    }
    return true;
}

/*!
 * 先检查括号是否匹配
 * @return
 */
bool FA::CheckParenthesis()
{
    //char * check = new char[length+1];
    //wcscpy(check, length+1, check_string.c_str());
    std::string check = regex_;
    std::stack<int> STACK;
    for (size_t i = 0; i < regex_.size(); i++)
    {
        if (check[i] == '(')
            STACK.push(i);
        else if (check[i] == ')')
        {
            if (STACK.empty())
            {
                std::cerr << "有多余的右括号" << std::endl;//暂时不记录匹配位置location
                std::cout << "请重新输入:" << std::endl;
                return false;
            }
            else
                STACK.pop();
        }
    }
    if (!STACK.empty())
    {
        //暂时不记录匹配位置location
        std::cerr << "有多余的左括号" << std::endl;
        std::cout << "请重新输入:" << std::endl;
        return false;
    }

    return true;
}

/*!
 * 添加交操作符“+”，便于中缀转后缀表达式
 */
void FA::AddJoinSymbol()
{
/*  测试终止符\0
	string check_string = "abcdefg\0aaa";
	cout<<check_string<<endl;
	int length = check_string.size();
	char * check = new char[2*length];
	strcpy(check,check_string.c_str());
	cout<<check<<endl;
	char *s = "ssss\0  aa";
	cout<<s<<endl;
	string a(s);
	cout<<a<<endl;
	*/
    size_t length = regex_.size();
    size_t new_regex_length = 0;

    char* new_regex = new char[2 * length + 2]; // 最多是两倍
    char first, second;

    for (size_t i = 0; i < length - 1; i++)
    {
        first = regex_.at(i);
        second = regex_.at(i + 1);
        new_regex[new_regex_length++] = first;
        // 要加的可能性如ab 、 *b 、 a( 、 )b 等情况
        // 第一个不是'('、'|'，第二个是'('或字母
        if (first != '(' && first != '|')
            if (std::isalpha(second) || second == '(')
                new_regex[new_regex_length++] = '+';
    }

    // 将最后一个字符写入
    new_regex[new_regex_length++] = second;
    new_regex[new_regex_length] = '\0';
    std::string STRING(new_regex);
    regex_ = STRING;
    std::cout << "加'+'后的表达式：" << regex_ << std::endl;
}

/*!
 * 中缀表达式转后缀表达式
 */
void FA::PostFix()
{
    // 设定regex_的最后一个符号式“#”，而其“#”一开始先放在栈s的栈底
    regex_ += '#';

    std::stack<char> s;
    char ch = '#', ch1, op;
    s.push(ch);

    // 读入一个字符
    std::string new_regex;
    size_t read_location = 0;
    ch = regex_.at(read_location++);
    while (!s.empty())
    {
        if (std::isalpha(ch))
        {
            new_regex += ch;
            ch = regex_.at(read_location++);
        }
        else
        {
            //cout<<"输出操作符："<<ch<<endl;
            ch1 = s.top();
            if (Isp(ch1) < Icp(ch))
            {
                s.push(ch);
                //cout<<"压栈"<<ch<<"  读取下一个"<<endl;
                ch = regex_.at(read_location++);
            }
            else if (Isp(ch1)>Icp(ch))
            {
                op = s.top();
                s.pop();
                //cout<<"退栈"<<op<<" 添加到输出字符串"<<endl;
                new_regex += op;
                //cout<<op;
            }
            else  //考虑优先级相等的情况
            {
                op = s.top();
                s.pop();
                //cout<<"退栈"<<op<<"  但不添加到输入字符串"<<endl;

                if (op == '(')
                    ch = regex_.at(read_location++);
            }
        }
    }

    regex_ = new_regex;
    std::cout << "后缀表达式：" << regex_ << std::endl;
}

/*
构造优先级表规则：（1）先括号内，再括号外；（2）优先级由高到低：闭包、|、+；（3）同级别，先左后右。
优先级表：
	 #	(	*	|	+	)
isp  0	1	7	5	3	8
icp	 0	8	6	4	2	1
*/

/*********************运算符优先级关系表********************/
/*
	c1\c2	(	*	|	+	)	#
	(		<	<	<	<	=	>
	*		<	>	>	>	>	>
	|		<	<	>	>	>	>
	+		<	<	<	>	>	>
	#		<	<	<	<	<	=
*/
/***********************************************************/

/*!
 * in stack priority  栈内优先级，栈顶的字符的优先级
 * @param ch
 * @return
 */
int FA::Isp(char ch)
{
    switch (ch)
    {
        case '#': return 0;
        case '(': return 1;
        case '*': return 7;
        case '|': return 5;
        case '+': return 3;
        case ')': return 8;
        default:
            //若走到这一步，说明出错了
            std::cerr << "ERROR!" << std::endl;
            return false;
    }
}

/*!
 * in coming priority 栈外优先级，当前扫描到的字符的优先级
 * @param ch
 * @return
 */
int FA::Icp(char ch)
{
    switch (ch)
    {
        case '#': return 0;
        case '(': return 8;
        case '*': return 6;
        case '|': return 4;
        case '+': return 2;
        case ')': return 1;
        default:
            //若走到这一步，说明出错了
            std::cerr << "ERROR!" << std::endl;
            return false;
    }
}

/*!
 * 获得新的状态节点，统一产生，便于管理，不能产生重复的状态
 * 并添加到state_set集合中
 * @return
 */
State FA::NewStateNode()
{
    State new_state{++state_num_};

    return new_state;
}

Cell FA::DoCell(char ch)
{
    Cell new_cell;
    Edge new_edge{};

    // 获取新状态节点
    State start_state = NewStateNode();
    State end_state = NewStateNode();

    // 构建边
    new_edge.start_state = start_state;
    new_edge.end_state = end_state;
    new_edge.trans_symbol = ch;

    // 构建单元
    new_cell.edge_set.emplace_back(new_edge);
    new_cell.start_state = start_state;
    new_cell.end_state = end_state;

    return new_cell;
}

void FA::CellEdgeSetCopy(Cell& des, const Cell& src)
{
    des.edge_set.insert(des.edge_set.begin(),
                            src.edge_set.begin(), src.edge_set.end());
}

Cell FA::DoUnit(const Cell& left, const Cell& right)
{
    Cell new_cell;
    Edge edge1{}, edge2{}, edge3{}, edge4{};

    // 获取新状态节点
    State start_state = NewStateNode(); // s
    State end_state = NewStateNode();   // e

    // 构建边
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

    // 构建单元
    // 先将left和right的edge_set复制到new_cell
    CellEdgeSetCopy(new_cell, left);
    CellEdgeSetCopy(new_cell, right);

    // 将新构建的四条边加入edge_set
    new_cell.edge_set.emplace_back(edge1);
    new_cell.edge_set.emplace_back(edge2);
    new_cell.edge_set.emplace_back(edge3);
    new_cell.edge_set.emplace_back(edge4);

    // 构建new_cell的起始状态和结束状态
    new_cell.start_state = start_state;
    new_cell.end_state = end_state;

    return new_cell;
}

Cell FA::DoJoin(Cell &left, Cell &right)
{
    // 将left的结束状态和right的开始状态合并，将right的边复制给left，将left返回
    // 将right中所有以小型nfa的start_state开头的边全部修改
    for (auto& edge : right.edge_set)
    {
        if (edge.start_state == right.start_state)
            edge.start_state = left.end_state;
        else if (edge.end_state == right.start_state)
            edge.end_state = left.end_state;
    }
    state_num_--;

    CellEdgeSetCopy(left, right);

    left.end_state = right.end_state;
    return left;
}

Cell FA::DoStar(const Cell& cell)
{
    Cell new_cell;
    Edge edge1{}, edge2{}, edge3{}, edge4{};

    // 获取新的状态节点
    State start_state = NewStateNode();
    State end_state = NewStateNode();

    // 构建边
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

    // 构建单元
    // 先将cell的edge_set复制到new_cell中
    CellEdgeSetCopy(new_cell, cell);
    // 将新构建的四条边加入edge_set中
    new_cell.edge_set.emplace_back(edge1);
    new_cell.edge_set.emplace_back(edge2);
    new_cell.edge_set.emplace_back(edge3);
    new_cell.edge_set.emplace_back(edge4);

    // 构建new_cell的起始状态和终止状态
    new_cell.start_state = start_state;
    new_cell.end_state = end_state;

    return new_cell;
}

/*!
 * 表达式转NFA处理函数，返回最终的NFA
 * @return
 */
NFA FA::ExpressToNFA()
{
    size_t length = regex_.size();
    char element;
    Cell cell, left, right;

    std::stack<Cell> s;
    for (size_t i = 0; i < length; ++i)
    {
        element = regex_.at(i);
        switch (element)
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
            default:
                cell = DoCell(element);
                s.push(cell);
        }
    }

    std::cout << "处理完毕！" << std::endl;
    cell = s.top();
    s.pop();

    return NFA(cell);
}

/*!
 * 子集构造算法：工作表算法
 * @return - DFA状态表
 */
DFA FA::WorkList()
{
    // 计算开始状态的边界
    std::set<State> q0 = Closure(nfa_.start_state_);
    // DFA中所有的状态机
    state_num_ = 0;
    std::map<std::set<State>, size_t> Q;    // 边界状态集到DFA状态编号的映射
    Q.emplace(q0, ++state_num_);
    // DFA单元
    Cell cell;

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

                cell.edge_set.emplace_back(Connect(Q, q, t, symbol));
            }
        }
    }

    return DFA(cell);
}

/*!
 * 求当前状态的ε-闭包
 * @param state
 * @return
 */
std::set<State> FA::Closure(const State &state)
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
Edge FA::Connect(const std::map<std::set<State>, size_t>& Q,
                 const std::set<State>& from, const std::set<State>& to,
                 char symbol)
{
    Edge edge{};
    edge.start_state = Q.find(from)->second;
    edge.end_state = Q.find(to)->second;
    edge.trans_symbol = symbol;

    return edge;
}

void FA::DisplayNFA()
{
    std::cout << "NFA 的边数：" << nfa_.edge_count_ << std::endl;
    std::cout << "NFA 的起始状态：" << nfa_.start_state_ << std::endl;
    std::cout << "NFA 的终止状态集为：" << nfa_.end_state_ << std::endl;

    size_t i = 0;
    for (const auto& edge : nfa_.edge_set_)
    {
        std::cout << "第" << i + 1 << "条边的起始状态：" << edge.start_state
                    << "  结束状态：" << edge.end_state
                    << "  转换符：" << edge.trans_symbol << std::endl;
        ++i;
    }
    std::cout << "结束" << std::endl;
}

void FA::DisplayDFA()
{
    std::cout << "DFA 的边数：" << dfa_.edge_count_ << std::endl;
    std::cout << "DFA 的起始状态：" << dfa_.start_state_ << std::endl;
    std::cout << "DFA 的终止状态：";
    for (const auto& state : dfa_.end_state_)
        std::cout << state << " ";
    std::cout << std::endl;

    size_t i = 0;
    for (const auto& edge : dfa_.edge_set_)
    {
        std::cout << "第" << i + 1 << "条边的起始状态：" << edge.start_state
                  << "  结束状态：" << edge.end_state
                  << "  转换符：" << edge.trans_symbol << std::endl;
        ++i;
    }
    std::cout << "结束" << std::endl;
}