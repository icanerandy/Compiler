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

FA::FA(std::string regex)
    :
    regex_(std::move(regex))
{
    // ����������ʽ
    CheckLegal();
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
    for (char check : regex_)
    {
        if (std::isalpha(check))//Сд�ʹ�д֮����5���ַ����ʲ��������ж�
        {
            //cout<<"��ĸ �Ϸ�";
        }
        else if (check == '(' || check == ')' || check == '*' || check == '|')
        {
            //cout<<"������ �Ϸ�";
        }
        else
        {
            std::cout << "���в��Ϸ����ַ�!" << std::endl;
            std::cout << "����������:" << std::endl;
            return false;
        }
    }
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
    size_t new_regex_length = 0;

    char* new_regex = new char[2 * length + 2]; // ���������
    char first, second;

    for (size_t i = 0; i < length - 1; i++)
    {
        first = regex_.at(i);
        second = regex_.at(i + 1);
        new_regex[new_regex_length++] = first;
        // Ҫ�ӵĿ�������ab �� *b �� a( �� )b �����
        // ��һ������'('��'|'���ڶ�����'('����ĸ
        if (first != '(' && first != '|')
            if (std::isalpha(second) || second == '(')
                new_regex[new_regex_length++] = '+';
    }

    // �����һ���ַ�д��
    new_regex[new_regex_length++] = second;
    new_regex[new_regex_length] = '\0';
    std::string STRING(new_regex);
    regex_ = STRING;
    std::cout << "��'+'��ı��ʽ��" << regex_ << std::endl;
}

/*!
 * ��׺���ʽת��׺���ʽ
 */
void FA::PostFix()
{
    std::stack<char> op;
    std::string suffix;
    for (const auto &c: regex_)
    {
        if (IsOperator(c))
        {//�������
            if (op.empty())//ջ�գ�ֱ����ջ
                op.emplace(c);
            else
            {//���ȼ�����������ȫ����ջ
                while (!op.empty())
                {
                    int t = op.top();
                    if (GetPriority(c) <= GetPriority(t))
                    {
                        op.pop();
                        suffix.push_back(t);
                    }
                    else
                        break;
                }
                op.emplace(c);
            }
        }
        else
        {
            if (c == '(')//������ֱ����ջ
                op.emplace(c);
            else if (c == ')')
            {//���������ţ�һֱ��ջ��ֱ������������
                while (op.top() != '(')
                {
                    suffix.push_back(op.top());
                    op.pop();
                }
                op.pop();
            }
            else
                suffix.push_back(c);//������ֱ�ӷ�����ʽ��
        }
    }
    while (!op.empty())
    {//ȡ��ʣ��������
        suffix.push_back(op.top());
        op.pop();
    }

    regex_ = suffix;
    std::cout << "��׺���ʽ��" << regex_ << std::endl;
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

int FA::GetPriority(char ch)
{
    int level = 0; // ���ȼ�
    switch (ch)
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
        default:
            break;
    }
    return level;
}

bool FA::Judge(const std::string& str)
{
    return dfa_.Judge(str);
}
