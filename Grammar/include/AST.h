/**
  ******************************************************************************
  * @file           : AST.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/4
  ******************************************************************************
  */

//
// Created by icaner on 2023/5/4.
//

#ifndef COMPILER_AST_H
#define COMPILER_AST_H

#include <string>
#include <list>
#include <stack>
#include <map>
#include <iostream>
#include <fstream>

class ASTNode
{
public:
    ASTNode();
    ~ASTNode();

public:
    inline void SetParents(ASTNode*);
    inline size_t GetChildrenSize();
    inline bool IsRoot();
    inline bool IsLeaf();
    inline std::list<ASTNode*> GetChildrenList() const;
    inline struct ASTNode* GetParent() const;

public:
    std::string ast_name_;

    std::list<ASTNode *> children_;
    struct ASTNode * pre_;
    struct ASTNode * next_;
    struct ASTNode * parent_;

    std::string content_;	// tokenԭʼ����
    std::string type_;	    // token����
    size_t line_no_;
    size_t column_no_;
};

class ASTTree
{
public:
    explicit ASTTree(ASTNode * root);
    ~ASTTree();

public:
    void PrettyPrint();   // �����������ӽڵ㼰���ڵ㶼Ϊ������λ�ò���һ��ֵΪż���Ľڵ�
    void print_node(const ASTNode *node, int level, const std::string &indent, bool is_last_sibling);
    void RemoveEmptyNodes(ASTNode* node);
    void RemoveEmptyNodes();

protected:
    ASTNode * root_;
    std::ofstream tree_out;
};

#endif //COMPILER_AST_H
