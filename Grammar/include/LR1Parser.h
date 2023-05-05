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
#include <list>
#include <queue>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "include/Lexer.h"
#include "include/AST.h"

class LR1Parser
{
private:
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
        size_t num; // ����ʽ����
        std::vector<std::string> T; // �ս��
        std::vector<std::string> N; // ���ս��
        std::vector<Production> prods;  // ����ʽ
    };

    struct LR1Item
    {
        Production prod;
        size_t location{};  // �����ڵ�λ��
        std::string next;   // չ������
    };

    struct LR1Items
    {
        std::vector<LR1Item> items{};
    };

    struct CanonicalCollection
    {
        std::vector<LR1Items> items;    // ��Ŀ������
        std::map<size_t, std::vector< std::pair<std::string, size_t> >> g;   // ����DFA��ͼ��first�Ǿ���ʲôת�ƣ�second��ת�Ƶ���״̬���
    };

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
    ASTNode * MkLeaf(const Token& token);
    ASTNode * MkNode(size_t idx, std::stack<ASTNode *>& tree_node);

private:
    ASTTree* tree;

private:
    std::vector<Token> tokens_;
    Grammar grammar_;
    CanonicalCollection canonicalCollection_;
    std::queue< std::pair<LR1Items, size_t> > Q;    // DFA���У����ڴ洢��ת�Ƶ���Ч��Ŀ��
    // map<���ڵ�״̬, map<���շ���, pair<(�ƽ�����Լ������), ת��״̬>>>
    std::map<size_t, std::map<std::string, std::pair<std::string, size_t>>> action_;
    std::map<size_t, std::map<std::string, size_t>> goto_;

private:
    std::map<std::string, std::set<std::string>> first_set_;
    std::map<std::string, std::set<std::string>> follow_set_;

private:
    std::ifstream grammar_in_;
    std::ofstream out_;

};


#endif //GRAMMAR_LR0PARSER_H
