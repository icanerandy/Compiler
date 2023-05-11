/**
  ******************************************************************************
  * @file           : TypeSystem.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/6
  ******************************************************************************
  */


#ifndef COMPILER_TYPESYSTEM_H
#define COMPILER_TYPESYSTEM_H

#include "Symbol.h"

#include <iostream>
#include <list>
#include <unordered_map>
#include <string>

class TypeSystem
{
public:
    std::unordered_map<std::string, std::list<Symbol *>> symbol_table_{};

public:
    TypeSystem();
    void AddSymbolsToTable(Symbol * head_symbol, std::string symbol_scope);
    void HandleDuplicateSymbol(Symbol * symbol, std::list<Symbol *> sym_list);
    TypeLink * NewType(std::string type_text);
    void SpecifierCpy(Specifier * dst, Specifier * org);
    TypeLink NewClass(std::string class_text);
    void SetClassType(Specifier * sp, char c);
    Symbol* NewSymbol(std::string name, std::string symbol_scope);
    void AddDeclarator(Symbol * symbol, int declaration_type);
    void AddSpecifierToDeclaration(TypeLink* specifier, Symbol* symbol);
    Symbol * GetSymbolByText(const std::string& text);
};


#endif //COMPILER_TYPESYSTEM_H
