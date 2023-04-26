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

FA::FA(std::string regex)
    :
    regex_(std::move(regex)),
    state_num_{}
{
    CheckLegal();
    AddJoinSymbol();

    PostFix();

    cell_ = ExpressToNFA();
    Display();
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
    // �趨regex_�����һ������ʽ��#�������䡰#��һ��ʼ�ȷ���ջs��ջ��
    regex_ += '#';

    std::stack<char> s;
    char ch = '#', ch1, op;
    s.push(ch);

    // ����һ���ַ�
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
            //cout<<"�����������"<<ch<<endl;
            ch1 = s.top();
            if (Isp(ch1) < Icp(ch))
            {
                s.push(ch);
                //cout<<"ѹջ"<<ch<<"  ��ȡ��һ��"<<endl;
                ch = regex_.at(read_location++);
            }
            else if (Isp(ch1)>Icp(ch))
            {
                op = s.top();
                s.pop();
                //cout<<"��ջ"<<op<<" ��ӵ�����ַ���"<<endl;
                new_regex += op;
                //cout<<op;
            }
            else  //�������ȼ���ȵ����
            {
                op = s.top();
                s.pop();
                //cout<<"��ջ"<<op<<"  ������ӵ������ַ���"<<endl;

                if (op == '(')
                    ch = regex_.at(read_location++);
            }
        }
    }

    regex_ = new_regex;
    std::cout << "��׺���ʽ��" << regex_ << std::endl;
}

/*
�������ȼ�����򣺣�1���������ڣ��������⣻��2�����ȼ��ɸߵ��ͣ��հ���|��+����3��ͬ����������ҡ�
���ȼ���
	 #	(	*	|	+	)
isp  0	1	7	5	3	8
icp	 0	8	6	4	2	1
*/

/*********************��������ȼ���ϵ��********************/
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
 * in stack priority  ջ�����ȼ���ջ�����ַ������ȼ�
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
            //���ߵ���һ����˵��������
            std::cerr << "ERROR!" << std::endl;
            return false;
    }
}

/*!
 * in coming priority ջ�����ȼ�����ǰɨ�赽���ַ������ȼ�
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
            //���ߵ���һ����˵��������
            std::cerr << "ERROR!" << std::endl;
            return false;
    }
}

/*!
 * ����µ�״̬�ڵ㣬ͳһ���������ڹ������ܲ����ظ���״̬
 * ����ӵ�state_set������
 * @return
 */
State FA::NewStateNode()
{
    State new_state{};
    new_state.value = state_num_ + 1;
    state_num_++;

    return new_state;
}

Cell FA::DoCell(char ch)
{
    Cell new_cell;
    new_cell.edge_count = 0;
    Edge new_edge{};

    // ��ȡ��״̬�ڵ�
    State start_state = NewStateNode();
    State end_state = NewStateNode();

    // ������
    new_edge.start_state = start_state;
    new_edge.end_state = end_state;
    new_edge.trans_symbol = ch;

    // ������Ԫ
    new_cell.edge_vector.emplace_back(new_edge);
    new_cell.start_state = start_state;
    new_cell.end_state = end_state;

    return new_cell;
}

void FA::CellEdgeSetCopy(Cell& des, const Cell& src)
{
    des.edge_vector.insert(des.edge_vector.begin(),
                            src.edge_vector.begin(), src.edge_vector.end());
}

Cell FA::DoUnit(const Cell& left, const Cell& right)
{
    Cell new_cell;
    new_cell.edge_count = 0;
    Edge edge1{}, edge2{}, edge3{}, edge4{};

    // ��ȡ��״̬�ڵ�
    State start_state = NewStateNode(); // s
    State end_state = NewStateNode();   // e

    // ������
    edge1.start_state = start_state;
    edge1.end_state = left.start_state;
    edge1.trans_symbol = '#';

    edge2.start_state = start_state;
    edge2.end_state = right.start_state;
    edge2.trans_symbol = '#';

    edge3.start_state = left.end_state;
    edge3.end_state = end_state;
    edge3.trans_symbol = '#';

    edge4.start_state = right.end_state;
    edge4.end_state = end_state;
    edge4.trans_symbol = '#';

    // ������Ԫ
    // �Ƚ�left��right��edge_set���Ƶ�new_cell
    CellEdgeSetCopy(new_cell, left);
    CellEdgeSetCopy(new_cell, right);

    // ���¹����������߼���edge_set
    new_cell.edge_vector.emplace_back(edge1);
    new_cell.edge_vector.emplace_back(edge2);
    new_cell.edge_vector.emplace_back(edge3);
    new_cell.edge_vector.emplace_back(edge4);

    // ����new_cell����ʼ״̬�ͽ���״̬
    new_cell.start_state = start_state;
    new_cell.end_state = end_state;

    return new_cell;
}

Cell FA::DoJoin(Cell &left, Cell &right)
{
    // ��left�Ľ���״̬��right�Ŀ�ʼ״̬�ϲ�����right�ı߸��Ƹ�left����left����
    // ��right��������С��nfa��start_state��ͷ�ı�ȫ���޸�
    for (auto& edge : right.edge_vector)
    {
        if (edge.start_state == right.start_state)
            edge.start_state = left.end_state;
        else if (edge.end_state == right.start_state)
            edge.end_state = left.end_state;
    }
    state_num_--;

    CellEdgeSetCopy(left, right);

    left.end_state = right.end_state;
    return left;
}

Cell FA::DoStar(const Cell& cell)
{
    Cell new_cell;
    new_cell.edge_count = 0;
    Edge edge1{}, edge2{}, edge3{}, edge4{};

    // ��ȡ�µ�״̬�ڵ�
    State start_state = NewStateNode();
    State end_state = NewStateNode();

    // ������
    edge1.start_state = start_state;
    edge1.end_state = end_state;
    edge1.trans_symbol = '#';

    edge2.start_state = cell.end_state;
    edge2.end_state = cell.start_state;
    edge2.trans_symbol = '#';

    edge3.start_state = start_state;
    edge3.end_state = cell.start_state;
    edge3.trans_symbol = '#';

    edge4.start_state = cell.end_state;
    edge4.end_state = end_state;
    edge4.trans_symbol = '#';

    // ������Ԫ
    // �Ƚ�cell��edge_set���Ƶ�new_cell��
    CellEdgeSetCopy(new_cell, cell);
    // ���¹����������߼���edge_set��
    new_cell.edge_vector.emplace_back(edge1);
    new_cell.edge_vector.emplace_back(edge2);
    new_cell.edge_vector.emplace_back(edge3);
    new_cell.edge_vector.emplace_back(edge4);

    // ����new_cell����ʼ״̬����ֹ״̬
    new_cell.start_state = start_state;
    new_cell.end_state = end_state;

    return new_cell;
}

/*!
 * ���ʽתNFA���������������յ�NFA
 * @return
 */
Cell FA::ExpressToNFA()
{
    size_t length = regex_.size();
    char element;
    Cell cell, left, right;

    std::stack<Cell> s;
    for (size_t i = 0; i < length; ++i)
    {
        element = regex_.at(i);
        cell.edge_count = cell.edge_vector.size();
        switch (element)
        {
            case '|':
                right = s.top();
                s.pop();
                left = s.top();
                s.pop();
                cell = DoUnit(left, right);
                s.push(cell);
                break;
            case '*':
                left = s.top();
                s.pop();
                cell = DoStar(left);
                s.push(cell);
                break;
            case '+':
                right = s.top();
                s.pop();
                left = s.top();
                s.pop();
                cell = DoJoin(left, right);
                s.push(cell);
                break;
            default:
                cell = DoCell(element);
                s.push(cell);
        }
    }

    std::cout << "������ϣ�" << std::endl;
    cell = s.top();
    s.pop();

    return cell;
}

void FA::Display()
{
    std::cout << "NFA �ı�����" << cell_.edge_vector.size() << std::endl;
    std::cout << "NFA ����ʼ״̬��" << cell_.start_state << std::endl;
    std::cout << "NFA ����ֹ״̬��" << cell_.end_state << std::endl;

    size_t i = 0;
    for (const auto& edge : cell_.edge_vector)
    {
        std::cout << "��" << i + 1 << "���ߵ���ʼ״̬��" << edge.start_state
                    << "  ����״̬��" << edge.end_state
                    << "  ת������" << edge.trans_symbol << std::endl;
        ++i;
    }
    std::cout << "����" << std::endl;
}
