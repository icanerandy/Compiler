//
// Created by icaner on 2023/4/20.
//

#include "include/Grammar.h"

#include <utility>

Grammar::Grammar(const std::string &grammar_file, std::string start_symbol)
    :
        start_symbol_(std::move(start_symbol))
{
    ProcessGrammar(grammar_file);

    PrintGrammar();
}

void Grammar::PrintGrammar()
{
    // ���ת������ķ�
    std::cout << "The grammar_:" << std::endl;
    std::cout << "------------------------------------" << std::endl;
    for (const auto &rule: grammar_)
    {
        std::cout << rule.first << " -> ";
        for (size_t i = 0; i < rule.second.size(); ++i)
        {
            Production production = rule.second.at(i);
            for (const auto & j : production)
            {
                std::cout << j << " ";
            }
            if (i != rule.second.size() - 1)
            {
                std::cout << "| ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << "------------------------------------" << std::endl;
}

const std::unordered_map<NonTerminal, RightPart>& Grammar::get_grammar()
{
    return grammar_;
}

std::string Grammar::get_start_symbol()
{
    return start_symbol_;
}

void Grammar::ProcessGrammar(const std::string& grammar_file)
{
    std::ifstream inputFile(grammar_file);
    std::string line;

    std::vector<std::pair<NonTerminal, RightPart>> reverseGrammar;

    while (std::getline(inputFile, line))
    {
        std::stringstream ss(line);
        std::string leftPart;
        std::vector<Production> productions;

        // ��ȡ���ս��
        ss >> leftPart;
        std::string arrow;
        ss >> arrow;

        // ��ȡ����ʽ
        Production production;
        std::string symbol;
        while (ss >> symbol)
        {
            if (symbol == "|")
            {
                productions.emplace_back(production);
                production.clear();
            }
            else
            {
                production.push_back(symbol);
            }
        }
        productions.emplace_back(production);

        reverseGrammar.emplace_back(leftPart, productions);
    }

    std::reverse(reverseGrammar.begin(), reverseGrammar.end());

    for (const auto& rule : reverseGrammar)
    {
        grammar_.emplace(rule.first, rule.second);
    }
}