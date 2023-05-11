/**
  ******************************************************************************
  * @file           : Directive.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/9
  ******************************************************************************
  */


#ifndef COMPILER_DIRECTIVE_H
#define COMPILER_DIRECTIVE_H

#include <string>

class Directive
{
public:
    static Directive CLASS_PUBLIC;
    static Directive END_CLASS;
    static Directive SUPER;
    static Directive FIELD_PRIVATE_STATIC;
    static Directive METHOD_STATIC;
    static Directive METHOD_PUBLIC;
    static Directive METHOD_PUBLIC_STATIC;
    static Directive END_METHOD;
    static Directive LIMIT_LOCALS;
    static Directive LIMIT_STACK;
    static Directive VAR;
    static Directive LINE;

public:
    explicit Directive(std::string text);
    std::string ToString();

private:
    std::string text_;
};


#endif //COMPILER_DIRECTIVE_H
