/**
  ******************************************************************************
  * @file           : Semantic.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/5
  ******************************************************************************
  */

//
// Created by icaner on 2023/5/5.
//

#ifndef COMPILER_SEMANTIC_H
#define COMPILER_SEMANTIC_H

#include "include/AST.h"

class Semantic
{
public:
    explicit Semantic(ASTTree *);

private:
    ASTTree * tree_;
};


#endif //COMPILER_SEMANTIC_H
