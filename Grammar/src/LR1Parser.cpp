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
    out_.open(lr_parse_result, std::ios::out);
    if (!out_.is_open())
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

    // 打印抽象树
    tree->RemoveEmptyNodes();
    tree->PrettyPrint();
 }

void LR1Parser::InitGrammar()
{
    std::string line;

    Production prod;

    // 读入左部
    std::getline(grammar_in_, line);
    std::stringstream ss(line);
    ss >> prod.left;

    // 将->吞掉
    std::string arrow;
    ss >> arrow;

    // 遍历右部符号
    std::string symbol;
    while (ss >> symbol)
        prod.right.emplace_back(symbol);
    grammar_.prods.emplace_back(prod);

    // 读入文法
    while (std::getline(grammar_in_, line))
    {
        // 读入第一个符号
        std::stringstream s(line);
        std::string first_symbol;
        s >> first_symbol;

        if (first_symbol.empty())
        {
            continue;
        }
        else if (first_symbol == "|")
        {
            prod.right.clear();
            while (s >> symbol)
                prod.right.emplace_back(symbol);
            grammar_.prods.emplace_back(prod);
        }
        else
        {
            prod.left = first_symbol;

            // 将->吞掉
            s >> arrow;

            prod.right.clear();
            while (s >> symbol)
                prod.right.emplace_back(symbol);
            grammar_.prods.emplace_back(prod);
        }
    }
 /*   // 读入文法
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

    // 文件最后一行可能用户错误多输入了一行回车，此时要消除
    if (grammar_.prods.at(grammar_.prods.size() - 1).left.empty())
        grammar_.prods.pop_back();*/

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
    for (const auto& production : grammar_.prods)
        grammar_.N.emplace_back(production.left);
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
    for (const auto& production : grammar_.prods)
    {
        for (const auto& sym : production.right)
        {
            if (std::find(grammar_.N.begin(), grammar_.N.end(), sym) == grammar_.N.end())
                    grammar_.T.emplace_back(sym);
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

/*    // 打印项目集规范族
    std::cout << "\n\n\n";
    std::cout << "canonicalCollection's size is " << canonicalCollection_.items.size() << "\n";
    for (size_t i = 0; i < canonicalCollection_.items.size(); ++i)
    {
        std::cout << "---------------------------------------------------------------------\n";
        std::cout << "LR1Items " << i << ":\n";
        const LR1Items& items = canonicalCollection_.items.at(i);
        for (const auto& item : items.items)
        {
            std::cout << item.prod.left << "->";
            for (size_t k = 0; k < item.prod.right.size(); ++k)
            {
                if (item.location == k)
                    std::cout << ".";
                std::cout << item.prod.right.at(k);
            }
            if (item.location == item.prod.right.size())
                std::cout << ".";
            std::cout << ", " << item.next << "\t";
        }
        std::cout << "\n";

        if (canonicalCollection_.g.find(i) != canonicalCollection_.g.end())
        {
            for (const auto& pair : canonicalCollection_.g.at(i))
            {
                std::cout << "using " << pair.first << " to " << pair.second << "\n";
            }
        }
    }
    std::cout << "---------------------------------------------------------------------\n";*/
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
                    // 判断是否存在移进-归约冲突
                    // 如果展望符是else，则选择移进
                    // 让else和if就近匹配
                    if (symbol != "else" && action_.at(i).at(symbol).first == "Reduce")
                    {
                        std::cerr << "存在移进-归约冲突：项目集（" << i << "）移进时冲突，" << LItem.prod.left << " -> ";
                        for (const auto& right : LItem.prod.right)
                            std::cerr << right << " ";
                        std::cerr << std::endl;
                    }

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
            // 归约项目
            else
            {
                /* 接收项目 */
                if (LItem.prod.left == grammar_.prods.at(0).left && LItem.location == LItem.prod.right.size())
                {
                    action_.at(i).at("#").first = "Accept";
                }
                else
                {
                    // 判断是否存在移进-归约冲突
                    if (action_.at(i).at(LItem.next).first == "Shift")
                    {
                        // 如果展望符是else，则选择移进
                        // 让else和if就近匹配
                        if (LItem.next == "else")
                            continue;

                        std::cerr << "存在移进-归约冲突：项目集（" << i << "）归约时冲突，" << LItem.prod.left << " -> ";
                        for (const auto& right : LItem.prod.right)
                            std::cerr << right << " ";
                        std::cerr << std::endl;
                    }

                    // 找到产生式对应的序号

                    // TODO:
                    // LR(1)一个归约项目的向前符号有多个则有多个项目除向前符号外都相同，这几个项目都会遍历到
                    // 然后在对应的接收向前符号时归约，即接收展望符集时归约
                    /* 这几个项目叫做同心项目，可以使用展望符的并集作为LR1项目的一部分 */
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
    out_ << "Syntax parsing start!\n";
    out_ << "---------------------------------------------------------------------\n";
    /* 初始化 */
    std::stack<size_t> state_stack; // 状态栈
    std::stack<std::string> symbol_stack; // 符号栈
    std::stack<ASTNode *> tree_node;    // 存放语法树节点
    state_stack.emplace(0);
    symbol_stack.emplace("#");

    size_t ip = 0;
    do
    {
        std::string symbol;
        if (tokens_.at(ip).type == "MAIN_ID" || tokens_.at(ip).type == "<函数名>" || tokens_.at(ip).type == "<常量>"
                || tokens_.at(ip).type == "<变量>" || tokens_.at(ip).type == "<常数>"
                || tokens_.at(ip).type == "<字符常量>" || tokens_.at(ip).type == "<字符串常量>")
            symbol = tokens_.at(ip).type;
        else
            symbol = tokens_.at(ip).content;

        out_ << "处理第" << ip << "个token " << "--- (" << tokens_.at(ip).type << ",  " << tokens_.at(ip).content << ")"
                    << "\t\t\t行列：" << "(" << tokens_.at(ip).line << " , " << tokens_.at(ip).column << ")\n";

        size_t cur_state = state_stack.top();

        if (action_.at(cur_state).find(symbol) == action_.at(cur_state).end())
        {
            // TODO: 错误处理
            std::cerr << "Syntax error at " << "(" << tokens_.at(ip).line << ", " << tokens_.at(ip).column << ")"
                        << "\t" << tokens_.at(ip).content << std::endl;
            break;
        }

        if (action_.at(cur_state).at(symbol).first == "Shift")
        {
            state_stack.emplace(action_.at(cur_state).at(symbol).second);
            symbol_stack.emplace(symbol);
            if (tokens_.at(ip).type == "<函数名>" || tokens_.at(ip).type == "<常量>" || tokens_.at(ip).type == "<变量>" || tokens_.at(ip).type == "<常数>" || tokens_.at(ip).type == "<字符常量>" || tokens_.at(ip).type == "<字符串常量>")
                tree_node.emplace(MkLeaf(tokens_.at(ip)));
            ++ip;
        }
        else if (action_.at(cur_state).at(symbol).first == "Reduce")
        {
            size_t idx = action_.at(cur_state).at(symbol).second;
            Production& prod = grammar_.prods.at(idx);

            out_ << "选择了产生式" << prod.left << " -> ";
            for (const auto& right : prod.right)
                out_ << right << " ";
            out_ << "进行归约\n";

            size_t size;
            if (prod.right.at(0) == "<epsilon>")
            {
                size = prod.right.size() - 1;
            }
            else
            {
                size = prod.right.size();
            }

            std::vector<std::string> s;  // 记录规约了哪些符号
            for (; size >= 1; size--)
            {
                state_stack.pop();
                s.emplace_back(symbol_stack.top());
                symbol_stack.pop();
            }
            ASTNode* node = MkNode(idx, tree_node);
            tree_node.emplace(node);

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
        else if (action_.at(cur_state).at(symbol).first == "Accept")
        {
            out_ << "语法分析成功！\n";
            std::cout << "语法分析成功！" << std::endl;
            auto * root = new ASTTree(tree_node.top());
            tree = root;
            break;
        }
        else
        {
            // TODO: 错误处理
            std::cerr << "Syntax error at " << "(" << tokens_.at(ip).line << ", " << tokens_.at(ip).column << ")" << std::endl;
            break;
        }
    } while (true);
    out_ << "---------------------------------------------------------------------\n";
    out_.flush();
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

            size_t new_size = first_set_.at(prod.left).size();
            if (new_size != old_size)
                still_changing = true;
        }
    }
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

ASTNode *LR1Parser::MkLeaf(const Token& token)
{
    auto * node = new ASTNode();
    node->ast_name_ = token.content;

    node->type_ = token.type;
    node->content_ = token.content;
    node->line_no_ = token.line;
    node->column_no_ = token.column;

    return node;
}

ASTNode *LR1Parser::MkNode(size_t idx, std::stack<ASTNode *>& tree_node)
{
    auto * node = new ASTNode();

    size_t size_of_prod = grammar_.prods.at(idx).right.size();

    ASTNode * tmp;

    switch (idx)
    {
        // TODO: 空串归约？
        case 0: /* 0. <程序>’ -> <程序> # */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 1: /* 1. <程序> -> <声明语句列表> <MAIN函数定义> <函数定义列表> */
        {
            node->ast_name_ = "程序";

            ASTNode * func_def_list = tree_node.top();  // 函数定义列表
            tree_node.pop();

            while (func_def_list)
            {
                node->children_.emplace_back(func_def_list);
                func_def_list = func_def_list->next_;
            }

            ASTNode * main_def = tree_node.top();   // MAIN函数定义
            tree_node.pop();
            node->children_.emplace_front(main_def);

            ASTNode * declaration_list = tree_node.top();   // 声明语句列表
            tree_node.pop();

            while (declaration_list)
            {
                node->children_.emplace_back(declaration_list);
                declaration_list = declaration_list->next_;
            }
        }
            break;
        case 2: /* 2. <MAIN函数定义> -> MAIN_ID ( ) <复合语句> */
        {
            node->ast_name_ = "MAIN函数定义";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 3: /* 3. <声明语句列表> -> <声明语句> <声明语句列表> */
        {
            ASTNode * declaration_list = tree_node.top();   // 声明语句列表
            tree_node.pop();

            ASTNode * declaration  = tree_node.top();   // 声明语句
            tree_node.pop();

            declaration->next_ = declaration_list;

            node = declaration;
        }
            break;
        case 4: /* 4. <声明语句列表> -> <epsilon> */
        {
            node = nullptr;
        }
            break;
        case 5: /* 5. <函数定义列表> -> <函数定义> <函数定义列表> */
        {
            ASTNode * func_def_list = tree_node.top();  // 函数定义列表
            tree_node.pop();

            ASTNode * func_def = tree_node.top();   // 函数定义
            tree_node.pop();

            func_def->next_ = func_def_list;

            node = func_def;
        }
            break;
        case 6: /* 6. <函数定义列表> -> <epsilon> */
        {
            return nullptr;
        }
            break;
        case 7: /* 7. <函数定义> -> <函数类型> <函数名> ( <函数定义形参列表> ) <复合语句> */
        {
            node->ast_name_ = "函数定义";

            ASTNode * compound_stmt = tree_node.top();
            tree_node.pop();

            ASTNode * func_def_args_list = tree_node.top();
            tree_node.pop();

            ASTNode * func_name = tree_node.top();
            tree_node.pop();

            ASTNode * type = tree_node.top();
            tree_node.pop();

            node->children_.emplace_front(compound_stmt);
            node->children_.emplace_front(func_def_args_list);
            node->children_.emplace_front(func_name);
            node->children_.emplace_front(type);
        }
            break;
        case 8: /* 8. <函数定义形参列表> -> <函数定义形参> */
        {
            node->ast_name_ = "函数定义形参列表";

            tmp = tree_node.top();
            tree_node.pop();

            while (tmp)
            {
                node->children_.emplace_back(tmp);
                tmp = tmp->next_;
            }
        }
            break;
        case 9: /* 9. <函数定义形参列表> -> <epsilon> */
        {
            return nullptr;
        }
            break;
        case 10: /* 10. <函数定义形参> -> <数据类型> <变量> */
        {
            ASTNode * var = tree_node.top();    // 变量
            tree_node.pop();

            ASTNode * type = tree_node.top();   // 数据类型
            tree_node.pop();

            type->children_.emplace_front(var);

            node = type;
        }
            break;
        case 11: /* 11. <函数定义形参> -> <数据类型> <变量> , <函数定义形参> */
        {
            ASTNode * func_args = tree_node.top();  // 函数定义形参
            tree_node.pop();

            ASTNode * var = tree_node.top();    // 变量
            tree_node.pop();

            ASTNode * type = tree_node.top();   // 数据类型
            tree_node.pop();

            type->children_.emplace_front(var);

            type->next_ = func_args;

            node = type;
        }
            break;
        case 12: /* 12. <复合语句> -> { <语句表> } */
        {
            node->ast_name_ = "复合语句";

            ASTNode * stmt_table = tree_node.top();
            tree_node.pop();

            while (stmt_table)
            {
                node->children_.emplace_back(stmt_table);
                stmt_table = stmt_table->next_;
            }
        }
            break;
        case 13: /* 13. <语句表> -> <语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 14: /* 14. <语句表> -> <语句> <语句表> */
        {
            ASTNode * stmt_table = tree_node.top();
            tree_node.pop();

            ASTNode * stmt = tree_node.top();
            tree_node.pop();

            stmt->next_ = stmt_table;

            node =  stmt;
        }
            break;
        case 15: /* 15. <执行语句> -> <数据处理语句>  */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 16: /* 16. <执行语句> -> <控制语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 17: /* 17. <数据处理语句> -> <表达式> ; */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 18: /* 18. <控制语句> -> <if语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 19: /* 19. <控制语句> -> <for语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 20: /* 20. <控制语句> -> <while语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 21: /* 21. <控制语句> -> <do-while语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 22: /* 22. <控制语句> -> <return语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 23: /* 23. <if语句> -> if ( <表达式> ) <循环语句> <if-tail> */
        {
            node->ast_name_ = "if语句";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 24: /* 24. <if-tail> -> else <循环语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 25: /* 25. <if-tail> -> <epsilon> */
        {
            return nullptr;
        }
            break;
        case 26: /* 26. <for语句> -> for ( <表达式> ; <表达式> ; <表达式> ) <循环语句> */
        {
            node->ast_name_ = "for语句";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 27: /* 27. <while语句> -> while ( <表达式> ) <循环语句> */
        {
            node->ast_name_ = "while语句";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 28: /* 28. <do-while语句> -> do <循环用复合语句> while ( <表达式> ) ; */
        {
            node->ast_name_ = "do-while语句";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 29: /* 29. <循环语句> -> <声明语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 30: /* 30. <循环语句> -> <循环执行语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 31: /* 31. <循环语句> -> <循环用复合语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 32: /* 32. <循环用复合语句> -> { <循环语句表> } */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 33: /* 33. <循环语句表> -> <循环语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 34: /* 34. <循环语句表> -> <循环语句> <循环语句表> */
        {
            node->ast_name_ = "循环语句表";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 35: /* 35. <循环执行语句> -> <if语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 36: /* 36. <循环执行语句> -> <for语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 37: /* 37. <循环执行语句> -> <while语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 38: /* 38. <循环执行语句> -> <do-while语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 39: /* 39. <循环执行语句> -> <return语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 40: /* 40. <循环执行语句> -> <break语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 41: /* 41. <循环执行语句> -> <continue语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 42: /* 42. <循环执行语句> -> <数据处理语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 43: /* 43. <return语句> -> return ; */
        {
            node->ast_name_ = "return语句";
        }
            break;
        case 44: /* 44. <return语句> -> return <表达式> ; */
        {
            node->ast_name_ = "return语句";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 45: /* 45. <break语句> -> break ; */
        {
            node->ast_name_ = "break语句";
        }
            break;
        case 46: /* 46. <continue语句> -> continue ; */
        {
            node->ast_name_ = "continue语句";
        }
            break;
        case 47: /* 47. <语句> -> <声明语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 48: /* 48. <语句> -> <执行语句> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 49: /* 49. <声明语句> -> <值声明> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 50: /* 50. <声明语句> -> <函数声明> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 51: /* 51. <值声明> -> <常量声明> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 52: /* 52. <值声明> -> <变量声明> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 53: /* 53. <常量声明> -> const <数据类型> <常量声明表> */
        {
            node->ast_name_ = "常量声明";

            ASTNode * table = tree_node.top();  // 常量声明表
            tree_node.pop();

            ASTNode * type = tree_node.top();   // 数据类型
            tree_node.pop();

            while (table)
            {
                type->children_.emplace_front(table);
                table = table->next_;
            }

            node->children_.emplace_front(type);
        }
            break;
        case 54: /* 54. <数据类型> -> int */
        {
            node->ast_name_ = "int";
        }
            break;
        case 55: /* 55. <数据类型> -> float */
        {
            node->ast_name_ = "float";
        }
            break;
        case 56: /* 56. <数据类型> -> char */
        {
            node->ast_name_ = "char";
        }
            break;
        case 57: /* 57. <数据类型> -> String */
        {
            node->ast_name_ = "String";
        }
            break;
        case 58: /* 58. <常量声明表> -> <常量> = <常数> ; */
        {
            node->ast_name_ = "=";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 59: /* 59. <常量声明表> -> <常量> = <常数> , <常量声明表> */
        {
            ASTNode * table = tree_node.top();  // 常量声明表
            tree_node.pop();

            ASTNode * const_num = tree_node.top();   // 常数
            tree_node.pop();

            ASTNode * const_var = tree_node.top();  // 常量
            tree_node.pop();

            node = new ASTNode();
            node->ast_name_ = "=";
            node->children_.emplace_front(const_num);
            node->children_.emplace_front(const_var);

            node->next_ = table;
        }
            break;
        case 60: /* 60. <变量声明> -> <数据类型> <变量声明表> */
        {
            node->ast_name_ = "变量声明";

            ASTNode * table = tree_node.top();  // 变量声明表
            tree_node.pop();

            ASTNode * type = tree_node.top();   // 数据类型
            tree_node.pop();

            while (table)
            {
                type->children_.emplace_back(table);
                table = table->next_;
            }

            node->children_.emplace_front(type);
        }
            break;
        case 61: /* 61. <变量声明表> -> <单变量声明> ; */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 62: /* 62. <变量声明表> -> <单变量声明> , <变量声明表> */
        {
            ASTNode * table = tree_node.top();  // 变量声明表
            tree_node.pop();

            ASTNode * single_decl = tree_node.top();  // 单变量声明
            tree_node.pop();

            single_decl->next_ = table;

            node = single_decl;
        }
            break;
        case 63: /* 63. <单变量声明> -> <变量> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 64: /* 64. <单变量声明> -> <变量> = <布尔表达式> */
        {
            node->ast_name_ = "=";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 65: /* 65. <函数声明> -> <函数类型> <函数名> ( <函数声明形参列表> ) ; */
        {
            node->ast_name_ = "函数声明";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 66: /* 66. <函数类型> -> <数据类型> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 67: /* 67. <函数类型> -> void*/
        {
            node->ast_name_ = "void";
        }
            break;
        case 68: /* 68. <函数声明形参列表> -> <函数声明形参> */
        {
            node->ast_name_ = "函数声明形参列表";

            ASTNode * func_args = tree_node.top();
            tree_node.pop();

            while (func_args)
            {
                node->children_.emplace_back(func_args);
                func_args = func_args->next_;
            }
        }
            break;
        case 69: /* 69. <函数声明形参列表> -> <epsilon> */
        {
            return nullptr;
        }
            break;
        case 70: /* 70. <函数声明形参> -> <数据类型> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 71: /* 71. <函数声明形参> -> <数据类型> , <函数声明形参> */
        {
            ASTNode * func_args = tree_node.top();  // 函数声明形参
            tree_node.pop();

            ASTNode * type = tree_node.top();   // 数据类型
            tree_node.pop();

            type->next_ = func_args;

            node = type;
        }
            break;
        case 72: /* 72. <表达式> -> <赋值表达式> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 73: /* 73. <表达式> -> <简单表达式> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 74: /* 74. <简单表达式> -> <布尔表达式> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 75: /* 75. <赋值表达式> -> <变量> = <布尔表达式> */
        {
            node->ast_name_ = "=";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 76: /* 76. <布尔表达式> -> <布尔表达式> || <布尔项> */
        {
            node->ast_name_ = "||";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 77: /* 77. <布尔表达式> -> <布尔项> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 78: /* 78. <布尔项> -> <布尔项> && <布尔因子> */
        {
            node->ast_name_ = "&&";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 79: /* 79. <布尔项> -> <布尔因子> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 80: /* 80. <布尔因子> -> ! <布尔因子> */
        {
            node->ast_name_ = "!";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 81: /* 81. <布尔因子> -> <关系表达式> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 82: /* 82. <关系表达式> -> <算术表达式> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 83: /* 83. <关系表达式> -> <算术表达式> <关系运算符> <算术表达式> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->ast_name_ = tmp->ast_name_;

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 84: /* 84. <关系运算符> -> > */
        {
            node->ast_name_ = ">";
        }
            break;
        case 85: /* 85. <关系运算符> -> < */
        {
            node->ast_name_ = "<";
        }
            break;
        case 86: /* 86. <关系运算符> -> >= */
        {
            node->ast_name_ = ">=";
        }
            break;
        case 87: /* 87. <关系运算符> -> <= */
        {
            node->ast_name_ = "<=";
        }
            break;
        case 88: /* 88. <关系运算符> -> == */
        {
            node->ast_name_ = "==";
        }
            break;
        case 89: /* 89. <关系运算符> -> != */
        {
            node->ast_name_ = "!=";
        }
            break;
        case 90: /* 90. <算术表达式> -> <项> + <算术表达式> */
        {
            node->ast_name_ = "+";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 91: /* 91. <算术表达式> -> <项> - <算术表达式> */
        {
            node->ast_name_ = "-";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 92: /* 92. <算术表达式> -> <项> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 93: /* 93. <项> -> <因子> * <项> */
        {
            node->ast_name_ = "*";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 94: /* 94. <项> -> <因子> / <项> */
        {
            node->ast_name_ = "/";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 95: /* 95. <项> -> <因子> % <项> */
        {
            node->ast_name_ = "%";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 96: /* 96. <项> -> <因子> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 97: /* 97. <因子> -> ( <算术表达式> ) */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 98: /* 98. <因子> -> <常数> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 99: /* 99. <因子> -> <字符常量> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 100: /* 100. <因子> -> <字符串常量> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 101: /* 101. <因子> -> <变量> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 102: /* 102. <因子> -> <函数调用> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 103: /* 103. <函数调用>  -> <函数名> ( <实参列表> ) */
        {
            node->ast_name_ = "函数调用";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 104: /* 104. <实参列表> -> <表达式> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 105: /* 105. <实参列表> -> <表达式> , <实参列表> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 106: /* 106. <实参列表> -> <epsilon> */
        {
            return nullptr;
        }
            break;
        default:
            break;
    }

    return node;
}

LR1Parser::~LR1Parser()
= default;
