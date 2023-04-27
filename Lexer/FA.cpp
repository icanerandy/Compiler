/**
  ******************************************************************************
  * @file           : FA.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/26
  ******************************************************************************
  */

#include "FA.h"

#include <utility>
#include <map>

FA::FA(std::string regex)
    :
    regex_(std::move(regex))
{
    // 处理正则表达式
    CheckLegal();
    AddJoinSymbol();
    PostFix();

    nfa_ = NFA(regex_);
    dfa_ = DFA(nfa_);
}

/*!
 * 检测输入的正则表达式是否合法
 * @return
 */
bool FA::CheckLegal()
{
    return CheckCharacter() && CheckParenthesis();
}

/*!
 * 检查输入的字符是否合适 () * | a~z A~Z
 * @return
 */
bool FA::CheckCharacter()
{
    for (char check : regex_)
    {
        if (std::isalpha(check))//小写和大写之间有5个字符，故不能连续判断
        {
            //cout<<"字母 合法";
        }
        else if (check == '(' || check == ')' || check == '*' || check == '|')
        {
            //cout<<"操作符 合法";
        }
        else
        {
            std::cout << "含有不合法的字符!" << std::endl;
            std::cout << "请重新输入:" << std::endl;
            return false;
        }
    }
    return true;
}

/*!
 * 先检查括号是否匹配
 * @return
 */
bool FA::CheckParenthesis()
{
    //char * check = new char[length+1];
    //wcscpy(check, length+1, check_string.c_str());
    std::string check = regex_;
    std::stack<int> STACK;
    for (size_t i = 0; i < regex_.size(); i++)
    {
        if (check[i] == '(')
            STACK.push(i);
        else if (check[i] == ')')
        {
            if (STACK.empty())
            {
                std::cerr << "有多余的右括号" << std::endl;//暂时不记录匹配位置location
                std::cout << "请重新输入:" << std::endl;
                return false;
            }
            else
                STACK.pop();
        }
    }
    if (!STACK.empty())
    {
        //暂时不记录匹配位置location
        std::cerr << "有多余的左括号" << std::endl;
        std::cout << "请重新输入:" << std::endl;
        return false;
    }

    return true;
}

/*!
 * 添加交操作符“+”，便于中缀转后缀表达式
 */
void FA::AddJoinSymbol()
{
/*  测试终止符\0
	string check_string = "abcdefg\0aaa";
	cout<<check_string<<endl;
	int length = check_string.size();
	char * check = new char[2*length];
	strcpy(check,check_string.c_str());
	cout<<check<<endl;
	char *s = "ssss\0  aa";
	cout<<s<<endl;
	string a(s);
	cout<<a<<endl;
	*/
    size_t length = regex_.size();
    size_t new_regex_length = 0;

    char* new_regex = new char[2 * length + 2]; // 最多是两倍
    char first, second;

    for (size_t i = 0; i < length - 1; i++)
    {
        first = regex_.at(i);
        second = regex_.at(i + 1);
        new_regex[new_regex_length++] = first;
        // 要加的可能性如ab 、 *b 、 a( 、 )b 等情况
        // 第一个不是'('、'|'，第二个是'('或字母
        if (first != '(' && first != '|')
            if (std::isalpha(second) || second == '(')
                new_regex[new_regex_length++] = '+';
    }

    // 将最后一个字符写入
    new_regex[new_regex_length++] = second;
    new_regex[new_regex_length] = '\0';
    std::string STRING(new_regex);
    regex_ = STRING;
    std::cout << "加'+'后的表达式：" << regex_ << std::endl;
}

/*!
 * 中缀表达式转后缀表达式
 */
void FA::PostFix()
{
    // 设定regex_的最后一个符号式“#”，而其“#”一开始先放在栈s的栈底
    regex_ += '#';

    std::stack<char> s;
    char ch = '#', ch1, op;
    s.push(ch);

    // 读入一个字符
    std::string new_regex;
    size_t read_location = 0;
    ch = regex_.at(read_location++);
    while (!s.empty())
    {
        if (std::isalpha(ch))
        {
            new_regex += ch;
            ch = regex_.at(read_location++);
        }
        else
        {
            //cout<<"输出操作符："<<ch<<endl;
            ch1 = s.top();
            if (Isp(ch1) < Icp(ch))
            {
                s.push(ch);
                //cout<<"压栈"<<ch<<"  读取下一个"<<endl;
                ch = regex_.at(read_location++);
            }
            else if (Isp(ch1)>Icp(ch))
            {
                op = s.top();
                s.pop();
                //cout<<"退栈"<<op<<" 添加到输出字符串"<<endl;
                new_regex += op;
                //cout<<op;
            }
            else  //考虑优先级相等的情况
            {
                op = s.top();
                s.pop();
                //cout<<"退栈"<<op<<"  但不添加到输入字符串"<<endl;

                if (op == '(')
                    ch = regex_.at(read_location++);
            }
        }
    }

    regex_ = new_regex;
    std::cout << "后缀表达式：" << regex_ << std::endl;
}

/*
构造优先级表规则：（1）先括号内，再括号外；（2）优先级由高到低：闭包、|、+；（3）同级别，先左后右。
优先级表：
	 #	(	*	|	+	)
isp  0	1	7	5	3	8
icp	 0	8	6	4	2	1
*/

/*********************运算符优先级关系表********************/
/*
	c1\c2	(	*	|	+	)	#
	(		<	<	<	<	=	>
	*		<	>	>	>	>	>
	|		<	<	>	>	>	>
	+		<	<	<	>	>	>
	#		<	<	<	<	<	=
*/
/***********************************************************/

/*!
 * in stack priority  栈内优先级，栈顶的字符的优先级
 * @param ch
 * @return
 */
int FA::Isp(char ch)
{
    switch (ch)
    {
        case '#': return 0;
        case '(': return 1;
        case '*': return 7;
        case '|': return 5;
        case '+': return 3;
        case ')': return 8;
        default:
            //若走到这一步，说明出错了
            std::cerr << "ERROR!" << std::endl;
            return false;
    }
}

/*!
 * in coming priority 栈外优先级，当前扫描到的字符的优先级
 * @param ch
 * @return
 */
int FA::Icp(char ch)
{
    switch (ch)
    {
        case '#': return 0;
        case '(': return 8;
        case '*': return 6;
        case '|': return 4;
        case '+': return 2;
        case ')': return 1;
        default:
            //若走到这一步，说明出错了
            std::cerr << "ERROR!" << std::endl;
            return false;
    }
}

