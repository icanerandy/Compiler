/**
  ******************************************************************************
  * @file           : Specifier.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/6
  ******************************************************************************
  */

#include "include/Specifier.h"

int Specifier::INT = 0;
int Specifier::CHAR = 1;
int Specifier::FLOAT = 2;
int Specifier::VOID = 3;
int Specifier::LABEL = 4;

int Specifier::FIXED = 0;
int Specifier::REGISTER = 1;
int Specifier::AUTO = 2;
int Specifier::TYPEDEF = 3;
int Specifier::CONSTANT = 4;

int Specifier::NO_OCLASS = 0;
int Specifier::PUBLIC = 1;
int Specifier::PRIVATE = 2;
int Specifier::EXTERN = 3;
int Specifier::COMMON = 4;


Specifier::Specifier()
    : is_long_(false),
    is_signed_(false),
    constant_value_(0)
{

}

void Specifier::SetType(int type)
{
    basic_type_ = type;
}

int Specifier::GetType() const
{
    return basic_type_;
}

void Specifier::SetStorageClass(int s)
{
    storage_class_ = s;
}

int Specifier::GetStorageClass() const
{
    return storage_class_;
}

void Specifier::SetOutputClass(int c)
{
    output_class_ = c;
}

int Specifier::GetOutputClass() const
{
    return output_class_;
}

void Specifier::SetLong(bool l)
{
    is_long_ = l;
}

bool Specifier::IsLong() const
{
    return is_long_;
}

void Specifier::SetSign(bool sign)
{
    is_signed_ = sign;
}

bool Specifier::IsSigned() const
{
    return is_signed_;
}

void Specifier::SetStatic(bool s)
{
    is_static_ = s;
}

bool Specifier::IsStatic() const
{
    return is_static_;
}

void Specifier::SetExternal(bool e)
{
    is_external_ = e;
}

bool Specifier::IsExternal() const
{
    return is_external_;
}

void Specifier::SetConstantVal(int v)
{
    constant_value_ = v;
}

int Specifier::GetConstantVal() const
{
    return constant_value_;
}
