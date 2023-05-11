/**
  ******************************************************************************
  * @file           : Symbol.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/6
  ******************************************************************************
  */

#include <utility>

#include "include/Symbol.h"


Symbol::Symbol()
{
    name_ = "";  // key
    rname_ = ""; // 对应寄存器名称

    i_value_ = 0;
    f_value_ = 0;
    c_value_ = 0;
    s_value_ = "";

    args_ = nullptr;    // 如果该符号对应的是函数名，那么args指向函数的输入参数符号列表
    symbol_scope_ = "other";
    body_ = nullptr;    // 函数体对应的抽象树节点
    next_ = nullptr;    // 指向下一个同层次的变量符号

    type_link_begin_ = nullptr;
    type_link_end_ = nullptr;
}

Symbol::Symbol(std::string name, std::string symbol_scope)
    : name_(std::move(name)),
    level_(0)
{
    rname_ = ""; // 对应寄存器名称

    i_value_ = 0;
    f_value_ = 0;
    c_value_ = 0;
    s_value_ = "";

    args_ = nullptr;    // 如果该符号对应的是函数名，那么args指向函数的输入参数符号列表
    symbol_scope_.assign(symbol_scope);
    body_ = nullptr;    // 函数体对应的抽象树节点
    next_ = nullptr;    // 指向下一个同层次的变量符号

    type_link_begin_ = nullptr;
    type_link_end_ = nullptr;
}

void Symbol::AddDeclarator(TypeLink *type)
{
    if (type_link_begin_ == nullptr)
    {
        type_link_begin_ = type;
        type_link_end_ = type;
    }
    else
    {
        type->SetNextLink(type_link_begin_);
        type_link_begin_ = type;
    }
}

void Symbol::AddSpecifier(TypeLink *type)
{
    if (type_link_begin_ == nullptr)
    {
        type_link_begin_ = type;
        type_link_end_ = type;
    }
    else
    {
        type_link_end_->SetNextLink(type);
        type_link_end_ = type;
    }
}

void Symbol::SetNextSymbol(Symbol *symbol)
{
    next_ = symbol;
}

Symbol* Symbol::GetNextSymbol()
{
    return next_;
}

void Symbol::SetValue(int i)
{
    i_value_ = i;
}

void Symbol::SetValue(float f)
{
    f_value_ = f;
}

void Symbol::SetValue(char c)
{
    c_value_ = c;
}

void Symbol::SetValue(std::string s)
{
    s_value_ = std::move(s);
}

std::string Symbol::GetType()
{
    Specifier * specifier = type_link_end_->specifier_;
    int type = specifier->GetType();

    switch (type)
    {
        case 0:
            return "int";
        case 1:
            return "char";
        case 2:
            return "float";
        case 3:
            return "void";
        default:
            return "error type";
    }
}

void Symbol::SetFuncBody(void * body)
{
    body_ = body;
}

void Symbol::SetScope(const std::string& scope)
{
    symbol_scope_.assign(scope);
}

bool Symbol::Equals(Symbol *symbol)
{
    if (name_ == symbol->name_ && level_ == symbol->level_ && symbol_scope_ == symbol->symbol_scope_)
        return true;
    return false;
}

Symbol::~Symbol()
= default;
