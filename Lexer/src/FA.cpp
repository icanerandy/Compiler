/**
  ******************************************************************************
  * @file           : FA.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/26
  ******************************************************************************
  */

#include "../include/FA.h"

#include <utility>

FA::FA()
= default;

FA::FA(const std::string& FA_name)
{
    std::ifstream in(FA_name, std::ios::in);
    if (!in.is_open())
    {
        std::cerr << "打开文件失败，无法读取状态表文件到dfa_table！" << std::endl;
        exit(-1);
    }

    std::string line;

    // 读取第一行的初态
    std::getline(in, line);
    State start(std::stoull(line, nullptr, 10));
    dfa_.start_state_ = start;

    // 读取第二行的终态集
    std::getline(in, line);
    std::string word;
    std::stringstream end_set(line);
    while (std::getline(end_set, word, ' '))
    {
        if (!word.empty())
        {
            State state(std::stoull(word, nullptr, 10));
            dfa_.end_state_.emplace(state);
        }
    }

    while (std::getline(in, line))
    {
        std::stringstream ss(line);

        std::string from, symbol, to;

        ss >> from;
        ss >> symbol;
        ss >> to;

        State from_state(std::stoull(from, nullptr, 10));
        char accept_symbol = symbol.at(0);
        State to_state(std::stoull(to, nullptr, 10));

        dfa_.dfa_table_[from_state][accept_symbol] = to_state;
    }

    in.close();
}

FA::FA(std::string FA_name, std::string regex)
    :
    FA_name_(std::move(FA_name))
{
    for (size_t i = 0; i < regex.size(); ++i)
    {
        if (regex.at(i) == '\\')
        {
            char ch = regex.at(i+1);
            switch (ch) {
                case '\\': regex_.emplace_back('\\', false); break;
                case 's': regex_.emplace_back(' ', false); break;
                case 'n': regex_.emplace_back('\n', false); break;
                case 't': regex_.emplace_back('\t', false); break;
                case 'r': regex_.emplace_back('\r', false); break;
                case '|': regex_.emplace_back('|', false); break;
                case '*': regex_.emplace_back('*', false); break;
                case '+': regex_.emplace_back('+', false); break;
                case '(': regex_.emplace_back('(', false); break;
                case ')': regex_.emplace_back(')', false); break;
                    // 其他转义字符也可以在这里处理，如\'、\"、\r等
                default:
                    // 如果不是有效的转义字符，则将原字符保留
                    std::cerr << "正则表达式错误：非有效转移字符！" << " \\" << ch << std::endl;
                    exit(-1);
            }
            i++;
        }
        else if (regex.at(i) == '|' || regex.at(i) == '*' || regex.at(i) == '+' || regex.at(i) == '(' || regex.at(i) == ')')
        {
            regex_.emplace_back(regex.at(i), true);
        }
        else
        {
            regex_.emplace_back(regex.at(i), false);
        }
    }

    // 处理正则表达式
    // CheckLegal();
    AddJoinSymbol();
    PostFix();

    nfa_ = NFA(regex_);
    dfa_ = DFA(nfa_);

    OutputDFATable();   // 输出MFA表到文件中
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
//    for (char check : regex_)
//    {
//        if (std::isalpha(check))//小写和大写之间有5个字符，故不能连续判断
//        {
//            //cout<<"字母 合法";
//        }
//        else if (check == '(' || check == ')' || check == '*' || check == '|')
//        {
//            //cout<<"操作符 合法";
//        }
//        else
//        {
//            std::cout << "含有不合法的字符!" << std::endl;
//            std::cout << "请重新输入:" << std::endl;
//            return false;
//        }
//    }
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
    std::vector<RegexSymbol> check = regex_;
    std::stack<int> STACK;
    for (size_t i = 0; i < regex_.size(); i++)
    {
        if (check[i].first == '(' && check[i].second)
            STACK.push(i);
        else if (check[i].first == ')' && check[i].second)
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

    std::vector<RegexSymbol> new_regex; // 最多是两倍
    RegexSymbol first, second;

    for (size_t i = 0; i < length - 1; i++)
    {
        first = regex_.at(i);
        second = regex_.at(i + 1);
        new_regex.emplace_back(first);
        // 要加的可能性如ab 、 *b 、 a( 、 )b 等情况
        // 第一个不是'('、'|'，第二个是'('或字母
        if (!( (first.first == '(' || first.first == '|') && first.second ))
            if ( (second.first == '(' && second.second) || !second.second )
                new_regex.emplace_back('+', true);
    }

    // 将最后一个字符写入
    new_regex.emplace_back(regex_.at(length - 1));
    regex_ = new_regex;
    // std::cout << "加'+'后的表达式：" << regex_ << std::endl;
}

/*!
 * 中缀表达式转后缀表达式
 */
void FA::PostFix()
{
    std::stack<RegexSymbol> op;
    std::vector<RegexSymbol> suffix;
    for (auto symbol : regex_)
    {
        if (symbol.second && symbol.first != '(' && symbol.first != ')')
        {//是运算符
            if (op.empty())//栈空，直接入栈
                op.emplace(symbol);
            else
            {//优先级更大的运算符全部出栈
                while (!op.empty())
                {
                    RegexSymbol t = op.top();
                    if (GetPriority(symbol) <= GetPriority(t))
                    {
                        op.pop();
                        suffix.push_back(t);
                    }
                    else
                        break;
                }
                op.emplace(symbol);
            }
        }
        else
        {
            if (symbol.first == '(' && symbol.second)//左括号直接入栈
                op.emplace(symbol);
            else if (symbol.first == ')' && symbol.second)
            {//遇到右括号，一直出栈，直到遇到左括号
                while (! (op.top().first == '(' && op.top().second))
                {
                    suffix.push_back(op.top());
                    op.pop();
                }
                op.pop();
            }
            else
                suffix.push_back(symbol);//操作数直接放入表达式中
        }
    }
    while (!op.empty())
    {//取出剩余的运算符
        suffix.push_back(op.top());
        op.pop();
    }

    regex_ = suffix;
    // std::cout << "后缀表达式：" << regex_ << std::endl;
}

bool FA::IsOperator(char c)
{//判断是不是运算符
    switch (c)
    {
        case '*':
        case '|':
        case '+':
            return true;
        default:
            return false;
    }
}

int FA::GetPriority(RegexSymbol symbol)
{
    int level = 0; // 优先级

    if (symbol.second)
    {
        switch (symbol.first)
        {
            case '(':
                level = 1;
                break;
            case '|':
                level = 2;
                break;
            case '+':
                level = 3;
                break;
            case '*':
                level = 4;
                break;
        }
    }

    return level;
}

DFA FA::GetDFA()
{
    return dfa_;
}

std::string FA::table_post_name_ = "_DFA_table";
void FA::OutputDFATable()
{
    std::string filename = "../Lexer/DFATables/" + FA_name_ + table_post_name_;

    std::ofstream out(filename, std::ios::out);
    if (!out.is_open())
    {
        std::cerr << "打开文件失败，无法写入内容到DFA_table中！" << std::endl;
        exit(-1);
    }

    // 第一行输出初态
    out << dfa_.start_state_ << "\n";

    // 第二行输出终态集合
    for (const auto& end_state : dfa_.GetEndStateSet())
        out << end_state << " ";
    out << "\n";

    // 输出：当前状态 接收符号 转移到状态
    for (const auto& it1 : dfa_.dfa_table_)
    {
        for (const auto& it2 : it1.second)
        {
            out << it1.first << " " << it2.first << " " <<  it2.second << "\n";
        }
    }

    out.flush();
    out.close();
}
