/**
  ******************************************************************************
  * @file           : FA.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/26
  ******************************************************************************
  */

#include "../include/FA.h"

#include <utility>
#include <map>

FA::FA()
= default;

FA::FA(std::string regex)
{
    for (size_t i = 0; i < regex.size(); ++i)
    {
        if (regex.at(i) == '\\')
        {
            char ch = regex.at(i+1);
            switch (ch) {
                case '\\': regex_.emplace_back('\\', false); break;
                case 's': regex_.emplace_back(' ', false); break;
                case 'n': regex_.emplace_back('\n', false); break;
                case 't': regex_.emplace_back('\t', false); break;
                case 'r': regex_.emplace_back('\r', false); break;
                case '|': regex_.emplace_back('|', false); break;
                case '*': regex_.emplace_back('*', false); break;
                case '+': regex_.emplace_back('+', false); break;
                case '(': regex_.emplace_back('(', false); break;
                case ')': regex_.emplace_back(')', false); break;
                    // ����ת���ַ�Ҳ���������ﴦ����\'��\"��\r��
                default:
                    // ���������Ч��ת���ַ�����ԭ�ַ�����
                    std::cerr << "������ʽ���󣺷���Чת���ַ���" << " \\" << ch << std::endl;
                    exit(-1);
            }
            i++;
        }
        else if (regex.at(i) == '|' || regex.at(i) == '*' || regex.at(i) == '+' || regex.at(i) == '(' || regex.at(i) == ')')
        {
            regex_.emplace_back(regex.at(i), true);
        }
        else
        {
            regex_.emplace_back(regex.at(i), false);
        }
    }

    // ����������ʽ
    // CheckLegal();
    AddJoinSymbol();
    PostFix();

    nfa_ = NFA(regex_);
    dfa_ = DFA(nfa_);
}

/*!
 * ��������������ʽ�Ƿ�Ϸ�
 * @return
 */
bool FA::CheckLegal()
{
    return CheckCharacter() && CheckParenthesis();
}

/*!
 * ���������ַ��Ƿ���� () * | a~z A~Z
 * @return
 */
bool FA::CheckCharacter()
{
//    for (char check : regex_)
//    {
//        if (std::isalpha(check))//Сд�ʹ�д֮����5���ַ����ʲ��������ж�
//        {
//            //cout<<"��ĸ �Ϸ�";
//        }
//        else if (check == '(' || check == ')' || check == '*' || check == '|')
//        {
//            //cout<<"������ �Ϸ�";
//        }
//        else
//        {
//            std::cout << "���в��Ϸ����ַ�!" << std::endl;
//            std::cout << "����������:" << std::endl;
//            return false;
//        }
//    }
    return true;
}

/*!
 * �ȼ�������Ƿ�ƥ��
 * @return
 */
bool FA::CheckParenthesis()
{
    //char * check = new char[length+1];
    //wcscpy(check, length+1, check_string.c_str());
    std::vector<RegexSymbol> check = regex_;
    std::stack<int> STACK;
    for (size_t i = 0; i < regex_.size(); i++)
    {
        if (check[i].first == '(' && check[i].second)
            STACK.push(i);
        else if (check[i].first == ')' && check[i].second)
        {
            if (STACK.empty())
            {
                std::cerr << "�ж����������" << std::endl;//��ʱ����¼ƥ��λ��location
                std::cout << "����������:" << std::endl;
                return false;
            }
            else
                STACK.pop();
        }
    }
    if (!STACK.empty())
    {
        //��ʱ����¼ƥ��λ��location
        std::cerr << "�ж����������" << std::endl;
        std::cout << "����������:" << std::endl;
        return false;
    }

    return true;
}

/*!
 * ��ӽ���������+����������׺ת��׺���ʽ
 */
void FA::AddJoinSymbol()
{
/*  ������ֹ��\0
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

    std::vector<RegexSymbol> new_regex; // ���������
    RegexSymbol first, second;

    for (size_t i = 0; i < length - 1; i++)
    {
        first = regex_.at(i);
        second = regex_.at(i + 1);
        new_regex.emplace_back(first);
        // Ҫ�ӵĿ�������ab �� *b �� a( �� )b �����
        // ��һ������'('��'|'���ڶ�����'('����ĸ
        if (!( (first.first == '(' || first.first == '|') && first.second ))
            if ( (second.first == '(' && second.second) || !second.second )
                new_regex.emplace_back('+', true);
    }

    // �����һ���ַ�д��
    new_regex.emplace_back(regex_.at(length - 1));
    regex_ = new_regex;
    // std::cout << "��'+'��ı��ʽ��" << regex_ << std::endl;
}

/*!
 * ��׺���ʽת��׺���ʽ
 */
void FA::PostFix()
{
    std::stack<RegexSymbol> op;
    std::vector<RegexSymbol> suffix;
    for (auto symbol : regex_)
    {
        if (symbol.second && symbol.first != '(' && symbol.first != ')')
        {//�������
            if (op.empty())//ջ�գ�ֱ����ջ
                op.emplace(symbol);
            else
            {//���ȼ�����������ȫ����ջ
                while (!op.empty())
                {
                    RegexSymbol t = op.top();
                    if (GetPriority(symbol) <= GetPriority(t))
                    {
                        op.pop();
                        suffix.push_back(t);
                    }
                    else
                        break;
                }
                op.emplace(symbol);
            }
        }
        else
        {
            if (symbol.first == '(' && symbol.second)//������ֱ����ջ
                op.emplace(symbol);
            else if (symbol.first == ')' && symbol.second)
            {//���������ţ�һֱ��ջ��ֱ������������
                while (! (op.top().first == '(' && op.top().second))
                {
                    suffix.push_back(op.top());
                    op.pop();
                }
                op.pop();
            }
            else
                suffix.push_back(symbol);//������ֱ�ӷ�����ʽ��
        }
    }
    while (!op.empty())
    {//ȡ��ʣ��������
        suffix.push_back(op.top());
        op.pop();
    }

    regex_ = suffix;
    // std::cout << "��׺���ʽ��" << regex_ << std::endl;
}

bool FA::IsOperator(char c)
{//�ж��ǲ��������
    switch (c)
    {
        case '*':
        case '|':
        case '+':
            return true;
        default:
            return false;
    }
}

int FA::GetPriority(RegexSymbol symbol)
{
    int level = 0; // ���ȼ�

    if (symbol.second)
    {
        switch (symbol.first)
        {
            case '(':
                level = 1;
                break;
            case '|':
                level = 2;
                break;
            case '+':
                level = 3;
                break;
            case '*':
                level = 4;
                break;
        }
    }

    return level;
}

DFA FA::GetDFA()
{
    return dfa_;
}
