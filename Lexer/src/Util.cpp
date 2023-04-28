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

/*
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
 * 9            +               operator
 * 10           -               operator
 * 11           *               operator
 * 12           /               operator
 * 13           <               operator
 * 14           >               operator
 * 15           =               operator
 * 16           <=              operator
 * 17           >=              operator
 * 18           ==              operator
 * 19           !=              operator
 * 20           (               operator
 * 21           )               operator
 * 22           [               array
 * 23           ]               array
 * 24           ;               line_end
 * 25           ,               dot
 * 26           {               struct
 * 27           }               struct
 * 28           ID              id
 * 29           NUM             num
 */

// COP 比较操作符:		<	<=	>	>=	==	!=
// AOP 赋值操作符:		=
// OOP 运算操作符:		+	-	*	/
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