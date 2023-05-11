/**
  ******************************************************************************
  * @file           : TypeSystem.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/6
  ******************************************************************************
  */

#include <utility>

#include "include/TypeSystem.h"


TypeSystem::TypeSystem()
{
    symbol_table_.clear();
}

void TypeSystem::AddSymbolsToTable(Symbol *head_symbol, std::string symbol_scope)
{
    while (head_symbol)
    {
        std::list<Symbol *> sym_list;
        if (symbol_table_.find(head_symbol->name_) == symbol_table_.end())
        {
            head_symbol->SetScope(symbol_scope);
            sym_list.emplace_front(head_symbol);
            symbol_table_[head_symbol->name_] = sym_list;
        }
        else
        {
            head_symbol->SetScope(symbol_scope);
            sym_list = symbol_table_.at(head_symbol->name_);
            HandleDuplicateSymbol(head_symbol, sym_list);
        }

        head_symbol = head_symbol->GetNextSymbol();
    }
}

void TypeSystem::HandleDuplicateSymbol(Symbol *symbol, std::list<Symbol *> sym_list)
{
    bool harmless = true;

    for (auto it = sym_list.begin(); it != sym_list.end(); ++it)
    {
        Symbol * sym = *it;
        if (sym->Equals(symbol))
        {
            // TODO: 处理多重声明
        }
    }

    if (harmless)
    {
        sym_list.emplace_front(symbol);
        symbol_table_[symbol->name_] = sym_list;
    }
}

TypeLink *TypeSystem::NewType(std::string type_text)
{
    Specifier * sp = nullptr;
    int type = Specifier::CHAR;
    bool is_long = false, is_singed = true;
    switch (type_text.at(0))
    {
        case 'c':
            if (type_text.at(1) == 'h')
                type = Specifier::CHAR;
            break;
        case 'd':
        case 'f':
            type = Specifier::FLOAT;
            break;
        case 'i':
            type = Specifier::INT;
            break;
        case 'l':
            is_long = true;
            break;
        case 'u':
            is_singed = false;
            break;
        case 'v':
            if (type_text.at(2) == 'i')
                type = Specifier::VOID;
            break;
        case 's':
            // ignore short signed
            break;
    }


    sp = new Specifier();
    sp->SetType(type);
    sp->SetLong(is_long);
    sp->SetSign(is_singed);

    auto * link = new TypeLink(false, false, sp);

    return link;
}

void TypeSystem::SpecifierCpy(Specifier *dst, Specifier *org)
{
    dst->SetConstantVal(org->GetConstantVal());
    dst->SetExternal(org->IsExternal());
    dst->SetLong(org->IsLong());
    dst->SetOutputClass(org->GetOutputClass());
    dst->SetSign(org->IsStatic());
    dst->SetStatic(org->IsStatic());
    dst->SetStorageClass(org->GetStorageClass());
}

TypeLink TypeSystem::NewClass(std::string class_text)
{
    auto * sp = new Specifier();
    /*sp->SetType(Specifier::NONE);*/
    SetClassType(sp, class_text.at(0));
}

void TypeSystem::SetClassType(Specifier *sp, char c)
{
    switch (c)
    {
        case 0:
            sp->SetStorageClass(Specifier::FIXED);
            sp->SetStatic(false);
            sp->SetExternal(false);
            break;
        case 't':
            sp->SetStorageClass(Specifier::TYPEDEF);
            break;
        case 'r':
            sp->SetStorageClass(Specifier::REGISTER);
            break;
        case 's':
            sp->SetStatic(true);
            break;
        case 'e':
            sp->SetExternal(true);
            break;

        default:
            std::cerr << "Internal error, Invalid Class type";
            exit(-1);
            break;
    }
}

Symbol *TypeSystem::NewSymbol(std::string name, std::string symbol_scope)
{
    return new Symbol(std::move(name), symbol_scope);
}

void TypeSystem::AddSpecifierToDeclaration(TypeLink *specifier, Symbol *symbol)
{
    while (symbol)
    {
        symbol->AddSpecifier(specifier);
        symbol = symbol->GetNextSymbol();
    }
}

void TypeSystem::AddDeclarator(Symbol *symbol, int declaration_type)
{
    auto * declarator = new Declarator(declaration_type);
    auto * link = new TypeLink(true, false, declarator);
    symbol->AddDeclarator(link);
}

Symbol *TypeSystem::GetSymbolByText(const std::string& text)
{
    if (text == "printf")
        return nullptr;
    // TODO: 从符号表中获取的符号一定是当前函数的变量声明吗，会不会取到其它符号？ 从头取，从头插
    if (symbol_table_.find(text) == symbol_table_.end())
        return nullptr;
    return symbol_table_.at(text).front();  // 从头取
}

