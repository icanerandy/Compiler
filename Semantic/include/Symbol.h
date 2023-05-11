/**
  ******************************************************************************
  * @file           : Symbol.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/6
  ******************************************************************************
  */


#ifndef COMPILER_SYMBOL_H
#define COMPILER_SYMBOL_H

#include "TypeLink.h"

#include <string>

class Symbol
{
public:
    std::string name_;  // key
    std::string rname_; // 对应寄存器名称

    int level_{};   // 变量的层次
    bool implicit_{};  // 是否匿名变量
    bool duplicate_{}; // 是否是同名变量

    int i_value_;
    float f_value_;
    char c_value_;
    std::string s_value_;

    Symbol * args_{};    // 如果该符号对应的是函数名，那么args指向函数的输入参数符号列表
    std::string symbol_scope_;
    void * body_;    // 函数体对应的抽象树节点，用void防止头文件包含错误
    Symbol * next_{};    // 指向下一个同层次的变量符号

    TypeLink * type_link_begin_{};
    TypeLink * type_link_end_{};

public:
    Symbol();
    Symbol(std::string name, std::string symbol_scope);
    ~Symbol();
    void AddDeclarator(TypeLink* type);
    void AddSpecifier(TypeLink* type);
    void SetNextSymbol(Symbol* symbol);
    Symbol* GetNextSymbol();
    void SetValue(int i);
    void SetValue(float f);
    void SetValue(char c);
    void SetValue(std::string s);
    std::string GetType();
    void SetScope(const std::string& scope);
    bool Equals(Symbol * symbol);
    void SetFuncBody(void * body);
};


#endif //COMPILER_SYMBOL_H
