/**
  ******************************************************************************
  * @file           : SymbolEntry.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/6
  ******************************************************************************
  */

#ifndef COMPILER_SYMBOLENTRY_H
#define COMPILER_SYMBOLENTRY_H

#include "TypeSystem.h"

class SymbolEntry
{
public:
    explicit SymbolEntry(Symbol * symbol);
    void SetPrev(SymbolEntry * prev);
    void SetNext(SymbolEntry * next);
    SymbolEntry * GetPrev();
    SymbolEntry * GetNext();

private:
    Symbol * symbol_;
    SymbolEntry * prev_{}, * next_{};
};


#endif //COMPILER_SYMBOLENTRY_H
