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
#include "include/Lexer.h"

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

// LR1项目
struct LR1Item
{
    Production prod;
    size_t location{};  // 点所在的位置
    std::string next;   // 展望符集
};

// LR0项目集
struct LR1Items
{
    std::vector<LR1Item> items{};
};

// LR0项目集规范族
struct CanonicalCollection
{
    std::vector<LR1Items> items;    // 项目集集合
    std::map<size_t, std::vector< std::pair<std::string, size_t> >> g;   // 保存DFA的图，first是经过什么转移，second是转移到的状态编号
};

class LR1Parser
{
public:
    explicit LR1Parser(const std::vector<Token>& tokens, const std::string& grammar_in, const std::string& lr_parse_result);
    ~LR1Parser();

private:
    void InitGrammar();
    void InitAction();
    void InitGoto();
    void DFA();
    void CreateAnalysisTable();
    void Parse();
    void Closure(LR1Items& LR1Items);
    void Go(LR1Items& from, const std::string& symbol, LR1Items& to);
    void GetFirstSet();
    void GetFollowSet();
    std::set<std::string> GetStringFirstSet(const std::vector<std::string>& ss);
    bool IsTerminal(const std::string& symbol);
    bool IsNonTerminal(const std::string& symbol);
    bool IsInLR1Items(const LR1Items& LR1Items, const LR1Item& LR1Item) const;
    size_t IsInCanonicalCollection(LR1Items& LR1Items);


private:
    std::vector<Token> tokens_;
    Grammar grammar_;
    CanonicalCollection canonicalCollection_;
    std::queue< std::pair<LR1Items, size_t> > Q;    // DFA队列，用于存储待转移的有效项目集
    // map<现在的状态, map<接收符号, pair<(移进，归约，接收), 转移状态>>>
    std::map<size_t, std::map<std::string, std::pair<std::string, size_t>>> action_;
    std::map<size_t, std::map<std::string, size_t>> goto_;

private:
    std::map<std::string, std::set<std::string>> first_set_;
    std::map<std::string, std::set<std::string>> follow_set_;

private:
    std::ifstream grammar_in_;
    std::ofstream lr_parse_result_;
};


#endif //GRAMMAR_LR0PARSER_H
