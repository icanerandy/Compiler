/**
  ******************************************************************************
  * @file           : LR1Parser.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/24
  ******************************************************************************
  */


#ifndef GRAMMAR_LR0PARSER_H
#define GRAMMAR_LR0PARSER_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

using LeftPart = std::string;
using RightPart = std::vector<std::string>;

struct Production
{
    LeftPart left;
    RightPart right;

    bool operator==(const Production& rhs) const
    {
        if (left != rhs.left)
            return false;
        if (right.size() != rhs.right.size())
            return false;
        for (size_t i = 0; i < right.size(); ++i)
        {
            if (rhs.right.at(i) != right.at(i))
                return false;
        }
        return true;
    }
};

struct Grammar
{
    size_t num; // 产生式数量
    std::vector<std::string> T; // 终结符
    std::vector<std::string> N; // 非终结符
    std::vector<Production> prods;  // 产生式
};

// LR0项目
struct LR0Item
{
    Production prod;
    size_t location{};    // 点所在的位置
};

// LR0项目集
struct LR0Items
{
    std::vector<LR0Item> items{};
};

// LR0项目集规范族
struct CanonicalCollection
{
    std::vector<LR0Items> items;    // 项目集集合
    std::vector< std::pair<std::string, size_t> > g[100];   // 保存DFA的图，first是经过什么转移，second是转移到的状态编号
};

class LR1Parser
{
public:
    explicit LR1Parser(const std::string& grammar_in, const std::string& grammar_output, const std::string& lr_parse_result);
    ~LR1Parser();

private:
    void InitGrammar();
    void DFA();
    void CreateAnalysisTable();
    void Closure(LR0Items& lr0Items);
    void Go(LR0Items& from, const std::string& symbol, LR0Items& to);
    bool IsTerminal(const std::string& symbol);
    bool IsNonTerminal(const std::string& symbol);
    bool IsInLR0Items(const LR0Items& lr0Items, const LR0Item& lr0Item) const;
    size_t IsInCanonicalCollection(LR0Items& lr0Items);
    void GetFirstSet();
    void GetFollowSet();


private:
    Grammar grammar_;
    CanonicalCollection canonicalCollection_;
    std::queue< std::pair<LR0Items, size_t> > Q;    // DFA队列，用于存储待转移的有效项目集
    std::pair<size_t, size_t> action_[100][100];    // first表示分析动作，0->ACC 1->S 2->R，second表示转移状态或者产生式编号
    size_t goto_[100][100];

private:
    std::ifstream grammar_in_;
    std::ofstream grammar_out_;
    std::ofstream lr_parse_result_;
};


#endif //GRAMMAR_LR0PARSER_H
