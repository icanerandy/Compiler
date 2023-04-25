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
    void PrintGrammar();    // ����ķ�
    void ProcessGrammar();  // �ķ�����
    std::string GetAddNonTerminalName(NonTerminal new_left_part);
    void ExtractLeftFactoring(); // ��ȡ������
    std::vector<RightPart> Classify(const NonTerminal& left_part);    // ����
    std::vector<std::string> DecompositionExtract(const NonTerminal& left_part, RightPart& list); // �Ե������������ӽ��зֽ���ȡ
    void RemoveLeftRecursion();   // ������ݹ�
    void RemoveLeftRecursionHelper(const NonTerminal& left_part); // ����ֱ����ݹ�

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
    std::unordered_set<Terminal> terminal_set_; // �ս������
    std::unordered_set<NonTerminal> nullable_sets_; // NULLABLE ����
    std::unordered_map<NonTerminal, std::unordered_set<Terminal>> first_sets_; // FIRST ����
    std::unordered_map<NonTerminal, std::unordered_set<Terminal>> follow_sets_; // FOLLOW ����
    std::unordered_map<NonTerminal, std::unordered_map<Terminal, Productions>> parse_table_; // ������
};

#endif //GRAMMAR_LL1PARSER_H
