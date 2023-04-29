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
 * 3            int             keyword
 * 4            float           keyword
 * 5            char            keyword
 * 6            String          keyword
 * 7            if              keyword
 * 8            else            keyword
 * 9            while           keyword
 *
 * 10            + (ADD)         operator
 * 11           - (MINUS)       operator
 * 12           * (MUL)         operator
 * 13           / (DIV)         operator
 * 14           % (MOD)         operator

 * 20           &               operator
 * 21           |               operator
 *
 * 22           && (AND)        operator
 * 23           || (OR)         operator
 * 24           ! (NOT)         operator
 *
 * 25           < (LT)          operator
 * 26           > (GT)          operator
 * 27           = (ASSIGN)      operator
 * 28           <= (LE)         operator
 * 29           >= (GE)         operator
 * 30           == (EQ)         operator
 * 31           != (NEQ)        operator
 *
 * 32           ( (LP)
 * 33           ) (RP)
 * 34           [ (ARRAY)
 * 35           ] (ARRAY)
 * 36           ; (SEMICOLON)
 * 37           , (COMMA)
 * 38           { (LC)
 * 39           } (RC)
 *
 * 40           id              ID
 * 41           num             CONST   数字常量
 * 42           cconst          CONST   字符常量
 * 43           sconst          CONST   字符串常量
 */

// RESERVED 保留字:	    return void int float char String if else while
// BOP 布尔操作符:        &&  ||
// COP 比较操作符:		<	<=  >	>=	==	!=
// AOP 赋值操作符:		=   +=  -=  *=  /=  %=
// OOP 运算操作符:		+	-	*	/   %     &   |
// EOP 句末操作符:		;
// SOP 结构分隔符:		(	)	,	[	]	{	}

std::unordered_map<std::string, int>
        Util::keyword_list_ = {
        {"return", 1},
        {"void", 2},
        {"int", 3},
        {"float", 4},
        {"char", 5},
        {"String", 6},
        {"if", 7},
        {"else", 8},
        {"while", 9}
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