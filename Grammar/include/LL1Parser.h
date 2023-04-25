#ifndef GRAMMAR_LL1PARSER_H
#define GRAMMAR_LL1PARSER_H

#include <iostream>
#include <string>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <memory>
#include <iomanip>
#include <locale>
#include<bits/stdc++.h>

#include "include/Grammar.h"

class LL1Parser {
public:
    explicit LL1Parser(Grammar grammar);
    ~LL1Parser();
    bool Parse(const std::vector<std::string>& tokens);
    void PrintFirstAndFollowSets();
    void PrintParseTable();

private:
    static std::vector<std::string> tokenize(const std::string& input);

private:
    void PrintGrammar();    // 输出文法
    void ProcessGrammar();  // 文法处理
    std::string GetAddNonTerminalName(NonTerminal new_left_part);
    void ExtractLeftFactoring(); // 提取左公因子
    std::vector<RightPart> Classify(const NonTerminal& left_part);    // 归类
    std::vector<std::string> DecompositionExtract(const NonTerminal& left_part, RightPart& list); // 对单独类别的左公因子进行分解提取
    void RemoveLeftRecursion();   // 消除左递归
    void RemoveLeftRecursionHelper(const NonTerminal& left_part); // 消除直接左递归

private:
    void build_terminal_set();
    void build_nullable_sets();
    void build_first_sets();
    void build_follow_sets();
    void build_parse_table();

private:
    std::unordered_set<Terminal> GetFirstSSet(const NonTerminal& non_terminal, const Production& production);

private:
    bool IsTerminal(const std::string& symbol);
    static bool IsEpsilon(const Terminal& symbol);

private:
    NonTerminal start_symbol_;
    std::unordered_map<NonTerminal, RightPart> grammar_;
    std::unordered_set<Terminal> terminal_set_; // 终结符集合
    std::unordered_set<NonTerminal> nullable_sets_; // NULLABLE 集合
    std::unordered_map<NonTerminal, std::unordered_set<Terminal>> first_sets_; // FIRST 集合
    std::unordered_map<NonTerminal, std::unordered_set<Terminal>> follow_sets_; // FOLLOW 集合
    std::unordered_map<NonTerminal, std::unordered_map<Terminal, Productions>> parse_table_; // 分析表
};

#endif //GRAMMAR_LL1PARSER_H
