/**
  ******************************************************************************
  * @file           : LR0Parser.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/24
  ******************************************************************************
  */

/**
 * LR(0)分析算法：
 *  从左（L）向右读入程序，最右（L）推导，不用前看符号来决定产生式的选择（0个前看符号） 这里的0不是指输入程序的token，而是指计算的FIRST集
 *  优点：容易实现
 *  缺点：能分析的文法有限
 *
 *  缺点：
 *      * 对每一个形如 X -> α · 的项目
 *          1. 直接把 α 规约成 X，紧跟一个 “goto”
 *          2. 尽管不会漏掉错误，但会延迟错误发现时机
 *              * 练习：尝试 “x x y x”
 *      * LR(0)分析表中可能包含冲突（这里的冲突和LL(1)中的冲突不一样）
 *
 *      问题1：错误定位
 *      问题2：冲突（移进-归约冲突/SR冲突/shift-reduce冲突）：不知道该移进还是移除
 *
 * 生成一个逆序的最右推导
 * 需要两个步骤：
 *  * 移进：一个记号到栈顶上，或者
 *  * 归约（·号到了串的末尾的时候）： 栈顶上的n个符号（某产生式的右部）到左部的非终结符
 *      * 对产生式 A -> β1 ... βn
 *          - 如果 βn ... β1 在栈顶上，则弹出 βn ... β1
 *          - 压入 A
 *
 *
 * SLR分析算法：
 *  和LR(0)分析算法基本步骤相同
 *  仅区别于对归约的处理
 *      * 对于状态i上的项目 X -> α · ， 仅对 y ∈ FOLLOW(X) 添加 ACTION[i, y] ：如果 y not ∈ FOLLOW(X)，则只能移入，减少了冲突
 *
 *  基于LR(0)，通过进一步判断一个前看符号，来决定是否执行归约动作
 *      * X -> α · 归约，当且仅当 y ∈ FOLLOW(X)
 *      优点：
 *          * 有可能减少需要归约的情况，减小LR(0)分析表的规模，更加精确地报错
 *          * 有可能!去除需要移进-归约冲突
 *      缺点：
 *          * 仍然有冲突出现的可能
 *
 * 核心问题：如何确定移进和归约的时机？
 *
 *
 * LR(1)项目：项目变成了二元组，a是某一个或某几个终结符。类似于SLR中 a ∈ FOLLOW(X)
 *  [X -> β · γ， a] 的含义是：
 *      * β 在栈顶上
 *      * 剩余的输入能够匹配 γa
 *  当归约 X -> βγ 式，a是前看符号
 *      * 把 ‘ reduce by X -> βγ ’填入ACTION[s, a]
 *
 *  其他和LR(0)相同，仅闭包的计算不同：
 *      对项目 [X -> β · Y γ, a]
 *          * 添加 [Y -> Γ, b] 到项目集，其中 b ∈ FIRST_S(γa)
 */

#ifndef GRAMMAR_LR0PARSER_H
#define GRAMMAR_LR0PARSER_H

#include "include/Grammar.h"

class LR0Parser
{
public:
    explicit LR0Parser(Grammar grammar);
    ~LR0Parser();
    bool Parse(const std::vector<std::string>& tokens);

private:
    void build_parse_table();

private:
    void Goto();
    void Closure();

private:
    NonTerminal start_symbol_;
    std::unordered_map<NonTerminal, RightPart> grammar_;
};


#endif //GRAMMAR_LR0PARSER_H
