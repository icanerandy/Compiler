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
    std::string rname_; // ��Ӧ�Ĵ�������

    int level_{};   // �����Ĳ��
    bool implicit_{};  // �Ƿ���������
    bool duplicate_{}; // �Ƿ���ͬ������

    int i_value_;
    float f_value_;
    char c_value_;
    std::string s_value_;

    Symbol * args_{};    // ����÷��Ŷ�Ӧ���Ǻ���������ôargsָ������������������б�
    std::string symbol_scope_;
    void * body_;    // �������Ӧ�ĳ������ڵ㣬��void��ֹͷ�ļ���������
    Symbol * next_{};    // ָ����һ��ͬ��εı�������

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
