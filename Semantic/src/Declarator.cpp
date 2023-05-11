/**
  ******************************************************************************
  * @file           : Declarator.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/6
  ******************************************************************************
  */

#include "include/Declarator.h"

int Declarator::POINTER = 0;
int Declarator::ARRAY = 1;
int Declarator::FUNCTION = 2;

Declarator::Declarator(int type)
    : declare_type_(POINTER),
    number_of_elements_(0)
{
    if (type < POINTER)
    {
        declare_type_ = POINTER;
    }

    if (type > FUNCTION)
    {
        declare_type_ = FUNCTION;
    }
}

void Declarator::SetElementNum(int num)
{
    if (num < 0)
    {
        number_of_elements_ = 0;
    }
    else
    {
        number_of_elements_ = num;
    }
}

int Declarator::GetType() const
{
    return number_of_elements_;
}
