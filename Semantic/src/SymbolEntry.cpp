/**
  ******************************************************************************
  * @file           : SymbolEntry.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/6
  ******************************************************************************
  */

#include "include/SymbolEntry.h"

SymbolEntry::SymbolEntry(Symbol *symbol)
    : symbol_(symbol)
{

}

void SymbolEntry::SetPrev(SymbolEntry *prev)
{
    prev_ = prev;
}

void SymbolEntry::SetNext(SymbolEntry *next)
{
    next_ = next;
}

SymbolEntry *SymbolEntry::GetPrev()
{
    return prev_;
}

SymbolEntry *SymbolEntry::GetNext()
{
    return next_;
}
