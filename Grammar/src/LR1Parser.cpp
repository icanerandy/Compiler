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
    out_.open(lr_parse_result, std::ios::out);
    if (!out_.is_open())
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

    // ��ӡ������
    tree->RemoveEmptyNodes();
    tree->PrettyPrint();
 }

void LR1Parser::InitGrammar()
{
    std::string line;

    Production prod;

    // ������
    std::getline(grammar_in_, line);
    std::stringstream ss(line);
    ss >> prod.left;

    // ��->�̵�
    std::string arrow;
    ss >> arrow;

    // �����Ҳ�����
    std::string symbol;
    while (ss >> symbol)
        prod.right.emplace_back(symbol);
    grammar_.prods.emplace_back(prod);

    // �����ķ�
    while (std::getline(grammar_in_, line))
    {
        // �����һ������
        std::stringstream s(line);
        std::string first_symbol;
        s >> first_symbol;

        if (first_symbol.empty())
        {
            continue;
        }
        else if (first_symbol == "|")
        {
            prod.right.clear();
            while (s >> symbol)
                prod.right.emplace_back(symbol);
            grammar_.prods.emplace_back(prod);
        }
        else
        {
            prod.left = first_symbol;

            // ��->�̵�
            s >> arrow;

            prod.right.clear();
            while (s >> symbol)
                prod.right.emplace_back(symbol);
            grammar_.prods.emplace_back(prod);
        }
    }
 /*   // �����ķ�
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

    // �ļ����һ�п����û������������һ�лس�����ʱҪ����
    if (grammar_.prods.at(grammar_.prods.size() - 1).left.empty())
        grammar_.prods.pop_back();*/

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
    for (const auto& production : grammar_.prods)
        grammar_.N.emplace_back(production.left);
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
    for (const auto& production : grammar_.prods)
    {
        for (const auto& sym : production.right)
        {
            if (std::find(grammar_.N.begin(), grammar_.N.end(), sym) == grammar_.N.end())
                    grammar_.T.emplace_back(sym);
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

/*    // ��ӡ��Ŀ���淶��
    std::cout << "\n\n\n";
    std::cout << "canonicalCollection's size is " << canonicalCollection_.items.size() << "\n";
    for (size_t i = 0; i < canonicalCollection_.items.size(); ++i)
    {
        std::cout << "---------------------------------------------------------------------\n";
        std::cout << "LR1Items " << i << ":\n";
        const LR1Items& items = canonicalCollection_.items.at(i);
        for (const auto& item : items.items)
        {
            std::cout << item.prod.left << "->";
            for (size_t k = 0; k < item.prod.right.size(); ++k)
            {
                if (item.location == k)
                    std::cout << ".";
                std::cout << item.prod.right.at(k);
            }
            if (item.location == item.prod.right.size())
                std::cout << ".";
            std::cout << ", " << item.next << "\t";
        }
        std::cout << "\n";

        if (canonicalCollection_.g.find(i) != canonicalCollection_.g.end())
        {
            for (const auto& pair : canonicalCollection_.g.at(i))
            {
                std::cout << "using " << pair.first << " to " << pair.second << "\n";
            }
        }
    }
    std::cout << "---------------------------------------------------------------------\n";*/
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
                    // �ж��Ƿ�����ƽ�-��Լ��ͻ
                    // ���չ������else����ѡ���ƽ�
                    // ��else��if�ͽ�ƥ��
                    if (symbol != "else" && action_.at(i).at(symbol).first == "Reduce")
                    {
                        std::cerr << "�����ƽ�-��Լ��ͻ����Ŀ����" << i << "���ƽ�ʱ��ͻ��" << LItem.prod.left << " -> ";
                        for (const auto& right : LItem.prod.right)
                            std::cerr << right << " ";
                        std::cerr << std::endl;
                    }

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
            // ��Լ��Ŀ
            else
            {
                /* ������Ŀ */
                if (LItem.prod.left == grammar_.prods.at(0).left && LItem.location == LItem.prod.right.size())
                {
                    action_.at(i).at("#").first = "Accept";
                }
                else
                {
                    // �ж��Ƿ�����ƽ�-��Լ��ͻ
                    if (action_.at(i).at(LItem.next).first == "Shift")
                    {
                        // ���չ������else����ѡ���ƽ�
                        // ��else��if�ͽ�ƥ��
                        if (LItem.next == "else")
                            continue;

                        std::cerr << "�����ƽ�-��Լ��ͻ����Ŀ����" << i << "����Լʱ��ͻ��" << LItem.prod.left << " -> ";
                        for (const auto& right : LItem.prod.right)
                            std::cerr << right << " ";
                        std::cerr << std::endl;
                    }

                    // �ҵ�����ʽ��Ӧ�����

                    // TODO:
                    // LR(1)һ����Լ��Ŀ����ǰ�����ж�����ж����Ŀ����ǰ�����ⶼ��ͬ���⼸����Ŀ���������
                    // Ȼ���ڶ�Ӧ�Ľ�����ǰ����ʱ��Լ��������չ������ʱ��Լ
                    /* �⼸����Ŀ����ͬ����Ŀ������ʹ��չ�����Ĳ�����ΪLR1��Ŀ��һ���� */
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
    out_ << "Syntax parsing start!\n";
    out_ << "---------------------------------------------------------------------\n";
    /* ��ʼ�� */
    std::stack<size_t> state_stack; // ״̬ջ
    std::stack<std::string> symbol_stack; // ����ջ
    std::stack<ASTNode *> tree_node;    // ����﷨���ڵ�
    state_stack.emplace(0);
    symbol_stack.emplace("#");

    size_t ip = 0;
    do
    {
        std::string symbol;
        if (tokens_.at(ip).type == "MAIN_ID" || tokens_.at(ip).type == "<������>" || tokens_.at(ip).type == "<����>"
                || tokens_.at(ip).type == "<����>" || tokens_.at(ip).type == "<����>"
                || tokens_.at(ip).type == "<�ַ�����>" || tokens_.at(ip).type == "<�ַ�������>")
            symbol = tokens_.at(ip).type;
        else
            symbol = tokens_.at(ip).content;

        out_ << "�����" << ip << "��token " << "--- (" << tokens_.at(ip).type << ",  " << tokens_.at(ip).content << ")"
                    << "\t\t\t���У�" << "(" << tokens_.at(ip).line << " , " << tokens_.at(ip).column << ")\n";

        size_t cur_state = state_stack.top();

        if (action_.at(cur_state).find(symbol) == action_.at(cur_state).end())
        {
            // TODO: ������
            std::cerr << "Syntax error at " << "(" << tokens_.at(ip).line << ", " << tokens_.at(ip).column << ")"
                        << "\t" << tokens_.at(ip).content << std::endl;
            break;
        }

        if (action_.at(cur_state).at(symbol).first == "Shift")
        {
            state_stack.emplace(action_.at(cur_state).at(symbol).second);
            symbol_stack.emplace(symbol);
            if (tokens_.at(ip).type == "<������>" || tokens_.at(ip).type == "<����>" || tokens_.at(ip).type == "<����>" || tokens_.at(ip).type == "<����>" || tokens_.at(ip).type == "<�ַ�����>" || tokens_.at(ip).type == "<�ַ�������>")
                tree_node.emplace(MkLeaf(tokens_.at(ip)));
            ++ip;
        }
        else if (action_.at(cur_state).at(symbol).first == "Reduce")
        {
            size_t idx = action_.at(cur_state).at(symbol).second;
            Production& prod = grammar_.prods.at(idx);

            out_ << "ѡ���˲���ʽ" << prod.left << " -> ";
            for (const auto& right : prod.right)
                out_ << right << " ";
            out_ << "���й�Լ\n";

            size_t size;
            if (prod.right.at(0) == "<epsilon>")
            {
                size = prod.right.size() - 1;
            }
            else
            {
                size = prod.right.size();
            }

            std::vector<std::string> s;  // ��¼��Լ����Щ����
            for (; size >= 1; size--)
            {
                state_stack.pop();
                s.emplace_back(symbol_stack.top());
                symbol_stack.pop();
            }
            ASTNode* node = MkNode(idx, tree_node);
            tree_node.emplace(node);

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
        else if (action_.at(cur_state).at(symbol).first == "Accept")
        {
            out_ << "�﷨�����ɹ���\n";
            std::cout << "�﷨�����ɹ���" << std::endl;
            auto * root = new ASTTree(tree_node.top());
            tree = root;
            break;
        }
        else
        {
            // TODO: ������
            std::cerr << "Syntax error at " << "(" << tokens_.at(ip).line << ", " << tokens_.at(ip).column << ")" << std::endl;
            break;
        }
    } while (true);
    out_ << "---------------------------------------------------------------------\n";
    out_.flush();
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

            size_t new_size = first_set_.at(prod.left).size();
            if (new_size != old_size)
                still_changing = true;
        }
    }
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

ASTNode *LR1Parser::MkLeaf(const Token& token)
{
    auto * node = new ASTNode();
    node->ast_name_ = token.content;

    node->type_ = token.type;
    node->content_ = token.content;
    node->line_no_ = token.line;
    node->column_no_ = token.column;

    return node;
}

ASTNode *LR1Parser::MkNode(size_t idx, std::stack<ASTNode *>& tree_node)
{
    auto * node = new ASTNode();

    size_t size_of_prod = grammar_.prods.at(idx).right.size();

    ASTNode * tmp;

    switch (idx)
    {
        // TODO: �մ���Լ��
        case 0: /* 0. <����>�� -> <����> # */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 1: /* 1. <����> -> <��������б�> <MAIN��������> <���������б�> */
        {
            node->ast_name_ = "����";

            ASTNode * func_def_list = tree_node.top();  // ���������б�
            tree_node.pop();

            while (func_def_list)
            {
                node->children_.emplace_back(func_def_list);
                func_def_list = func_def_list->next_;
            }

            ASTNode * main_def = tree_node.top();   // MAIN��������
            tree_node.pop();
            node->children_.emplace_front(main_def);

            ASTNode * declaration_list = tree_node.top();   // ��������б�
            tree_node.pop();

            while (declaration_list)
            {
                node->children_.emplace_back(declaration_list);
                declaration_list = declaration_list->next_;
            }
        }
            break;
        case 2: /* 2. <MAIN��������> -> MAIN_ID ( ) <�������> */
        {
            node->ast_name_ = "MAIN��������";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 3: /* 3. <��������б�> -> <�������> <��������б�> */
        {
            ASTNode * declaration_list = tree_node.top();   // ��������б�
            tree_node.pop();

            ASTNode * declaration  = tree_node.top();   // �������
            tree_node.pop();

            declaration->next_ = declaration_list;

            node = declaration;
        }
            break;
        case 4: /* 4. <��������б�> -> <epsilon> */
        {
            node = nullptr;
        }
            break;
        case 5: /* 5. <���������б�> -> <��������> <���������б�> */
        {
            ASTNode * func_def_list = tree_node.top();  // ���������б�
            tree_node.pop();

            ASTNode * func_def = tree_node.top();   // ��������
            tree_node.pop();

            func_def->next_ = func_def_list;

            node = func_def;
        }
            break;
        case 6: /* 6. <���������б�> -> <epsilon> */
        {
            return nullptr;
        }
            break;
        case 7: /* 7. <��������> -> <��������> <������> ( <���������β��б�> ) <�������> */
        {
            node->ast_name_ = "��������";

            ASTNode * compound_stmt = tree_node.top();
            tree_node.pop();

            ASTNode * func_def_args_list = tree_node.top();
            tree_node.pop();

            ASTNode * func_name = tree_node.top();
            tree_node.pop();

            ASTNode * type = tree_node.top();
            tree_node.pop();

            node->children_.emplace_front(compound_stmt);
            node->children_.emplace_front(func_def_args_list);
            node->children_.emplace_front(func_name);
            node->children_.emplace_front(type);
        }
            break;
        case 8: /* 8. <���������β��б�> -> <���������β�> */
        {
            node->ast_name_ = "���������β��б�";

            tmp = tree_node.top();
            tree_node.pop();

            while (tmp)
            {
                node->children_.emplace_back(tmp);
                tmp = tmp->next_;
            }
        }
            break;
        case 9: /* 9. <���������β��б�> -> <epsilon> */
        {
            return nullptr;
        }
            break;
        case 10: /* 10. <���������β�> -> <��������> <����> */
        {
            ASTNode * var = tree_node.top();    // ����
            tree_node.pop();

            ASTNode * type = tree_node.top();   // ��������
            tree_node.pop();

            type->children_.emplace_front(var);

            node = type;
        }
            break;
        case 11: /* 11. <���������β�> -> <��������> <����> , <���������β�> */
        {
            ASTNode * func_args = tree_node.top();  // ���������β�
            tree_node.pop();

            ASTNode * var = tree_node.top();    // ����
            tree_node.pop();

            ASTNode * type = tree_node.top();   // ��������
            tree_node.pop();

            type->children_.emplace_front(var);

            type->next_ = func_args;

            node = type;
        }
            break;
        case 12: /* 12. <�������> -> { <����> } */
        {
            node->ast_name_ = "�������";

            ASTNode * stmt_table = tree_node.top();
            tree_node.pop();

            while (stmt_table)
            {
                node->children_.emplace_back(stmt_table);
                stmt_table = stmt_table->next_;
            }
        }
            break;
        case 13: /* 13. <����> -> <���> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 14: /* 14. <����> -> <���> <����> */
        {
            ASTNode * stmt_table = tree_node.top();
            tree_node.pop();

            ASTNode * stmt = tree_node.top();
            tree_node.pop();

            stmt->next_ = stmt_table;

            node =  stmt;
        }
            break;
        case 15: /* 15. <ִ�����> -> <���ݴ������>  */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 16: /* 16. <ִ�����> -> <�������> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 17: /* 17. <���ݴ������> -> <���ʽ> ; */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 18: /* 18. <�������> -> <if���> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 19: /* 19. <�������> -> <for���> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 20: /* 20. <�������> -> <while���> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 21: /* 21. <�������> -> <do-while���> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 22: /* 22. <�������> -> <return���> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 23: /* 23. <if���> -> if ( <���ʽ> ) <ѭ�����> <if-tail> */
        {
            node->ast_name_ = "if���";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 24: /* 24. <if-tail> -> else <ѭ�����> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 25: /* 25. <if-tail> -> <epsilon> */
        {
            return nullptr;
        }
            break;
        case 26: /* 26. <for���> -> for ( <���ʽ> ; <���ʽ> ; <���ʽ> ) <ѭ�����> */
        {
            node->ast_name_ = "for���";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 27: /* 27. <while���> -> while ( <���ʽ> ) <ѭ�����> */
        {
            node->ast_name_ = "while���";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 28: /* 28. <do-while���> -> do <ѭ���ø������> while ( <���ʽ> ) ; */
        {
            node->ast_name_ = "do-while���";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 29: /* 29. <ѭ�����> -> <�������> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 30: /* 30. <ѭ�����> -> <ѭ��ִ�����> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 31: /* 31. <ѭ�����> -> <ѭ���ø������> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 32: /* 32. <ѭ���ø������> -> { <ѭ������> } */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 33: /* 33. <ѭ������> -> <ѭ�����> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 34: /* 34. <ѭ������> -> <ѭ�����> <ѭ������> */
        {
            node->ast_name_ = "ѭ������";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 35: /* 35. <ѭ��ִ�����> -> <if���> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 36: /* 36. <ѭ��ִ�����> -> <for���> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 37: /* 37. <ѭ��ִ�����> -> <while���> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 38: /* 38. <ѭ��ִ�����> -> <do-while���> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 39: /* 39. <ѭ��ִ�����> -> <return���> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 40: /* 40. <ѭ��ִ�����> -> <break���> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 41: /* 41. <ѭ��ִ�����> -> <continue���> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 42: /* 42. <ѭ��ִ�����> -> <���ݴ������> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 43: /* 43. <return���> -> return ; */
        {
            node->ast_name_ = "return���";
        }
            break;
        case 44: /* 44. <return���> -> return <���ʽ> ; */
        {
            node->ast_name_ = "return���";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 45: /* 45. <break���> -> break ; */
        {
            node->ast_name_ = "break���";
        }
            break;
        case 46: /* 46. <continue���> -> continue ; */
        {
            node->ast_name_ = "continue���";
        }
            break;
        case 47: /* 47. <���> -> <�������> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 48: /* 48. <���> -> <ִ�����> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 49: /* 49. <�������> -> <ֵ����> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 50: /* 50. <�������> -> <��������> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 51: /* 51. <ֵ����> -> <��������> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 52: /* 52. <ֵ����> -> <��������> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 53: /* 53. <��������> -> const <��������> <����������> */
        {
            node->ast_name_ = "��������";

            ASTNode * table = tree_node.top();  // ����������
            tree_node.pop();

            ASTNode * type = tree_node.top();   // ��������
            tree_node.pop();

            while (table)
            {
                type->children_.emplace_front(table);
                table = table->next_;
            }

            node->children_.emplace_front(type);
        }
            break;
        case 54: /* 54. <��������> -> int */
        {
            node->ast_name_ = "int";
        }
            break;
        case 55: /* 55. <��������> -> float */
        {
            node->ast_name_ = "float";
        }
            break;
        case 56: /* 56. <��������> -> char */
        {
            node->ast_name_ = "char";
        }
            break;
        case 57: /* 57. <��������> -> String */
        {
            node->ast_name_ = "String";
        }
            break;
        case 58: /* 58. <����������> -> <����> = <����> ; */
        {
            node->ast_name_ = "=";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 59: /* 59. <����������> -> <����> = <����> , <����������> */
        {
            ASTNode * table = tree_node.top();  // ����������
            tree_node.pop();

            ASTNode * const_num = tree_node.top();   // ����
            tree_node.pop();

            ASTNode * const_var = tree_node.top();  // ����
            tree_node.pop();

            node = new ASTNode();
            node->ast_name_ = "=";
            node->children_.emplace_front(const_num);
            node->children_.emplace_front(const_var);

            node->next_ = table;
        }
            break;
        case 60: /* 60. <��������> -> <��������> <����������> */
        {
            node->ast_name_ = "��������";

            ASTNode * table = tree_node.top();  // ����������
            tree_node.pop();

            ASTNode * type = tree_node.top();   // ��������
            tree_node.pop();

            while (table)
            {
                type->children_.emplace_back(table);
                table = table->next_;
            }

            node->children_.emplace_front(type);
        }
            break;
        case 61: /* 61. <����������> -> <����������> ; */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 62: /* 62. <����������> -> <����������> , <����������> */
        {
            ASTNode * table = tree_node.top();  // ����������
            tree_node.pop();

            ASTNode * single_decl = tree_node.top();  // ����������
            tree_node.pop();

            single_decl->next_ = table;

            node = single_decl;
        }
            break;
        case 63: /* 63. <����������> -> <����> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 64: /* 64. <����������> -> <����> = <�������ʽ> */
        {
            node->ast_name_ = "=";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 65: /* 65. <��������> -> <��������> <������> ( <���������β��б�> ) ; */
        {
            node->ast_name_ = "��������";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 66: /* 66. <��������> -> <��������> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 67: /* 67. <��������> -> void*/
        {
            node->ast_name_ = "void";
        }
            break;
        case 68: /* 68. <���������β��б�> -> <���������β�> */
        {
            node->ast_name_ = "���������β��б�";

            ASTNode * func_args = tree_node.top();
            tree_node.pop();

            while (func_args)
            {
                node->children_.emplace_back(func_args);
                func_args = func_args->next_;
            }
        }
            break;
        case 69: /* 69. <���������β��б�> -> <epsilon> */
        {
            return nullptr;
        }
            break;
        case 70: /* 70. <���������β�> -> <��������> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 71: /* 71. <���������β�> -> <��������> , <���������β�> */
        {
            ASTNode * func_args = tree_node.top();  // ���������β�
            tree_node.pop();

            ASTNode * type = tree_node.top();   // ��������
            tree_node.pop();

            type->next_ = func_args;

            node = type;
        }
            break;
        case 72: /* 72. <���ʽ> -> <��ֵ���ʽ> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 73: /* 73. <���ʽ> -> <�򵥱��ʽ> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 74: /* 74. <�򵥱��ʽ> -> <�������ʽ> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 75: /* 75. <��ֵ���ʽ> -> <����> = <�������ʽ> */
        {
            node->ast_name_ = "=";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 76: /* 76. <�������ʽ> -> <�������ʽ> || <������> */
        {
            node->ast_name_ = "||";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 77: /* 77. <�������ʽ> -> <������> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 78: /* 78. <������> -> <������> && <��������> */
        {
            node->ast_name_ = "&&";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 79: /* 79. <������> -> <��������> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 80: /* 80. <��������> -> ! <��������> */
        {
            node->ast_name_ = "!";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 81: /* 81. <��������> -> <��ϵ���ʽ> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 82: /* 82. <��ϵ���ʽ> -> <�������ʽ> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 83: /* 83. <��ϵ���ʽ> -> <�������ʽ> <��ϵ�����> <�������ʽ> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->ast_name_ = tmp->ast_name_;

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 84: /* 84. <��ϵ�����> -> > */
        {
            node->ast_name_ = ">";
        }
            break;
        case 85: /* 85. <��ϵ�����> -> < */
        {
            node->ast_name_ = "<";
        }
            break;
        case 86: /* 86. <��ϵ�����> -> >= */
        {
            node->ast_name_ = ">=";
        }
            break;
        case 87: /* 87. <��ϵ�����> -> <= */
        {
            node->ast_name_ = "<=";
        }
            break;
        case 88: /* 88. <��ϵ�����> -> == */
        {
            node->ast_name_ = "==";
        }
            break;
        case 89: /* 89. <��ϵ�����> -> != */
        {
            node->ast_name_ = "!=";
        }
            break;
        case 90: /* 90. <�������ʽ> -> <��> + <�������ʽ> */
        {
            node->ast_name_ = "+";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 91: /* 91. <�������ʽ> -> <��> - <�������ʽ> */
        {
            node->ast_name_ = "-";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 92: /* 92. <�������ʽ> -> <��> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 93: /* 93. <��> -> <����> * <��> */
        {
            node->ast_name_ = "*";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 94: /* 94. <��> -> <����> / <��> */
        {
            node->ast_name_ = "/";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 95: /* 95. <��> -> <����> % <��> */
        {
            node->ast_name_ = "%";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 96: /* 96. <��> -> <����> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 97: /* 97. <����> -> ( <�������ʽ> ) */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 98: /* 98. <����> -> <����> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 99: /* 99. <����> -> <�ַ�����> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 100: /* 100. <����> -> <�ַ�������> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 101: /* 101. <����> -> <����> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 102: /* 102. <����> -> <��������> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 103: /* 103. <��������>  -> <������> ( <ʵ���б�> ) */
        {
            node->ast_name_ = "��������";

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 104: /* 104. <ʵ���б�> -> <���ʽ> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node = tmp;
        }
            break;
        case 105: /* 105. <ʵ���б�> -> <���ʽ> , <ʵ���б�> */
        {
            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);

            tmp = tree_node.top();
            tree_node.pop();
            node->children_.emplace_front(tmp);
        }
            break;
        case 106: /* 106. <ʵ���б�> -> <epsilon> */
        {
            return nullptr;
        }
            break;
        default:
            break;
    }

    return node;
}

LR1Parser::~LR1Parser()
= default;
