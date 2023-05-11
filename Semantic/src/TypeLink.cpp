/**
  ******************************************************************************
  * @file           : TypeLink.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/6
  ******************************************************************************
  */
//
// Created by icaner on 2023/5/6.
//

#include "include/TypeLink.h"

TypeLink::TypeLink(bool is_declarator, bool is_typedef, Specifier *type_object)
        : is_declarator_(is_declarator),
          is_typedef_(is_typedef),
          specifier_(type_object)
{

}

TypeLink::TypeLink(bool is_declarator, bool is_typedef, Declarator *type_object)
        : is_declarator_(is_declarator),
          is_typedef_(is_typedef),
          declarator_(type_object)
{

}

void TypeLink::SetNextLink(TypeLink *link)
{
    next_ = link;
}

TypeLink *TypeLink::ToNext() const
{
    return next_;
}

Specifier *TypeLink::GetTypeObject()
{
    return nullptr;
}

