/**
  ******************************************************************************
  * @file           : TypeLink.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/6
  ******************************************************************************
  */


#ifndef COMPILER_TYPELINK_H
#define COMPILER_TYPELINK_H

#include "Specifier.h"
#include "Declarator.h"

class TypeLink
{
public:
    bool is_declarator_;    // true 那么该object的对象是declarator，false指向specifier
    bool is_typedef_;  // true 那么当前变量的类型是由typedef定义的

    Specifier * specifier_{};
    Declarator * declarator_{};

    TypeLink * next_{};

public:
    TypeLink(bool is_declarator, bool is_typedef, Specifier * type_object );
    TypeLink(bool is_declarator, bool is_typedef, Declarator * type_object );
    void SetNextLink(TypeLink* link);
    TypeLink * ToNext() const;
    Specifier * GetTypeObject();
};


#endif //COMPILER_TYPELINK_H
