//
// Created by icaner on 2023/4/20.
//

#ifndef GRAMMAR_GRAMMAR_H
#define GRAMMAR_GRAMMAR_H

#include <iostream>
#include <string>
#include <stack>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <vector>
#include <memory>
#include <iomanip>
#include <locale>
#include<bits/stdc++.h>

typedef std::vector<std::string> Production;
typedef std::vector<Production> Productions;
typedef std::vector<Production> RightPart;
typedef std::string NonTerminal;
typedef std::string Terminal;

class Grammar
{
public:
    Grammar(const std::string& grammar_file, std::string  start_symbol);    // 读入文法文件初始化
    void PrintGrammar();    // 打印文法

public:
    std::string get_start_symbol(); // 获取开始符号
    const std::unordered_map<NonTerminal, RightPart>& get_grammar();   // 获取文法

private:
    void ProcessGrammar(const std::string& grammar_file);  // 文法处理
    std::string GetAddNonTerminalName(NonTerminal new_left_part);
    void ExtractLeftFactoring(); // 提取左公因子
    std::vector<RightPart> Classify(const NonTerminal& left_part);    // 归类
    std::vector<std::string> DecompositionExtract(const NonTerminal& left_part, RightPart& list); // 对单独类别的左公因子进行分解提取
    void RemoveLeftRecursion();   // 消除左递归
    void RemoveLeftRecursionHelper(const NonTerminal& left_part); // 消除直接左递归

private:
    std::string start_symbol_;
    std::unordered_map<NonTerminal, RightPart> grammar_;
};

#endif //GRAMMAR_GRAMMAR_H
