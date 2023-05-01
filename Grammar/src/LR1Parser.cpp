/**
  ******************************************************************************
  * @file           : LR1Parser.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/24
  ******************************************************************************
  */

#include "include/LR1Parser.h"

LR1Parser::LR1Parser(const std::vector<Token>& tokens, const std::string& grammar_in, const std::string& lr_parse_result)
    : tokens_(tokens)
{
    grammar_in_.open(grammar_in, std::ios::in);
    if (!grammar_in_.is_open())
    {
        std::cerr << "读取语法文件失败！" << std::endl;
        system("pause");
        exit(-1);
    }
    lr_parse_result_.open(lr_parse_result, std::ios::out);
    if (!lr_parse_result_.is_open())
    {
        std::cerr << "输出LL(1)分析结果失败！" << std::endl;
        system("pause");
        exit(-1);
    }

    // 初始化文法
    InitGrammar();

    // 求FIRST集和FOLLOW集
    GetFirstSet();
    GetFollowSet();

    // 构建DFA和SLR1预测分析表
    DFA();

    // 初始化action和goto表为空
    InitAction();
    InitGoto();

    // 生成SLR1分析表
    CreateAnalysisTable();

    // 语法分析
    Parse();
 }

void LR1Parser::InitGrammar()
{
    std::string line;

    // 读入文法
    while (std::getline(grammar_in_, line))
    {
        LeftPart left;
        RightPart right;
        Production prod;

        // 读入左部
        std::stringstream ss(line);
        ss >> left;
        prod.left = left;

        // 将->吞掉
        std::string arrow;
        ss >> arrow;

        // 遍历右部符号
        std::string symbol;
        while (ss >> symbol)
        {
            if (symbol == "|")
            {
                grammar_.prods.emplace_back(prod);
                prod.right.clear();
            }
            else
            {
                prod.right.emplace_back(symbol);
            }
        }
        grammar_.prods.emplace_back(prod);
    }
    /*
     * 为什么要添加一个新的产生式？
     * 因为要将表达式转为NFA，如果开始符号对应多个产生式，则NFA的初态也会有多个
     * 虽然能处理多个初态，但显得很麻烦，不如新填一条产生式
     */
    // 将S'->S加入开头
    LeftPart start_left = grammar_.prods.at(0).left + '\'';
    RightPart start_right;
    start_right.emplace_back(grammar_.prods.at(0).left);
    grammar_.prods.insert(grammar_.prods.begin(), {start_left, start_right});
    grammar_.num = grammar_.prods.size();

    // 左部的都为非终结符
    for (const auto& prod : grammar_.prods)
        grammar_.N.emplace_back(prod.left);
    // 去重
    std::vector<std::string>::iterator it, it1;
    for (it = ++grammar_.N.begin(); it != grammar_.N.end();)
    {
        it1 = find(grammar_.N.begin(), it, *it); //若当前位置之前存在重复元素，删除当前元素,erase返回当前元素的下一个元素指针
        if (it1 != it)
            it = grammar_.N.erase(it);
        else
            it++;
    }


    // 右部中没有在左部中出现过的符号都为终结符，注意程序将ε当作终结符
    for (const auto& prod : grammar_.prods)
    {
        for (const auto& symbol : prod.right)
        {
            if (std::find(grammar_.N.begin(), grammar_.N.end(), symbol) == grammar_.N.end())
                    grammar_.T.emplace_back(symbol);
        }
    }
    // 去重
    for (it = ++grammar_.T.begin(); it != grammar_.T.end();)
    {
        it1 = find(grammar_.T.begin(), it, *it); //若当前位置之前存在重复元素，删除当前元素,erase返回当前元素的下一个元素指针
        if (it1 != it)
            it = grammar_.T.erase(it);
        else
            it++;
    }
}

void LR1Parser::InitAction()
{
    for (size_t i = 0; i < canonicalCollection_.items.size(); ++i)
    {
        for (const auto & symbol : grammar_.T)
        {
            if (symbol != "<epsilon>")
                action_[i][symbol] = std::make_pair("", -1);
        }
        // 不要忘了把结束符放进去
        action_[i]["#"] = std::make_pair("", -1);
    }
}

void LR1Parser::InitGoto()
{
    for (size_t i = 0; i < canonicalCollection_.items.size(); ++i)
    {
        for (const auto & symbol : grammar_.N)
            goto_[i][symbol] = -1;
    }
}

void LR1Parser::DFA()
{
    // 构建初始项目集
    LR1Item lr1Item;
    lr1Item.location = 0;
    lr1Item.prod.left = grammar_.prods[0].left;
    lr1Item.prod.right = grammar_.prods[0].right;
    lr1Item.next = "#";

    LR1Items lr1Items;
    lr1Items.items.emplace_back(lr1Item);
    Closure(lr1Items);

    // 加入初始有效项目集
    canonicalCollection_.items.emplace_back(lr1Items);
    // 将新加入的有效项目集加入待扩展队列中
    Q.emplace(lr1Items, 0);

    while (!Q.empty())
    {
        LR1Items& from = Q.front().first;
        size_t sidx = Q.front().second;
        // 遍历每个终结符
        for (auto & symbol : grammar_.T)
        {
            LR1Items to;
            Go(from, symbol, to);
            size_t idx;
            // 若求出的项目集to不为空
            if (!to.items.empty())
            {
                // 查找是否已经在有效项目集族中
                idx = IsInCanonicalCollection(to);
                if (idx > 0)
                {
                    idx -= 1;
                }
                else
                {
                    idx = canonicalCollection_.items.size();
                    canonicalCollection_.items.emplace_back(to);
                    // 把新加入的有效项目集加入待扩展队列中
                    Q.emplace(to, idx);
                }
                // 从原状态到转移状态加一条边，边上的值为转移符号
                canonicalCollection_.g[sidx].emplace_back(symbol, idx);
            }
        }

        // 遍历每个非终结符
        for (auto & symbol : grammar_.N)
        {
            LR1Items to;
            Go(from, symbol, to);
            size_t idx;
            if (!to.items.empty())
            {
                idx = IsInCanonicalCollection(to);
                if (idx > 0)
                {
                    idx -= 1;
                }
                else
                {
                    idx = canonicalCollection_.items.size();
                    canonicalCollection_.items.emplace_back(to);
                    Q.emplace(to, idx);
                }
                canonicalCollection_.g[sidx].emplace_back(symbol, idx);
            }
        }

        // 当前状态扩展完成，将其移除队列
        Q.pop();
    }

//    // 移除DFA图中的重复元素
//    for (auto & it : canonicalCollection_.g)
//    {
//        std::set< std::pair<std::string, size_t> > tmp(it.begin(), it.end());
//        it.assign(tmp.begin(), tmp.end());
//    }
}

void LR1Parser::CreateAnalysisTable()
{
    for (size_t i = 0; i < canonicalCollection_.items.size(); ++i)
    {
        LR1Items& LItems = canonicalCollection_.items.at(i);

        // 构建action表
        for (const auto & LItem : LItems.items)
        {
            // 非归约项目
            if (LItem.location < LItem.prod.right.size() && LItem.prod.right.at(LItem.location) != "<epsilon>")
            {
                std::string symbol = LItem.prod.right.at(LItem.location);

                if (IsTerminal(symbol))
                {
                    // 找到对应终结符的出边，得到其转移到的状态
                    for (size_t k = 0; k < canonicalCollection_.g.at(i).size(); ++k)
                    {
                        std::pair<std::string, size_t> pair = canonicalCollection_.g.at(i).at(k);
                        if (pair.first == symbol)
                        {
                            action_.at(i).at(symbol).first = "Shift";
                            action_.at(i).at(symbol).second = pair.second;

                            /*
                             * 对于
                             * S -> a b c
                             * S -> a b d
                             * 怎么处理？
                             */
                            break;
                        }
                    }
                }
            }
            else    // 归约项目
            {
                /* 接收项目 */
                if (LItem.prod.left == grammar_.prods.at(0).left && LItem.location == LItem.prod.right.size())
                {
                    action_.at(i).at("#").first = "Accept";
                }
                else
                {
                    // 找到产生式对于的序号
                    for (size_t k = 0; k < grammar_.prods.size(); ++k)
                    {
                        if (LItem.prod == grammar_.prods.at(k))
                        {
                            action_.at(i).at(LItem.next).first = "Reduce";
                            action_.at(i).at(LItem.next).second = k;
                            break;
                        }
                    }
                }
            }
        }

        // 说明当前状态不存在出边
        if (canonicalCollection_.g.find(i) == canonicalCollection_.g.end())
            continue;

        // 构建goto表
        // 遍历当前项目集的pair<接受符号，转移状态>
        for (const auto & it : canonicalCollection_.g.at(i))
        {
            std::string symbol = it.first;
            if (IsNonTerminal(symbol))
                goto_.at(i).at(symbol) = it.second;
        }
    }
}

void LR1Parser::Parse()
{
    std::cout << "Syntax parsing start!" << std::endl;
    /* 初始化 */
    std::stack<size_t> state_stack; // 状态栈
    std::stack<std::string> symbol_stack; // 符号栈
    state_stack.emplace(0);
    symbol_stack.emplace("#");

    size_t ip = 0;
    do
    {
        std::cout << "处理第" << ip << "个token..." << std::endl;
        std::string type = tokens_.at(ip)./*type*/content;

        size_t cur_state = state_stack.top();
        if (action_.at(cur_state).at(type).first == "Shift")
        {
            state_stack.emplace(action_.at(cur_state).at(type).second);
            symbol_stack.emplace(type);
            ++ip;
        }
        else if (action_.at(cur_state).at(type).first == "Reduce")
        {
            size_t idx = action_.at(cur_state).at(type).second;
            Production& prod = grammar_.prods.at(idx);

            size_t size = 0;
            if (!(prod.right.at(0) == "<epsilon>"))
                size = prod.right.size();
            for (; size >= 1; size--)
            {
                state_stack.pop();
                symbol_stack.pop();
            }

            symbol_stack.emplace(prod.left);
            cur_state = state_stack.top();

            cur_state = goto_.at(cur_state).at(prod.left);
            if (cur_state == -1)
            {
                // TODO: 错误处理
                std::cerr << "Syntax error at " << "(" << tokens_.at(ip).line << ", " << tokens_.at(ip).column << ")" << std::endl;
                break;
            }
            else
            {
                state_stack.emplace(cur_state);
            }
        }
        else if (action_.at(cur_state).at(type).first == "Accept")
        {
            std::cout << "语法分析成功！" << std::endl;
            break;
        }
        else
        {
            // TODO: 错误处理
            std::cerr << "Syntax error at " << "(" << tokens_.at(ip).line << ", " << tokens_.at(ip).column << ")" << std::endl;
            break;
        }
    } while (true);

}

void LR1Parser::Closure(LR1Items& lr1Items)
{
    bool still_changing = true;
    while (still_changing)  // 开始迭代
    {
        still_changing = false;

        LR1Items tmp_items;
        // 枚举每个项目
        tmp_items.items.assign(lr1Items.items.begin(), lr1Items.items.end());
        for (auto & L : tmp_items.items)
        {
            // 非归约项目
            if (L.location < L.prod.right.size())
            {
                std::string symbol = L.prod.right.at(L.location);
                if (IsNonTerminal(symbol))  // 把符合条件的LR1项目加入闭包中
                {
                    // 先求出非终结符后面的串的FIRST集
                    std::vector<std::string> ss;
                    ss.assign(L.prod.right.begin() + L.location + 1, L.prod.right.end());
                    ss.emplace_back(L.next);
                    std::set<std::string> string_first_set = GetStringFirstSet(ss);

                    for (auto & prod : grammar_.prods)
                    {
                        if (prod.left == symbol)
                        {
                            /* 枚举每个在B后面的串的FIRST集 */
                            for (const auto& it : string_first_set)
                            {
                                LR1Item new_item;
                                new_item.location = 0;
                                new_item.prod = prod;
                                new_item.next = it;
                                // 查看是否已经在当前的LR1项目集中
                                if (!IsInLR1Items(lr1Items, new_item))
                                {
                                    still_changing = true;  // 继续迭代
                                    lr1Items.items.emplace_back(new_item);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

/*!
 * 转移函数
 * @param from - 当前的项目集
 * @param symbol - 经symbol转移
 * @param to - 转移后的项目集转移后的项目集
 */
void LR1Parser::Go(LR1Items &from, const std::string& symbol, LR1Items &to)
{
    for (const auto& item : from.items)
    {
        // 寻找非归约项目
        if (item.location < item.prod.right.size() && item.prod.right.at(item.location) != "<epsilon>")
        {
            std::string next_symbol = item.prod.right.at(item.location);
            // 如果点后面是非终结符，且和传入的符号相同
            if (next_symbol == symbol)
            {
                LR1Item lr1Item;
                lr1Item.location = item.location + 1;
                lr1Item.prod.left.assign(item.prod.left);
                lr1Item.next = item.next;
                lr1Item.prod.right.assign(item.prod.right.begin(), item.prod.right.end());
                to.items.emplace_back(lr1Item);
            }
        }
    }
    // 若to中有项目，求其闭包
    if (!to.items.empty())
        Closure(to);
}

/*!
 * 求（T U N）的FIRST集
 */
void LR1Parser::GetFirstSet()
{
    // 终结符的FIRST集是其本身
    for (const auto& symbol : grammar_.T)
        first_set_[symbol].emplace(symbol);
    for (const auto& symbol : grammar_.N)
        first_set_[symbol] = {};

    for (const auto& symbol : first_set_)
        std::cout << symbol.first << "\t";

    std::cout << "\n\n";
    for (const auto& symbol : grammar_.prods)
        std::cout << symbol.left << "\t";

    // 迭代查找非终结符的FIRST集
    bool still_changing = true;
    while (still_changing)
    {
        still_changing = false;
        for (const auto & prod : grammar_.prods)
        {
            size_t old_size = first_set_.at(prod.left).size();

            // 如果右部第一个符号是空或终结符，则加入到左部的FIRST集中
            std::string symbol = prod.right.at(0);
            if (IsTerminal(symbol) || symbol == "<epsilon>")
            {
                first_set_.at(prod.left).emplace(symbol);
            }
            else
            {
                bool next = true; // 当前符号是非终结符，且当前符号可以推出空，则还需要判断下一个符号
                size_t idx = 0; // 待判断符号的下标
                while (next && idx < prod.right.size())
                {
                    next = false;
                    symbol = prod.right.at(idx);
                    // 遍历当前符号的FIRST集
                    for (const auto& it : first_set_.at(symbol))
                    {
                        // 把当前符号的FIRST集中非空元素加入到左部符号的FIRST集中
                        // 右部所有符号都能推导出ε，则把ε加入到左部符号的FIRST集中
                        if (it != "<epsilon>")
                        {
                            first_set_.at(prod.left).emplace(it);
                        }
                        else    // 当前符号能推导出ε，则还需判断下一个符号
                        {
                            next = true;
                            idx += 1;
                        }
                    }
                }
                // 如果next为true，则产生式右部每一个符号都能推导出ε，将ε加入左部的FIRST集中
                if (next)
                    first_set_.at(prod.left).emplace("<epsilon>");
            }

            std::cout << prod.left << std::endl;
            size_t new_size = first_set_.at(prod.left).size();
            if (new_size != old_size)
                still_changing = true;
        }
    }

    //system("cls");
    std::cout << "FIRST:" << std::endl;
    for (const auto & it : grammar_.N)
    {
        std::cout << it << ": ";
        for (const auto & it1 : first_set_.at(it))
            std::cout << it1 << " ";
        std::cout << std::endl;
    }
    //system("pause");
}

/*!
 * 求非终结符的FOLLOW集
 */
void LR1Parser::GetFollowSet()
{
    // 初始化终结符的FOLLOW集为空寂
    for (const auto& symbol : grammar_.N)
        follow_set_[symbol] = {};

    // 将#加入到文法的开始符号的FOLLOW集中
    follow_set_.at(grammar_.N.at(0)).emplace("#");

    bool still_changing = true;
    while (still_changing)
    {
        still_changing = false;
        for (const auto & prod : grammar_.prods)
        {
            for (size_t i = 0; i < prod.right.size(); ++i)
            {
                if (IsNonTerminal(prod.right.at(i)))
                {

                    size_t old_size = follow_set_.at(prod.right.at(i)).size();
                    // ss是从下一个符号开始的符号串
                    std::vector<std::string> ss(prod.right.begin() + i + 1, prod.right.end());
                    std::set<std::string> afters_first_set = GetStringFirstSet(ss);
                    bool can_result_epsilon = false;
                    // 将ss的FIRST集中所有非空元素加入到当前符号的FOLLOW集中
                    for (const auto & it : afters_first_set)
                    {
                        if (it != "<epsilon>")
                            follow_set_.at(prod.right.at(i)).emplace(it);
                        else
                            can_result_epsilon = true;
                    }

                    // 如果ss能推导出ε，或者当前符号是产生式右部末尾
                    if (can_result_epsilon || (i+1 == prod.right.size()))
                        follow_set_.at(prod.right.at(i)).insert(
                                follow_set_.at(prod.left).begin(),
                                follow_set_.at(prod.left).end()
                                );

                    size_t new_size = follow_set_.at(prod.right.at(i)).size();
                    if (new_size != old_size)
                        still_changing = true;
                }
            }
        }
    }

    //system("cls");
    std::cout << "FOLLOW:" << std::endl;
    for (const auto & it : grammar_.N)
    {
        std::cout << it << ": ";
        for (const auto & it1 : follow_set_.at(it))
            std::cout << it1 << " ";
        std::cout << std::endl;
    }
    //system("pause");
}

/*!
 * 求串的FIRST集中，返回结果集
 * @param ss
 * @return
 */
std::set<std::string> LR1Parser::GetStringFirstSet(const std::vector<std::string>& ss)
{
    std::set<std::string> first;

    if (ss.empty())
        return first;

    // 当前符号是非终结符，且当前符号可以推导出空，则还需要判断下一个符号
    bool still_changing = true;
    size_t idx = 0;
    while (still_changing && idx < ss.size())
    {
        still_changing = false;
        // 当前符号是终结符或空，加入到FIRST集中
        if (IsTerminal(ss.at(idx)) || ss.at(idx) == "<epsilon>")
        {
            first.emplace(ss.at(idx));
        }
        else
        {
            for (const auto & symbol : first_set_.at(ss.at(idx)))
            {
                if (symbol != "<epsilon>")
                    first.emplace(symbol);
                else
                    still_changing = true;
            }
        }
        idx += 1;
    }

    // 如果到达产生式右部末尾还为真，说明整个串可以推导出空，将ε加入到FIRST集中
    if (still_changing)
        first.emplace("<epsilon>");

    return first;
}

bool LR1Parser::IsTerminal(const std::string& symbol)
{
    return std::find(grammar_.T.begin(), grammar_.T.end(), symbol) != grammar_.T.end() || symbol == "#";
}

bool LR1Parser::IsNonTerminal(const std::string& symbol)
{
    return std::find(grammar_.N.begin(), grammar_.N.end(), symbol) != grammar_.N.end();
}

bool LR1Parser::IsInLR1Items(const LR1Items& lr1Items, const LR1Item& lr1Item) const
{
    if (std::any_of(lr1Items.items.begin(), lr1Items.items.end(), [&lr1Item](const LR1Item& item) {
        return item.prod == lr1Item.prod && item.location == lr1Item.location && item.next == lr1Item.next;
    })) return true;
    return false;
}

/*!
 * 判断传入的项目集是否在项目集规范族中，若在返回其序号
 * @param lr1Items
 * @return
 */
size_t LR1Parser::IsInCanonicalCollection(LR1Items &lr1Items)
{
    for (size_t i = 0; i < canonicalCollection_.items.size(); ++i)
    {
        LR1Items& cur_items = canonicalCollection_.items.at(i);

        if (cur_items.items.size() != lr1Items.items.size())
            continue;

        // 每个项目都在该项目集中，则认为这两个项目集相等
        bool flag = true;
        for (const auto& item : cur_items.items)
        {
            if (!IsInLR1Items(lr1Items, item))
            {
                flag = false;
                break;
            }
        }

        if (flag)
        {
            return i + 1;
        }
    }
    return 0;
}

LR1Parser::~LR1Parser()
= default;
