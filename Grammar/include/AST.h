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
#include <algorithm>
#include <fstream>
#include "include/Symbol.h"
#include "include/ProgramGenerator.h"
#include "include/TypeSystem.h"

class ASTNode
{
public:
    ASTNode();
    ~ASTNode();

public:
    void SetParents(ASTNode*);
    size_t GetChildrenSize() const;
    bool IsRoot() const;
    bool IsLeaf() const;
    std::list<ASTNode*>& GetChildrenList();
    struct ASTNode* GetParent() const;
    void SetAttributeForLeaf(Symbol * symbol, std::string content);
    Symbol * GetSymbol();

public:
    std::string type_;	    // token类型
    std::string content_;	// token原始内容
    Symbol * symbol_;
    std::string res_type_;   // 经过递归处理后的返回结果的类型
    size_t line_no_;
    size_t column_no_;

    bool is_int_;
    int i_value_;
    float f_value_;
    std::string s_value_;
    char c_value_;

    std::list<ASTNode *> children_;
    struct ASTNode * pre_;
    struct ASTNode * next_;
    struct ASTNode * parent_;
};

class ASTTree
{
public:
    explicit ASTTree(ASTNode * root);
    ~ASTTree();

public:
    void AddTypeSystem(TypeSystem * typeSystem);
    void PrettyPrint();   // 遍历树并在子节点及父节点都为奇数的位置插入一个值为偶数的节点
    void print_node(const ASTNode *node, int level, const std::string &indent, bool is_last_sibling);
    void RemoveEmptyNodes(ASTNode* node);
    void RemoveEmptyNodes();
    ASTNode * ExecuteNode(ASTNode * root);
    void CpyChildToParent(ASTNode * parent, ASTNode * child);
    void CpyValueToTreeNode(ASTNode * root);
    std::string GetType(ASTNode * root);

private:
    int AOP(const std::string& op, int& v1, int& v2);
    float AOP(const std::string& op, float& v1, float& v2);
    int OOP(std::string op, int v1, int v2);
    float OOP(std::string op, float v1, float v2);
    int COP(const std::string& op, int v1, int v2);
    int COP(const std::string& op, float v1, float v2);
    int BOP(const std::string& op, int v1, int v2);
    int BOP(const std::string& op, float v1, float v2);
    void PassValueToFuncArg(Symbol * sym, ASTNode * node);
    void SaveArgs(std::list<Symbol *> * local_vars);
    void RestoreArgs(std::list<Symbol *> * local_vars);
    void CpySymbolValue(Symbol * src, Symbol * des);
    std::list<Symbol *> * GetArgsByScope(const std::string& scope);

public:
    void ExecuteTree(Symbol * main_entry);

private:
    ASTNode * root_;
    std::ofstream tree_out_;
    TypeSystem * typeSystem_;
    std::ofstream interpreter_;
    std::stack<std::string> runtime_scope_; // 运行时函数域
    std::stack<std::pair<ASTNode *, bool>> func_call_stack;  // 函数调用堆栈
    std::stack<Symbol *> func_args_stack; // 参数环境保护堆栈
    std::list<int> func_ret_val;   // 函数返回值队列
};

#endif //COMPILER_AST_H
