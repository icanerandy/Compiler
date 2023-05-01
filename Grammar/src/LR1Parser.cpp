/**
  ******************************************************************************
  * @file           : LR1Parser.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/24
  ******************************************************************************
  */

#include "include/LR1Parser.h"

LR1Parser::LR1Parser(const std::vector<Token>& tokens, const std::string& grammar_in, const std::string& lr_parse_result)
    : tokens_(tokens)
{
    grammar_in_.open(grammar_in, std::ios::in);
    if (!grammar_in_.is_open())
    {
        std::cerr << "��ȡ�﷨�ļ�ʧ�ܣ�" << std::endl;
        system("pause");
        exit(-1);
    }
    lr_parse_result_.open(lr_parse_result, std::ios::out);
    if (!lr_parse_result_.is_open())
    {
        std::cerr << "���LL(1)�������ʧ�ܣ�" << std::endl;
        system("pause");
        exit(-1);
    }

    // ��ʼ���ķ�
    InitGrammar();

    // ��FIRST����FOLLOW��
    GetFirstSet();
    GetFollowSet();

    // ����DFA��SLR1Ԥ�������
    DFA();

    // ��ʼ��action��goto��Ϊ��
    InitAction();
    InitGoto();

    // ����SLR1������
    CreateAnalysisTable();

    // �﷨����
    Parse();
 }

void LR1Parser::InitGrammar()
{
    std::string line;

    // �����ķ�
    while (std::getline(grammar_in_, line))
    {
        LeftPart left;
        RightPart right;
        Production prod;

        // ������
        std::stringstream ss(line);
        ss >> left;
        prod.left = left;

        // ��->�̵�
        std::string arrow;
        ss >> arrow;

        // �����Ҳ�����
        std::string symbol;
        while (ss >> symbol)
        {
            if (symbol == "|")
            {
                grammar_.prods.emplace_back(prod);
                prod.right.clear();
            }
            else
            {
                prod.right.emplace_back(symbol);
            }
        }
        grammar_.prods.emplace_back(prod);
    }
    /*
     * ΪʲôҪ���һ���µĲ���ʽ��
     * ��ΪҪ�����ʽתΪNFA�������ʼ���Ŷ�Ӧ�������ʽ����NFA�ĳ�̬Ҳ���ж��
     * ��Ȼ�ܴ�������̬�����Եú��鷳����������һ������ʽ
     */
    // ��S'->S���뿪ͷ
    LeftPart start_left = grammar_.prods.at(0).left + '\'';
    RightPart start_right;
    start_right.emplace_back(grammar_.prods.at(0).left);
    grammar_.prods.insert(grammar_.prods.begin(), {start_left, start_right});
    grammar_.num = grammar_.prods.size();

    // �󲿵Ķ�Ϊ���ս��
    for (const auto& prod : grammar_.prods)
        grammar_.N.emplace_back(prod.left);
    // ȥ��
    std::vector<std::string>::iterator it, it1;
    for (it = ++grammar_.N.begin(); it != grammar_.N.end();)
    {
        it1 = find(grammar_.N.begin(), it, *it); //����ǰλ��֮ǰ�����ظ�Ԫ�أ�ɾ����ǰԪ��,erase���ص�ǰԪ�ص���һ��Ԫ��ָ��
        if (it1 != it)
            it = grammar_.N.erase(it);
        else
            it++;
    }


    // �Ҳ���û�������г��ֹ��ķ��Ŷ�Ϊ�ս����ע����򽫦ŵ����ս��
    for (const auto& prod : grammar_.prods)
    {
        for (const auto& symbol : prod.right)
        {
            if (std::find(grammar_.N.begin(), grammar_.N.end(), symbol) == grammar_.N.end())
                    grammar_.T.emplace_back(symbol);
        }
    }
    // ȥ��
    for (it = ++grammar_.T.begin(); it != grammar_.T.end();)
    {
        it1 = find(grammar_.T.begin(), it, *it); //����ǰλ��֮ǰ�����ظ�Ԫ�أ�ɾ����ǰԪ��,erase���ص�ǰԪ�ص���һ��Ԫ��ָ��
        if (it1 != it)
            it = grammar_.T.erase(it);
        else
            it++;
    }
}

void LR1Parser::InitAction()
{
    for (size_t i = 0; i < canonicalCollection_.items.size(); ++i)
    {
        for (const auto & symbol : grammar_.T)
        {
            if (symbol != "<epsilon>")
                action_[i][symbol] = std::make_pair("", -1);
        }
        // ��Ҫ���˰ѽ������Ž�ȥ
        action_[i]["#"] = std::make_pair("", -1);
    }
}

void LR1Parser::InitGoto()
{
    for (size_t i = 0; i < canonicalCollection_.items.size(); ++i)
    {
        for (const auto & symbol : grammar_.N)
            goto_[i][symbol] = -1;
    }
}

void LR1Parser::DFA()
{
    // ������ʼ��Ŀ��
    LR1Item lr1Item;
    lr1Item.location = 0;
    lr1Item.prod.left = grammar_.prods[0].left;
    lr1Item.prod.right = grammar_.prods[0].right;
    lr1Item.next = "#";

    LR1Items lr1Items;
    lr1Items.items.emplace_back(lr1Item);
    Closure(lr1Items);

    // �����ʼ��Ч��Ŀ��
    canonicalCollection_.items.emplace_back(lr1Items);
    // ���¼������Ч��Ŀ���������չ������
    Q.emplace(lr1Items, 0);

    while (!Q.empty())
    {
        LR1Items& from = Q.front().first;
        size_t sidx = Q.front().second;
        // ����ÿ���ս��
        for (auto & symbol : grammar_.T)
        {
            LR1Items to;
            Go(from, symbol, to);
            size_t idx;
            // ���������Ŀ��to��Ϊ��
            if (!to.items.empty())
            {
                // �����Ƿ��Ѿ�����Ч��Ŀ������
                idx = IsInCanonicalCollection(to);
                if (idx > 0)
                {
                    idx -= 1;
                }
                else
                {
                    idx = canonicalCollection_.items.size();
                    canonicalCollection_.items.emplace_back(to);
                    // ���¼������Ч��Ŀ���������չ������
                    Q.emplace(to, idx);
                }
                // ��ԭ״̬��ת��״̬��һ���ߣ����ϵ�ֵΪת�Ʒ���
                canonicalCollection_.g[sidx].emplace_back(symbol, idx);
            }
        }

        // ����ÿ�����ս��
        for (auto & symbol : grammar_.N)
        {
            LR1Items to;
            Go(from, symbol, to);
            size_t idx;
            if (!to.items.empty())
            {
                idx = IsInCanonicalCollection(to);
                if (idx > 0)
                {
                    idx -= 1;
                }
                else
                {
                    idx = canonicalCollection_.items.size();
                    canonicalCollection_.items.emplace_back(to);
                    Q.emplace(to, idx);
                }
                canonicalCollection_.g[sidx].emplace_back(symbol, idx);
            }
        }

        // ��ǰ״̬��չ��ɣ������Ƴ�����
        Q.pop();
    }

//    // �Ƴ�DFAͼ�е��ظ�Ԫ��
//    for (auto & it : canonicalCollection_.g)
//    {
//        std::set< std::pair<std::string, size_t> > tmp(it.begin(), it.end());
//        it.assign(tmp.begin(), tmp.end());
//    }
}

void LR1Parser::CreateAnalysisTable()
{
    for (size_t i = 0; i < canonicalCollection_.items.size(); ++i)
    {
        LR1Items& LItems = canonicalCollection_.items.at(i);

        // ����action��
        for (const auto & LItem : LItems.items)
        {
            // �ǹ�Լ��Ŀ
            if (LItem.location < LItem.prod.right.size() && LItem.prod.right.at(LItem.location) != "<epsilon>")
            {
                std::string symbol = LItem.prod.right.at(LItem.location);

                if (IsTerminal(symbol))
                {
                    // �ҵ���Ӧ�ս���ĳ��ߣ��õ���ת�Ƶ���״̬
                    for (size_t k = 0; k < canonicalCollection_.g.at(i).size(); ++k)
                    {
                        std::pair<std::string, size_t> pair = canonicalCollection_.g.at(i).at(k);
                        if (pair.first == symbol)
                        {
                            action_.at(i).at(symbol).first = "Shift";
                            action_.at(i).at(symbol).second = pair.second;

                            /*
                             * ����
                             * S -> a b c
                             * S -> a b d
                             * ��ô����
                             */
                            break;
                        }
                    }
                }
            }
            else    // ��Լ��Ŀ
            {
                /* ������Ŀ */
                if (LItem.prod.left == grammar_.prods.at(0).left && LItem.location == LItem.prod.right.size())
                {
                    action_.at(i).at("#").first = "Accept";
                }
                else
                {
                    // �ҵ�����ʽ���ڵ����
                    for (size_t k = 0; k < grammar_.prods.size(); ++k)
                    {
                        if (LItem.prod == grammar_.prods.at(k))
                        {
                            action_.at(i).at(LItem.next).first = "Reduce";
                            action_.at(i).at(LItem.next).second = k;
                            break;
                        }
                    }
                }
            }
        }

        // ˵����ǰ״̬�����ڳ���
        if (canonicalCollection_.g.find(i) == canonicalCollection_.g.end())
            continue;

        // ����goto��
        // ������ǰ��Ŀ����pair<���ܷ��ţ�ת��״̬>
        for (const auto & it : canonicalCollection_.g.at(i))
        {
            std::string symbol = it.first;
            if (IsNonTerminal(symbol))
                goto_.at(i).at(symbol) = it.second;
        }
    }
}

void LR1Parser::Parse()
{
    std::cout << "Syntax parsing start!" << std::endl;
    /* ��ʼ�� */
    std::stack<size_t> state_stack; // ״̬ջ
    std::stack<std::string> symbol_stack; // ����ջ
    state_stack.emplace(0);
    symbol_stack.emplace("#");

    size_t ip = 0;
    do
    {
        std::cout << "�����" << ip << "��token..." << std::endl;
        std::string type = tokens_.at(ip)./*type*/content;

        size_t cur_state = state_stack.top();
        if (action_.at(cur_state).at(type).first == "Shift")
        {
            state_stack.emplace(action_.at(cur_state).at(type).second);
            symbol_stack.emplace(type);
            ++ip;
        }
        else if (action_.at(cur_state).at(type).first == "Reduce")
        {
            size_t idx = action_.at(cur_state).at(type).second;
            Production& prod = grammar_.prods.at(idx);

            size_t size = 0;
            if (!(prod.right.at(0) == "<epsilon>"))
                size = prod.right.size();
            for (; size >= 1; size--)
            {
                state_stack.pop();
                symbol_stack.pop();
            }

            symbol_stack.emplace(prod.left);
            cur_state = state_stack.top();

            cur_state = goto_.at(cur_state).at(prod.left);
            if (cur_state == -1)
            {
                // TODO: ������
                std::cerr << "Syntax error at " << "(" << tokens_.at(ip).line << ", " << tokens_.at(ip).column << ")" << std::endl;
                break;
            }
            else
            {
                state_stack.emplace(cur_state);
            }
        }
        else if (action_.at(cur_state).at(type).first == "Accept")
        {
            std::cout << "�﷨�����ɹ���" << std::endl;
            break;
        }
        else
        {
            // TODO: ������
            std::cerr << "Syntax error at " << "(" << tokens_.at(ip).line << ", " << tokens_.at(ip).column << ")" << std::endl;
            break;
        }
    } while (true);

}

void LR1Parser::Closure(LR1Items& lr1Items)
{
    bool still_changing = true;
    while (still_changing)  // ��ʼ����
    {
        still_changing = false;

        LR1Items tmp_items;
        // ö��ÿ����Ŀ
        tmp_items.items.assign(lr1Items.items.begin(), lr1Items.items.end());
        for (auto & L : tmp_items.items)
        {
            // �ǹ�Լ��Ŀ
            if (L.location < L.prod.right.size())
            {
                std::string symbol = L.prod.right.at(L.location);
                if (IsNonTerminal(symbol))  // �ѷ���������LR1��Ŀ����հ���
                {
                    // ��������ս������Ĵ���FIRST��
                    std::vector<std::string> ss;
                    ss.assign(L.prod.right.begin() + L.location + 1, L.prod.right.end());
                    ss.emplace_back(L.next);
                    std::set<std::string> string_first_set = GetStringFirstSet(ss);

                    for (auto & prod : grammar_.prods)
                    {
                        if (prod.left == symbol)
                        {
                            /* ö��ÿ����B����Ĵ���FIRST�� */
                            for (const auto& it : string_first_set)
                            {
                                LR1Item new_item;
                                new_item.location = 0;
                                new_item.prod = prod;
                                new_item.next = it;
                                // �鿴�Ƿ��Ѿ��ڵ�ǰ��LR1��Ŀ����
                                if (!IsInLR1Items(lr1Items, new_item))
                                {
                                    still_changing = true;  // ��������
                                    lr1Items.items.emplace_back(new_item);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

/*!
 * ת�ƺ���
 * @param from - ��ǰ����Ŀ��
 * @param symbol - ��symbolת��
 * @param to - ת�ƺ����Ŀ��ת�ƺ����Ŀ��
 */
void LR1Parser::Go(LR1Items &from, const std::string& symbol, LR1Items &to)
{
    for (const auto& item : from.items)
    {
        // Ѱ�ҷǹ�Լ��Ŀ
        if (item.location < item.prod.right.size() && item.prod.right.at(item.location) != "<epsilon>")
        {
            std::string next_symbol = item.prod.right.at(item.location);
            // ���������Ƿ��ս�����Һʹ���ķ�����ͬ
            if (next_symbol == symbol)
            {
                LR1Item lr1Item;
                lr1Item.location = item.location + 1;
                lr1Item.prod.left.assign(item.prod.left);
                lr1Item.next = item.next;
                lr1Item.prod.right.assign(item.prod.right.begin(), item.prod.right.end());
                to.items.emplace_back(lr1Item);
            }
        }
    }
    // ��to������Ŀ������հ�
    if (!to.items.empty())
        Closure(to);
}

/*!
 * ��T U N����FIRST��
 */
void LR1Parser::GetFirstSet()
{
    // �ս����FIRST�����䱾��
    for (const auto& symbol : grammar_.T)
        first_set_[symbol].emplace(symbol);
    for (const auto& symbol : grammar_.N)
        first_set_[symbol] = {};

    for (const auto& symbol : first_set_)
        std::cout << symbol.first << "\t";

    std::cout << "\n\n";
    for (const auto& symbol : grammar_.prods)
        std::cout << symbol.left << "\t";

    // �������ҷ��ս����FIRST��
    bool still_changing = true;
    while (still_changing)
    {
        still_changing = false;
        for (const auto & prod : grammar_.prods)
        {
            size_t old_size = first_set_.at(prod.left).size();

            // ����Ҳ���һ�������ǿջ��ս��������뵽�󲿵�FIRST����
            std::string symbol = prod.right.at(0);
            if (IsTerminal(symbol) || symbol == "<epsilon>")
            {
                first_set_.at(prod.left).emplace(symbol);
            }
            else
            {
                bool next = true; // ��ǰ�����Ƿ��ս�����ҵ�ǰ���ſ����Ƴ��գ�����Ҫ�ж���һ������
                size_t idx = 0; // ���жϷ��ŵ��±�
                while (next && idx < prod.right.size())
                {
                    next = false;
                    symbol = prod.right.at(idx);
                    // ������ǰ���ŵ�FIRST��
                    for (const auto& it : first_set_.at(symbol))
                    {
                        // �ѵ�ǰ���ŵ�FIRST���зǿ�Ԫ�ؼ��뵽�󲿷��ŵ�FIRST����
                        // �Ҳ����з��Ŷ����Ƶ����ţ���Ѧż��뵽�󲿷��ŵ�FIRST����
                        if (it != "<epsilon>")
                        {
                            first_set_.at(prod.left).emplace(it);
                        }
                        else    // ��ǰ�������Ƶ����ţ������ж���һ������
                        {
                            next = true;
                            idx += 1;
                        }
                    }
                }
                // ���nextΪtrue�������ʽ�Ҳ�ÿһ�����Ŷ����Ƶ����ţ����ż����󲿵�FIRST����
                if (next)
                    first_set_.at(prod.left).emplace("<epsilon>");
            }

            std::cout << prod.left << std::endl;
            size_t new_size = first_set_.at(prod.left).size();
            if (new_size != old_size)
                still_changing = true;
        }
    }

    //system("cls");
    std::cout << "FIRST:" << std::endl;
    for (const auto & it : grammar_.N)
    {
        std::cout << it << ": ";
        for (const auto & it1 : first_set_.at(it))
            std::cout << it1 << " ";
        std::cout << std::endl;
    }
    //system("pause");
}

/*!
 * ����ս����FOLLOW��
 */
void LR1Parser::GetFollowSet()
{
    // ��ʼ���ս����FOLLOW��Ϊ�ռ�
    for (const auto& symbol : grammar_.N)
        follow_set_[symbol] = {};

    // ��#���뵽�ķ��Ŀ�ʼ���ŵ�FOLLOW����
    follow_set_.at(grammar_.N.at(0)).emplace("#");

    bool still_changing = true;
    while (still_changing)
    {
        still_changing = false;
        for (const auto & prod : grammar_.prods)
        {
            for (size_t i = 0; i < prod.right.size(); ++i)
            {
                if (IsNonTerminal(prod.right.at(i)))
                {

                    size_t old_size = follow_set_.at(prod.right.at(i)).size();
                    // ss�Ǵ���һ�����ſ�ʼ�ķ��Ŵ�
                    std::vector<std::string> ss(prod.right.begin() + i + 1, prod.right.end());
                    std::set<std::string> afters_first_set = GetStringFirstSet(ss);
                    bool can_result_epsilon = false;
                    // ��ss��FIRST�������зǿ�Ԫ�ؼ��뵽��ǰ���ŵ�FOLLOW����
                    for (const auto & it : afters_first_set)
                    {
                        if (it != "<epsilon>")
                            follow_set_.at(prod.right.at(i)).emplace(it);
                        else
                            can_result_epsilon = true;
                    }

                    // ���ss���Ƶ����ţ����ߵ�ǰ�����ǲ���ʽ�Ҳ�ĩβ
                    if (can_result_epsilon || (i+1 == prod.right.size()))
                        follow_set_.at(prod.right.at(i)).insert(
                                follow_set_.at(prod.left).begin(),
                                follow_set_.at(prod.left).end()
                                );

                    size_t new_size = follow_set_.at(prod.right.at(i)).size();
                    if (new_size != old_size)
                        still_changing = true;
                }
            }
        }
    }

    //system("cls");
    std::cout << "FOLLOW:" << std::endl;
    for (const auto & it : grammar_.N)
    {
        std::cout << it << ": ";
        for (const auto & it1 : follow_set_.at(it))
            std::cout << it1 << " ";
        std::cout << std::endl;
    }
    //system("pause");
}

/*!
 * �󴮵�FIRST���У����ؽ����
 * @param ss
 * @return
 */
std::set<std::string> LR1Parser::GetStringFirstSet(const std::vector<std::string>& ss)
{
    std::set<std::string> first;

    if (ss.empty())
        return first;

    // ��ǰ�����Ƿ��ս�����ҵ�ǰ���ſ����Ƶ����գ�����Ҫ�ж���һ������
    bool still_changing = true;
    size_t idx = 0;
    while (still_changing && idx < ss.size())
    {
        still_changing = false;
        // ��ǰ�������ս����գ����뵽FIRST����
        if (IsTerminal(ss.at(idx)) || ss.at(idx) == "<epsilon>")
        {
            first.emplace(ss.at(idx));
        }
        else
        {
            for (const auto & symbol : first_set_.at(ss.at(idx)))
            {
                if (symbol != "<epsilon>")
                    first.emplace(symbol);
                else
                    still_changing = true;
            }
        }
        idx += 1;
    }

    // ����������ʽ�Ҳ�ĩβ��Ϊ�棬˵�������������Ƶ����գ����ż��뵽FIRST����
    if (still_changing)
        first.emplace("<epsilon>");

    return first;
}

bool LR1Parser::IsTerminal(const std::string& symbol)
{
    return std::find(grammar_.T.begin(), grammar_.T.end(), symbol) != grammar_.T.end() || symbol == "#";
}

bool LR1Parser::IsNonTerminal(const std::string& symbol)
{
    return std::find(grammar_.N.begin(), grammar_.N.end(), symbol) != grammar_.N.end();
}

bool LR1Parser::IsInLR1Items(const LR1Items& lr1Items, const LR1Item& lr1Item) const
{
    if (std::any_of(lr1Items.items.begin(), lr1Items.items.end(), [&lr1Item](const LR1Item& item) {
        return item.prod == lr1Item.prod && item.location == lr1Item.location && item.next == lr1Item.next;
    })) return true;
    return false;
}

/*!
 * �жϴ������Ŀ���Ƿ�����Ŀ���淶���У����ڷ��������
 * @param lr1Items
 * @return
 */
size_t LR1Parser::IsInCanonicalCollection(LR1Items &lr1Items)
{
    for (size_t i = 0; i < canonicalCollection_.items.size(); ++i)
    {
        LR1Items& cur_items = canonicalCollection_.items.at(i);

        if (cur_items.items.size() != lr1Items.items.size())
            continue;

        // ÿ����Ŀ���ڸ���Ŀ���У�����Ϊ��������Ŀ�����
        bool flag = true;
        for (const auto& item : cur_items.items)
        {
            if (!IsInLR1Items(lr1Items, item))
            {
                flag = false;
                break;
            }
        }

        if (flag)
        {
            return i + 1;
        }
    }
    return 0;
}

LR1Parser::~LR1Parser()
= default;
