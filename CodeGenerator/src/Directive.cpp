/**
  ******************************************************************************
  * @file           : Directive.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/9
  ******************************************************************************
  */


#include "../include/Directive.h"

Directive Directive::CLASS_PUBLIC(".class public");
Directive Directive::END_CLASS(".end class");
Directive Directive::SUPER(".super");
Directive Directive::FIELD_PRIVATE_STATIC(".field private static");
Directive Directive::METHOD_STATIC(".method static");
Directive Directive::METHOD_PUBLIC(".method public");
Directive Directive::METHOD_PUBLIC_STATIC(".method public static");
Directive Directive::END_METHOD(".end method");
Directive Directive::LIMIT_LOCALS(".limit locals");
Directive Directive::LIMIT_STACK(".limit stack");
Directive Directive::VAR(".var");
Directive Directive::LINE(".line");

Directive::Directive(std::string text)
    : text_(std::move(text))
{

}

std::string Directive::ToString()
{
    return text_;
}
