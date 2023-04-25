/**
  ******************************************************************************
  * @file           : LR0Parser.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/24
  ******************************************************************************
  */

#include "include/LR0Parser.h"

LR0Parser::LR0Parser(Grammar grammar)
        :
        start_symbol_(grammar.get_start_symbol()),
        grammar_(grammar.get_grammar())
{

}

LR0Parser::~LR0Parser()
= default;

/*!
 * LR(0)分析算法
 * stack = []
 * push ($) // $: end of file   文件描述符
 * push (1) // 1: initial state
 * while (true)
 *  token t = nextToken()
 *  state s = stack[top]
 *  if (ACTION[s, t]) == "si")
 *      push (t);   push (i)
 *  else if (ACTION[S, T] == "rj")
 *      pop (the right hand off production "j: X -> β")
 *      state s = stack[top]
 *      push (X);   push (GOTO[s, X])
 *  else error (...)
 *
 * @param tokens - 词法分析输出的token流
 * @return
 */
bool LR0Parser::Parse(const std::vector<std::string> &tokens)
{
    return false;
}

/*!
 * LR(0)算法
 *
 * C0 = closure (S' -> · S $)   // the init closure
 * SET = { C0 } // all states
 * Q = enQueue (C0)  // a queue 作为一个工作表查看还有哪些状态没有计算过
 * while (Q is not empty)
 *  C = deQue (Q)
 *  foreach (x ∈ (N ∪ T))   // 对每一个非终结符和终结符x都做讨论，每一个都引出边出去（移动·号）    x引出的边导出到项目集D
 *      D = goto (C, x)     // 项目集C中可能的形式 X -> β1 · a γ1 ， X -> β2 · Y γ2 ，·号后面跟的无非就是一个终结符或非终结符，则边上的符号就是这个终结符或非终结符（a和Y）
 *
 *  if (x ∈ N)
 *      ACTION[C, x] = D
 *  else GOTO[C, x] = D
 *
 *  if (D not ∈ SET)    // 已经计算过这个项目集，则不必再计算这个项目集所导出去的边都有哪些
 *      SET ∪= { D }
 *      enQueue (D)
 *
 */
void LR0Parser::build_parse_table()
{

}

/*!
 * GOTO表算法
 *
 * goto (C, x)
 *  temp = {}   // a set
 *  foreach (C's item i : A -> β · x γ)
 *      temp ∪= { A -> β x · γ }
 *  return closure (temp)
 */
void LR0Parser::Goto()
{

}

/*!
 * CLOSURE算法：加入了所有·号后面出现非终结符的情况
 *      X -> β a · B γ' --- 如果它期待B
 *      B -> · γ''      --- 那它也一定期待着γ''
 *
 * closure (C)
 *  while (C is still changing)
 *      foreach (C's item i : A -> β · B γ)
 *          C ∪= { B - > ... }
 */
void LR0Parser::Closure()
{

}
