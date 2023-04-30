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

LR1Parser::LR1Parser(const std::string& grammar_in, const std::string& grammar_output, const std::string& lr_parse_result)
{
    grammar_in_.open(grammar_in, std::ios::in);
    if (!grammar_in_.is_open())
    {
        std::cerr << "读取语法文件失败！" << std::endl;
        system("pause");
        exit(-1);
    }
    grammar_out_.open(grammar_output, std::ios::out);
    if (!grammar_out_.is_open())
    {
        std::cerr << "输出修改后的语法文件失败！" << std::endl;
        system("pause");
        exit(-1);
    }
    lr_parse_result_.open(grammar_output, std::ios::out);
    if (!lr_parse_result_.is_open())
    {
        std::cerr << "输出LL(1)分析结果失败！" << std::endl;
        system("pause");
        exit(-1);
    }

    InitGrammar();
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

    // 右部中没有在左部中出现过的符号都为终结符
    for (const auto& prod : grammar_.prods)
    {
        for (const auto& symbol : prod.right)
        {
            if (std::find(grammar_.N.begin(), grammar_.N.end(), symbol) == grammar_.N.end())
                grammar_.T.emplace_back(symbol);
        }
    }

    // 求FIRST集和FOLLOW集

    // 构建DFA和SLR1预测分析表
    DFA();

}

void LR1Parser::DFA()
{
    // 构建初始项目集
    LR0Item lr0Item;
    lr0Item.location = 0;
    lr0Item.prod.left = grammar_.prods[0].left;
    lr0Item.prod.right = grammar_.prods[0].right;
    LR0Items lr0Items;
    lr0Items.items.emplace_back(lr0Item);
    Closure(lr0Items);

    // 加入初始有效项目集
    canonicalCollection_.items.emplace_back(lr0Items);
    // 将新加入的有效项目集加入待扩展队列中
    Q.emplace(lr0Items, 0);

    while (!Q.empty())
    {
        LR0Items& from = Q.front().first;
        size_t sidx = Q.front().second;
        // 遍历每个终结符
        for (auto & symbol : grammar_.T)
        {
            LR0Items to;
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
            LR0Items to;
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

    // 移除DFA图中的重复元素
    for (auto & it : canonicalCollection_.g)
    {
        std::set< std::pair<std::string, size_t> > tmp(it.begin(), it.end());
        it.assign(tmp.begin(), tmp.end());
    }
}

void LR1Parser::CreateAnalysisTable()
{

}

void LR1Parser::Closure(LR0Items& lr0Items)
{
    bool still_changing = true;
    while (still_changing)  // 开始迭代
    {
        still_changing = false;

        LR0Items tmp_items;
        // 枚举每个项目
        tmp_items.items.assign(lr0Items.items.begin(), lr0Items.items.end());
        for (auto & L : tmp_items.items)
        {
            // 非归约项目
            if (L.location < L.prod.right.size())
            {
                std::string symbol = L.prod.right.at(L.location);
                if (IsNonTerminal(symbol))  // 把符合条件的LR0项目加入闭包中
                {
                    for (auto & prod : grammar_.prods)
                    {
                        if (prod.left == symbol)
                        {
                            LR0Item new_item;
                            new_item.location = 0;
                            new_item.prod = prod;
                            // 查看是否已经在当前的LR0项目集中
                            if (!IsInLR0Items(lr0Items, new_item))
                            {
                                still_changing = true;  // 继续迭代
                                lr0Items.items.emplace_back(new_item);
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
void LR1Parser::Go(LR0Items &from, const std::string& symbol, LR0Items &to)
{
    for (const auto& item : from.items)
    {
        // 寻找非归约项目
        if (item.location < item.prod.right.size())
        {
            std::string next_symbol = item.prod.right.at(item.location);
            // 如果点后面是非终结符，且和传入的符号相同
            if (next_symbol == symbol)
            {
                LR0Item lr0Item;
                lr0Item.location = item.location + 1;
                lr0Item.prod.left.assign(item.prod.left);
                lr0Item.prod.right.assign(item.prod.right.begin(), item.prod.right.end());
                to.items.emplace_back(lr0Item);
            }
        }
    }
    // 若to中有项目，求其闭包
    if (!to.items.empty())
        Closure(to);
}

bool LR1Parser::IsTerminal(const std::string& symbol)
{
    return std::find(grammar_.T.begin(), grammar_.T.end(), symbol) != grammar_.T.end();
}

bool LR1Parser::IsNonTerminal(const std::string& symbol)
{
    return std::find(grammar_.N.begin(), grammar_.N.end(), symbol) != grammar_.N.end();
}

bool LR1Parser::IsInLR0Items(const LR0Items& lr0Items, const LR0Item& lr0Item) const
{
    if (std::any_of(lr0Items.items.begin(), lr0Items.items.end(), [&lr0Item](const LR0Item& item) {
        return item.prod == lr0Item.prod && item.location == lr0Item.location;
    })) return true;
    return false;
}

/*!
 * 判断传入的项目集是否在项目集规范族中，若在返回其序号
 * @param lr0Items
 * @return
 */
size_t LR1Parser::IsInCanonicalCollection(LR0Items &lr0Items)
{
    for (size_t i = 0; i < canonicalCollection_.items.size(); ++i)
    {
        LR0Items& cur_items = canonicalCollection_.items.at(i);

        if (cur_items.items.size() != lr0Items.items.size())
            continue;

        // 每个项目都在该项目集中，则认为这两个项目集相等
        bool flag = true;
        for (const auto& item : cur_items.items)
        {
            if (!IsInLR0Items(lr0Items, item))
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
