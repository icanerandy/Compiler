/**
  ******************************************************************************
  * @file           : Util.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/28
  ******************************************************************************
  */

#include "include/Util.h"

/* 四类：关键字、操作符、特别符、标识符
 * 种别码syn   单词符号symbol  类型type
 * 0            #               end
 * 1            void            keyword
 * 2            int             keyword
 * 3            float           keyword
 * 4            char            keyword
 * 5            String          keyword
 * 6            if              keyword
 * 7            else            keyword
 * 8            while           keyword
 *
 * 9            + (ADD)         operator
 * 10           - (MINUS)       operator
 * 11           * (MUL)         operator
 * 12           / (DIV)         operator
 * 13           % (MOD)         operator

 * 19           &               operator
 * 20           |               operator
 *
 * 21           && (AND)        operator
 * 22           || (OR)         operator
 * 23           ! (NOT)         operator
 *
 * 24           < (LT)          operator
 * 25           > (GT)          operator
 * 26           = (ASSIGN)      operator
 * 27           <= (LE)         operator
 * 28           >= (GE)         operator
 * 29           == (EQ)         operator
 * 30           != (NEQ)        operator
 *
 * 31           ( (LP)
 * 32           ) (RP)
 * 33           [ (ARRAY)
 * 34           ] (ARRAY)
 * 35           ; (SEMICOLON)
 * 36           , (COMMA)
 * 37           { (LC)
 * 38           } (RC)
 *
 * 39           id              ID
 * 40           num             NUM
 */

// BOP 布尔操作符:        &&  ||
// COP 比较操作符:		<	<=  >	>=	==	!=
// AOP 赋值操作符:		=   +=  -=  *=  /=  %=
// OOP 运算操作符:		+	-	*	/   %     &   |
// EOP 句末操作符:		;
// SOP 结构分隔符:		(	)	,	[	]	{	}
// RESERVED 保留字:	    void int float char String if else while

std::unordered_map<std::string, int>
Util::keyword_list_ = {
        {"void", 1},
        {"int", 2},
        {"float", 3},
        {"char", 4},
        {"String", 5},
        {"if", 6},
        {"else", 7},
        {"while", 8}
};