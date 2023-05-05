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
 * 1            return          keyword
 * 2            void            keyword
 * 3            const           keyword
 * 4            int             keyword
 * 5            float           keyword
 * 6            char            keyword
 * 7            String          keyword
 * 8            if              keyword
 * 9            else            keyword
 * 10           do              keyword
 * 11           while           keyword
 * 12           for             keyword
 * 13           break           keyword
 * 14           continue        keyword
 *
 * 100          + (ADD)         operator
 * 101          - (MINUS)       operator
 * 102          * (MUL)         operator
 * 103          / (DIV)         operator
 * 104          % (MOD)         operator

 * 105          &               operator
 * 106          |               operator
 *
 * 107          && (AND)        operator
 * 108          || (OR)         operator
 * 109          ! (NOT)         operator
 *
 * 110          < (LT)          operator
 * 111          > (GT)          operator
 * 112          = (ASSIGN)      operator
 * 113          <= (LE)         operator
 * 114          >= (GE)         operator
 * 115          == (EQ)         operator
 * 116          != (NEQ)        operator
 *
 * 200          ( (LP)
 * 201          ) (RP)
 * 202          [ (ARRAY)
 * 203          ] (ARRAY)
 * 204          ; (SEMICOLON)
 * 205          , (COMMA)
 * 206          { (LC)
 * 207          } (RC)
 *
 * 300          id              identifier
 * 400          num             const   数字常量
 * 500          cconst          const   字符常量
 * 600          sconst          const   字符串常量
 */

// RESERVED 保留字:	    return void const int float char String if else do while for break continue
// BOP 布尔操作符:        &&  ||
// COP 比较操作符:		<	<=  >	>=	==	!=
// AOP 赋值操作符:		=   +=  -=  *=  /=  %=
// OOP 运算操作符:		+	-	*	/   %     &   |
// EOP 句末操作符:		;
// SOP 结构分隔符:		(	)	,	[	]	{	}

std::unordered_map<std::string, int>
        Util::keyword_list_ = {
        {"return", 0},
        {"void", 1},
        {"const", 2},
        {"int", 3},
        {"float", 4},
        {"char", 5},
        {"String", 6},
        {"if", 7},
        {"else", 8},
        {"do", 9},
        {"while", 10},
        {"for", 11},
        {"break", 12},
        {"continue", 13}
};

std::unordered_map<std::string, int>
        Util::BOP = {
        {"&&", 10},
        {"||", 11}
};

std::unordered_map<std::string, int>
        Util::COP = {
        {"<", 12},
        {"<=", 13},
        {">", 14},
        {">=", 15},
        {"==", 16},
        {"!=", 17}
};

std::unordered_map<std::string, int>
        Util::AOP = {
        {"=", 18},
        {"+=", 19},
        {"-=", 20},
        {"*=", 21},
        {"/=", 22},
        {"%=", 23}
};

std::unordered_map<std::string, int>
        Util::OOP = {
        {"+", 24},
        {"-", 25},
        {"*", 26},
        {"/", 27},
        {"%", 28},
        {"&", 29},
        {"|", 30}
};

std::unordered_map<std::string, int>
        Util::EOP = {
        {";", 31}
};

std::unordered_map<std::string, int>
        Util::SOP = {
        {"(", 32},
        {")", 33},
        {",", 34},
        {"[", 35},
        {"]", 36},
        {"{", 37},
        {"}", 38}
};