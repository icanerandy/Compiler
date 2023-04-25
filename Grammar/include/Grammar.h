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
    Grammar(const std::string& grammar_file, std::string  start_symbol);    // �����ķ��ļ���ʼ��
    void PrintGrammar();    // ��ӡ�ķ�

public:
    std::string get_start_symbol(); // ��ȡ��ʼ����
    const std::unordered_map<NonTerminal, RightPart>& get_grammar();   // ��ȡ�ķ�

private:
    void ProcessGrammar(const std::string& grammar_file);  // �ķ�����
    std::string GetAddNonTerminalName(NonTerminal new_left_part);
    void ExtractLeftFactoring(); // ��ȡ������
    std::vector<RightPart> Classify(const NonTerminal& left_part);    // ����
    std::vector<std::string> DecompositionExtract(const NonTerminal& left_part, RightPart& list); // �Ե������������ӽ��зֽ���ȡ
    void RemoveLeftRecursion();   // ������ݹ�
    void RemoveLeftRecursionHelper(const NonTerminal& left_part); // ����ֱ����ݹ�

private:
    std::string start_symbol_;
    std::unordered_map<NonTerminal, RightPart> grammar_;
};

#endif //GRAMMAR_GRAMMAR_H
