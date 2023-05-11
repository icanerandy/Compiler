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
    : tokens_(tokens),
    symbol_scope_("global")
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

    tree->AddTypeSystem(&type_system_);
    // ִ�н�����
    Symbol * main_entry = type_system_.GetSymbolByText("main");
    if (main_entry == nullptr)
    {
        // TODO: δ����main����
    }
    else {
        tree->ExecuteTree(main_entry);
    }
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
            out_ << "Shift for Input: " << symbol << "\n";

            if (symbol == "(" || symbol == "{")
                nesting_level_++;
            if (symbol == ")" || symbol == "}")
                nesting_level_--;
            if (symbol == "MAIN_ID")
                symbol_scope_ = "main";

            state_stack.emplace(action_.at(cur_state).at(symbol).second);
            symbol_stack.emplace(symbol);

            if (tokens_.at(ip).type == "<������>" || tokens_.at(ip).type == "<����>" || tokens_.at(ip).type == "<����>"
                || tokens_.at(ip).type == "<����>" || tokens_.at(ip).type == "<�ַ�����>" || tokens_.at(ip).type == "<�ַ�������>")
                tree_node_.emplace(MkLeaf(tokens_.at(ip)));
            ++ip;
        }
        else if (action_.at(cur_state).at(symbol).first == "Reduce")
        {
            size_t idx = action_.at(cur_state).at(symbol).second;
            Production& prod = grammar_.prods.at(idx);

            out_ << "reduced by product:\n" << prod.left << " -> ";
            for (const auto& right : prod.right)
                out_ << right << " ";
            out_ << "look ahead {" << symbol << "}\n";

            size_t size;
            if (prod.right.at(0) == "<epsilon>")
            {
                size = prod.right.size() - 1;
            }
            else
            {
                size = prod.right.size();
            }

            ASTNode* node = MkNode(idx);
            tree_node_.emplace(node);

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
        else if (action_.at(cur_state).at(symbol).first == "Accept")
        {
            out_ << "�﷨�����ɹ���\n";
            std::cout << "�﷨�����ɹ���" << std::endl;
            auto * root = new ASTTree(tree_node_.top());
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

    node->type_ = token.type;
    node->content_ = token.content;
    node->line_no_ = token.line;
    node->column_no_ = token.column;

    // TODO: ����ͬ������
    if (token.type == "MAIN_ID" || token.type == "<������>")
        if (nesting_level_ == 0)   // ����Ϊȫ��������ʱ�޸��������ڷ��ű���˵���Ѿ��������ˣ����Ϊ�������û���
            symbol_scope_.assign(token.content);

    return node;
}

ASTNode *LR1Parser::MkNode(size_t idx)
{
    auto * node = new ASTNode();

    size_t size_of_prod = grammar_.prods.at(idx).right.size();

    ASTNode * tmp;

    switch (idx)
    {
        // TODO: �մ���Լ��
        case 0: /* 0. <����>�� -> <����> # */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 1: /* 1. <����> -> <��������б�> <MAIN��������> <���������б�> */
        {
            node->type_ = "����";

            ASTNode * func_def_list = tree_node_.top();  // ���������б�
            tree_node_.pop();

            ASTNode * main_def = tree_node_.top();   // MAIN��������
            tree_node_.pop();

            ASTNode * declaration_list = tree_node_.top();   // ��������б�
            tree_node_.pop();

            // ����Ŀ�����
            ProgramGenerator::GetInstance()->Generate();
            ProgramGenerator::EmitBlankLine();
            while (declaration_list)
            {
                if (declaration_list->type_ == "��������")
                {
                    auto child = declaration_list->GetChildrenList().front()->GetChildrenList().front();
                    if (child->type_ == "INIT")
                        ProgramGenerator::EmitDirective(Directive::FIELD_PRIVATE_STATIC, child->GetChildrenList().front()->content_, "I", "= " +
                                std::to_string(child->GetChildrenList().back()->i_value_));
                    else
                        ProgramGenerator::EmitDirective(Directive::FIELD_PRIVATE_STATIC, child->content_, "I");
                }

                node->children_.emplace_back(declaration_list);
                declaration_list = declaration_list->next_;
            }

            node->children_.emplace_back(main_def);

            ProgramGenerator::GetInstance()->GenerateMainMethod();
            GenerateAsm(main_def->GetChildrenList().front());
            ProgramGenerator::Emit(Instruction::RETURN);
            ProgramGenerator::EmitDirective(Directive::END_METHOD);

            ASTNode * cur_func = func_def_list;
            while (cur_func)
            {
                node->children_.emplace_back(cur_func);

                auto func = cur_func->GetChildrenList().front()->next_;
                Symbol * func_sym = func->GetSymbol();

                std::string declaration = func_sym->name_ + EmitArgs(func_sym);
                ProgramGenerator::EmitDirective(Directive::METHOD_PUBLIC_STATIC, declaration);
                GenerateAsm(cur_func);
                ProgramGenerator::EmitDirective(Directive::END_METHOD);

                cur_func = cur_func->next_;
            }

            ProgramGenerator::GetInstance()->finish();
        }
            break;
        case 2: /* 2. <MAIN��������> -> MAIN_ID ( ) <�������> */
        {
            node->type_ = "MAIN_ID";

            ASTNode * compound_stmt = tree_node_.top();
            tree_node_.pop();
            node->children_.emplace_front(compound_stmt);

            Symbol * main_entry = type_system_.NewSymbol("main", symbol_scope_);
            main_entry->body_ = compound_stmt;
            type_system_.AddDeclarator(main_entry, Declarator::FUNCTION);
            symbol_scope_ = "main";
            type_system_.AddSymbolsToTable(main_entry, symbol_scope_);

            AddScope(compound_stmt, "main");
            symbol_scope_ = "global";
        }
            break;
        case 3: /* 3. <��������б�> -> <�������> <��������б�> */
        {
            ASTNode * declaration_list = tree_node_.top();   // ��������б�
            tree_node_.pop();

            ASTNode * declaration  = tree_node_.top();   // �������
            tree_node_.pop();

            if (declaration_list)
                declaration_list->pre_ = declaration;
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
            ASTNode * func_def_list = tree_node_.top();  // ���������б�
            tree_node_.pop();

            ASTNode * func_def = tree_node_.top();   // ��������
            tree_node_.pop();

            if (func_def_list)
                func_def_list->pre_ = func_def;
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
            node->type_ = "��������";

            ASTNode * compound_stmt = tree_node_.top();
            tree_node_.pop();

            ASTNode * func_def_args_list = tree_node_.top();
            tree_node_.pop();
            SetAttribute(func_def_args_list);

            ASTNode * func_name = tree_node_.top();
            tree_node_.pop();
            func_name->symbol_ = type_system_.GetSymbolByText(func_name->content_);

            ASTNode * type = tree_node_.top();
            tree_node_.pop();

            compound_stmt->pre_ = func_def_args_list;
            if (func_def_args_list)
                func_def_args_list->pre_ = func_name;
            func_name->pre_ = type;
            type->next_ = func_name;
            if (func_def_args_list)
            {
                func_name->next_ = func_def_args_list;
                func_def_args_list->next_ = compound_stmt;
            }
            else
            {
                func_name->next_ = compound_stmt;
            }

            node->children_.emplace_front(compound_stmt);
            node->children_.emplace_front(func_def_args_list);
            node->children_.emplace_front(func_name);
            node->children_.emplace_front(type);

            // ����ĸ������ڵ�Ҫ��ӵ����ű��ж�Ӧ������
            Symbol * symbol = type_system_.GetSymbolByText(func_name->content_);
            if (symbol == nullptr)
            {
                // δ�������ĺ���
                Symbol * att_list;
                if (func_def_args_list)
                {
                    att_list = (Symbol *)attribution_stack_.top();
                    attribution_stack_.pop();
                }
                auto * att_name = type_system_.NewSymbol(func_name->content_, symbol_scope_);
                auto * att_type = (TypeLink *)attribution_stack_.top();
                attribution_stack_.pop();

                type_system_.AddSpecifierToDeclaration(att_type, att_name);
                type_system_.AddDeclarator(att_name, Declarator::FUNCTION);
                if (func_def_args_list)
                {
                    att_name->args_ = att_list;
                }
                else
                    att_list = nullptr;

                type_system_.AddSymbolsToTable(att_name, symbol_scope_);
                symbol = type_system_.GetSymbolByText(func_name->content_);
            }
            else
            {
                attribution_stack_.pop();
            }

            symbol->SetFuncBody(compound_stmt);

            symbol_scope_ = "global";
        }
            break;
        case 8: /* 8. <���������β��б�> -> <���������β�> */
        {
            node->type_ = "���������β��б�";

            tmp = tree_node_.top();
            tree_node_.pop();

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
            ASTNode * var = tree_node_.top();    // ����
            tree_node_.pop();

            ASTNode * type = tree_node_.top();   // ��������
            tree_node_.pop();

            type->children_.emplace_front(var);

            node = type;

            auto * specifier = (TypeLink *)attribution_stack_.top();
            attribution_stack_.pop();

            auto * symbol = type_system_.NewSymbol(var->content_, symbol_scope_);
            symbol->level_ = nesting_level_;
            type_system_.AddSpecifierToDeclaration(specifier, symbol);

            attribution_stack_.emplace(symbol);
        }
            break;
        case 11: /* 11. <���������β�> -> <��������> <����> , <���������β�> */
        {
            ASTNode * func_args = tree_node_.top();  // ���������β�
            tree_node_.pop();

            ASTNode * var = tree_node_.top();    // ����
            tree_node_.pop();

            ASTNode * type = tree_node_.top();   // ��������
            tree_node_.pop();

            type->children_.emplace_front(var);

            if (func_args)
                func_args->pre_ = type;
            type->next_ = func_args;

            node = type;

            auto * args = (Symbol *)attribution_stack_.top();
            attribution_stack_.pop();

            auto * symbol = type_system_.NewSymbol(var->content_, symbol_scope_);
            symbol->level_ = nesting_level_;
            auto * specifier = (TypeLink *)attribution_stack_.top();
            attribution_stack_.pop();
            type_system_.AddSpecifierToDeclaration(specifier, symbol);

            symbol->SetNextSymbol(args);
            attribution_stack_.emplace(symbol);
        }
            break;
        case 12: /* 12. <�������> -> { <����> } */
        {
            node->type_ = "�������";

            ASTNode * stmt_table = tree_node_.top();
            tree_node_.pop();

            while (stmt_table)
            {
                node->children_.emplace_back(stmt_table);
                stmt_table = stmt_table->next_;
            }
        }
            break;
        case 13: /* 13. <����> -> <���> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 14: /* 14. <����> -> <���> <����> */
        {
            ASTNode * stmt_table = tree_node_.top();
            tree_node_.pop();

            ASTNode * stmt = tree_node_.top();
            tree_node_.pop();

            stmt_table->pre_ = stmt;
            stmt->next_ = stmt_table;

            node = stmt;
        }
            break;
        case 15: /* 15. <ִ�����> -> <���ݴ������>  */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 16: /* 16. <ִ�����> -> <�������> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 17: /* 17. <���ݴ������> -> <���ʽ> ; */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 18: /* 18. <�������> -> <if���> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 19: /* 19. <�������> -> <for���> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 20: /* 20. <�������> -> <while���> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 21: /* 21. <�������> -> <do-while���> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 22: /* 22. <�������> -> <return���> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 23: /* 23. <if���> -> if ( <���ʽ> ) <ѭ�����> <if-tail> */
        {
            node->type_ = "if���";

            ASTNode * tail = tree_node_.top();
            tree_node_.pop();

            ASTNode * stmt = tree_node_.top();
            tree_node_.pop();

            ASTNode * exp = tree_node_.top();
            tree_node_.pop();

            if (tail)
                tail->pre_ = stmt;
            stmt->pre_ = exp;
            exp->next_ = stmt;
            stmt->next_ = tail;

            node->children_.emplace_front(tail);
            node->children_.emplace_front(stmt);
            node->children_.emplace_front(exp);
        }
            break;
        case 24: /* 24. <if-tail> -> else <ѭ�����> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
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
            node->type_ = "for���";

            ASTNode * stmt = tree_node_.top();
            tree_node_.pop();

            ASTNode * exp1 = tree_node_.top();
            tree_node_.pop();

            ASTNode * exp2 = tree_node_.top();
            tree_node_.pop();

            ASTNode * exp3 = tree_node_.top();
            tree_node_.pop();

            stmt->pre_ = exp1;
            exp1->pre_ = exp2;
            exp2->pre_ = exp3;
            exp3->next_ = exp2;
            exp2->next_ = exp1;
            exp1->next_ = stmt;

            node->children_.emplace_front(stmt);
            node->children_.emplace_front(exp1);
            node->children_.emplace_front(exp2);
            node->children_.emplace_front(exp3);
        }
            break;
        case 27: /* 27. <while���> -> while ( <���ʽ> ) <ѭ�����> */
        {
            node->type_ = "while���";

            ASTNode * stmt = tree_node_.top();
            tree_node_.pop();

            ASTNode * exp = tree_node_.top();
            tree_node_.pop();

            stmt->pre_ = exp;
            exp->next_ = stmt;
            node->children_.emplace_front(stmt);
            node->children_.emplace_front(exp);
        }
            break;
        case 28: /* 28. <do-while���> -> do <ѭ���ø������> while ( <���ʽ> ) ; */
        {
            node->type_ = "do-while���";

            ASTNode * exp = tree_node_.top();
            tree_node_.pop();

            ASTNode * compound_stmt = tree_node_.top();
            tree_node_.pop();

            exp->pre_ = compound_stmt;
            compound_stmt->next_ = exp;
            node->children_.emplace_front(exp);
            node->children_.emplace_front(compound_stmt);
        }
            break;
        case 29: /* 29. <ѭ�����> -> <�������> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 30: /* 30. <ѭ�����> -> <ѭ��ִ�����> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 31: /* 31. <ѭ�����> -> <ѭ���ø������> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 32: /* 32. <ѭ���ø������> -> { <ѭ������> } */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 33: /* 33. <ѭ������> -> <ѭ�����> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 34: /* 34. <ѭ������> -> <ѭ�����> <ѭ������> */
        {
            node->type_ = "ѭ������";

            ASTNode * table = tree_node_.top();
            tree_node_.pop();

            ASTNode * stmt = tree_node_.top();
            tree_node_.pop();

            table->pre_ = stmt;
            stmt->next_ =  table;
            node->children_.emplace_front(table);
            node->children_.emplace_front(stmt);
        }
            break;
        case 35: /* 35. <ѭ��ִ�����> -> <if���> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 36: /* 36. <ѭ��ִ�����> -> <for���> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 37: /* 37. <ѭ��ִ�����> -> <while���> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 38: /* 38. <ѭ��ִ�����> -> <do-while���> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 39: /* 39. <ѭ��ִ�����> -> <return���> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 40: /* 40. <ѭ��ִ�����> -> <break���> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 41: /* 41. <ѭ��ִ�����> -> <continue���> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 42: /* 42. <ѭ��ִ�����> -> <���ݴ������> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 43: /* 43. <return���> -> return ; */
        {
            node->type_ = "return���";
        }
            break;
        case 44: /* 44. <return���> -> return <���ʽ> ; */
        {
            node->type_ = "return���";

            ASTNode * exp = tree_node_.top();
            tree_node_.pop();
            node->children_.emplace_front(exp);
        }
            break;
        case 45: /* 45. <break���> -> break ; */
        {
            node->type_ = "break���";
        }
            break;
        case 46: /* 46. <continue���> -> continue ; */
        {
            node->type_ = "continue���";
        }
            break;
        case 47: /* 47. <���> -> <�������> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 48: /* 48. <���> -> <ִ�����> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 49: /* 49. <�������> -> <ֵ����> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 50: /* 50. <�������> -> <��������> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 51: /* 51. <ֵ����> -> <��������> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 52: /* 52. <ֵ����> -> <��������> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 53: /* 53. <��������> -> const <��������> <����������> */
        {
            node->type_ = "��������";

            ASTNode * table = tree_node_.top();  // ����������
            tree_node_.pop();

            ASTNode * type = tree_node_.top();   // ��������
            tree_node_.pop();

            while (table)
            {
                type->children_.emplace_front(table);
                table = table->next_;
            }

            node->children_.emplace_front(type);

            Specifier * last = type_system_.NewType("const")->GetTypeObject();
            Specifier * dst = ((TypeLink *)attribution_stack_.top())->GetTypeObject();  // ��������
            type_system_.SpecifierCpy(dst, last);
        }
            break;
        case 54: /* 54. <��������> -> int */
        {
            node->type_ = "int";

            attribute_node_ = type_system_.NewType("int");
            attribution_stack_.emplace(attribute_node_);
        }
            break;
        case 55: /* 55. <��������> -> float */
        {
            node->type_ = "float";

            attribute_node_ = type_system_.NewType("float");
            attribution_stack_.emplace(attribute_node_);
        }
            break;
        case 56: /* 56. <��������> -> char */
        {
            node->type_ = "char";

            attribute_node_ = type_system_.NewType("char");
            attribution_stack_.emplace(attribute_node_);
        }
            break;
        case 57: /* 57. <��������> -> String */
        {
            node->type_ = "String";

            attribute_node_ = type_system_.NewType("String");
            attribution_stack_.emplace(attribute_node_);
        }
            break;
        case 58: /* 58. <����������> -> <����> = <����> ; */
        {
            node->type_ = "INIT";

            ASTNode * const_num = tree_node_.top();
            tree_node_.pop();

            ASTNode * const_var = tree_node_.top();
            tree_node_.pop();

            const_num->pre_ = const_var;
            const_var->next_ = const_num;

            node->children_.emplace_front(const_num);
            node->children_.emplace_front(const_var);
        }
            break;
        case 59: /* 59. <����������> -> <����> = <����> , <����������> */
        {
            ASTNode * table = tree_node_.top();  // ����������
            tree_node_.pop();

            auto * new_table = new ASTNode();
            new_table->type_ = "INIT";
            ASTNode * const_num = tree_node_.top();   // ����
            tree_node_.pop();
            ASTNode * const_var = tree_node_.top();  // ����
            tree_node_.pop();

            const_num->pre_ = const_var;
            const_var->next_ = const_num;
            new_table->children_.emplace_front(const_num);
            new_table->children_.emplace_front(const_var);

            table->pre_ = new_table;
            new_table->next_ = table;

            node = new_table;
        }
            break;
        case 60: /* 60. <��������> -> <��������> <����������> */
        {
            auto * symbol = (Symbol *)attribution_stack_.top();
            attribution_stack_.pop();
            auto * specifier = (TypeLink *)attribution_stack_.top();
            attribution_stack_.pop();

            type_system_.AddSpecifierToDeclaration(specifier, symbol);
            type_system_.AddSymbolsToTable(symbol, symbol_scope_);

            node->type_ = "��������";

            ASTNode * table = tree_node_.top();  // ����������
            tree_node_.pop();

            ASTNode * type = tree_node_.top();   // ��������
            tree_node_.pop();

            while (table)
            {
                type->children_.emplace_back(table);
                table = table->next_;
            }

            node->children_.emplace_front(type);

            SetAttribute(node);
        }
            break;
        case 61: /* 61. <����������> -> <����������> ; */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;

            auto * cur_symbol = (Symbol *)attribution_stack_.top();
            cur_symbol->level_ = nesting_level_;
        }
            break;
        case 62: /* 62. <����������> -> <����������> , <����������> */
        {
            ASTNode * table = tree_node_.top();  // ����������
            tree_node_.pop();

            ASTNode * single_decl = tree_node_.top();  // ����������
            tree_node_.pop();

            table->pre_ = single_decl;
            single_decl->next_ = table;

            node = single_decl;

            auto * cur_symbol = (Symbol *)attribution_stack_.top();
            cur_symbol->level_ = nesting_level_;
            attribution_stack_.pop();
            auto * last_symbol = (Symbol *)attribution_stack_.top();
            last_symbol->level_ = nesting_level_;
            attribution_stack_.pop();
            last_symbol->SetNextSymbol(cur_symbol);
            attribution_stack_.emplace(last_symbol);
        }
            break;
        case 63: /* 63. <����������> -> <����> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;

            attribute_node_ = type_system_.NewSymbol(tmp->content_, symbol_scope_);
            attribution_stack_.emplace(attribute_node_);
        }
            break;
        case 64: /* 64. <����������> -> <����> = <�������ʽ> */
        {
            node->type_ = "INIT";

            ASTNode * bool_exp = tree_node_.top();
            tree_node_.pop();

            ASTNode * var = tree_node_.top();
            tree_node_.pop();

            bool_exp->pre_ = var;
            var->next_ = bool_exp;
            node->children_.emplace_front(bool_exp);
            node->children_.emplace_front(var);

            attribute_node_ = type_system_.NewSymbol(var->content_, symbol_scope_);
            attribution_stack_.emplace(attribute_node_);
        }
            break;
        case 65: /* 65. <��������> -> <��������> <������> ( <���������β��б�> ) ; */
        {
            node->type_ = "��������";

            ASTNode * func_args_list = tree_node_.top();
            tree_node_.pop();

            ASTNode * func_name = tree_node_.top();
            tree_node_.pop();

            ASTNode * type = tree_node_.top();
            tree_node_.pop();

            if (func_args_list)
            {
                func_args_list->pre_ = func_name;
                type->next_ = func_args_list;
                node->children_.emplace_front(func_args_list);
            }
            type->children_.emplace_front(func_name);
            node->children_.emplace_front(type);

            Symbol * att_list;
            if (func_args_list)
            {
                att_list = (Symbol *)attribution_stack_.top();
                attribution_stack_.pop();
            }
            auto * att_name = type_system_.NewSymbol(func_name->content_, symbol_scope_);
            auto * att_type = (TypeLink *)attribution_stack_.top();
            attribution_stack_.pop();

            type_system_.AddSpecifierToDeclaration(att_type, att_name);
            type_system_.AddDeclarator(att_name, Declarator::FUNCTION);
            if (func_args_list)
            {
                att_name->args_ = att_list;
            }
            else
                att_list = nullptr;

            type_system_.AddSymbolsToTable(att_name, symbol_scope_);
            symbol_scope_ = "global";
        }
            break;
        case 66: /* 66. <��������> -> <��������> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 67: /* 67. <��������> -> void*/
        {
            node->type_ = "void";

            symbol_scope_ = "function_scope";
            attribute_node_ = type_system_.NewType("void");
            attribution_stack_.emplace(attribute_node_);
        }
            break;
        case 68: /* 68. <���������β��б�> -> <���������β�> */
        {
            node->type_ = "���������β��б�";

            ASTNode * func_args = tree_node_.top();
            tree_node_.pop();

            while (func_args)
            {
                node->children_.emplace_back(func_args);
                func_args = func_args->next_;
            }

            auto * symbol = (Symbol *)attribution_stack_.top();
            type_system_.AddSymbolsToTable(symbol, symbol_scope_);
        }
            break;
        case 69: /* 69. <���������β��б�> -> <epsilon> */
        {
            return nullptr;
        }
            break;
        case 70: /* 70. <���������β�> -> <��������> <����> */
        {
            ASTNode * var = tree_node_.top();    // ����
            tree_node_.pop();

            ASTNode * type = tree_node_.top();   // ��������
            tree_node_.pop();

            type->children_.emplace_front(var);
            node = type;

            auto * symbol = type_system_.NewSymbol(var->content_, symbol_scope_);
            symbol->level_ = nesting_level_;
            auto * specifier = (TypeLink *)attribution_stack_.top();
            attribution_stack_.pop();
            type_system_.AddSpecifierToDeclaration(specifier, symbol);

            attribution_stack_.emplace(symbol);
        }
            break;
        case 71: /* 71. <���������β�> -> <��������> <����> , <���������β�> */
        {
            ASTNode * next_func_args = tree_node_.top();  // ���������β�
            tree_node_.pop();

            ASTNode * var = tree_node_.top();    // ����
            tree_node_.pop();

            ASTNode * type = tree_node_.top();   // ��������
            tree_node_.pop();

            type->children_.emplace_front(var);

            next_func_args->pre_ = type;
            type->next_ = next_func_args;
            node = type;

            auto * args = (Symbol *)attribution_stack_.top();
            attribution_stack_.pop();

            auto * symbol = type_system_.NewSymbol(var->content_, symbol_scope_);
            symbol->level_ = nesting_level_;
            auto * specifier = (TypeLink *)attribution_stack_.top();
            attribution_stack_.pop();
            type_system_.AddSpecifierToDeclaration(specifier, symbol);

            symbol->SetNextSymbol(args);
            attribution_stack_.emplace(symbol);
        }
            break;
        case 72: /* 72. <���ʽ> -> <��ֵ���ʽ> */
        {
            node->type_ = "���ʽ";

            tmp = tree_node_.top();
            tree_node_.pop();
            node->children_.emplace_front(tmp);

            SetAttribute(node);
        }
            break;
        case 73: /* 73. <���ʽ> -> <�򵥱��ʽ> */
        {
            node->type_ = "���ʽ";

            tmp = tree_node_.top();
            tree_node_.pop();
            node->children_.emplace_front(tmp);

            SetAttribute(node);
        }
            break;
        case 74: /* 74. <�򵥱��ʽ> -> <�������ʽ> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 75: /* 75. <��ֵ���ʽ> -> <����> = <�������ʽ> */
        {
            node->content_ = "=";
            node->type_ = "AOP";

            ASTNode * bool_exp = tree_node_.top();
            tree_node_.pop();

            ASTNode * var = tree_node_.top();
            tree_node_.pop();

            bool_exp->pre_ = var;
            var->next_ = bool_exp;

            node->children_.emplace_front(bool_exp);
            node->children_.emplace_front(var);
        }
            break;
        case 76: /* 76. <�������ʽ> -> <�������ʽ> || <������> */
        {
            node->content_ = "||";
            node->type_ = "BOP";

            ASTNode * bool_term = tree_node_.top();
            tree_node_.pop();

            ASTNode * bool_exp = tree_node_.top();
            tree_node_.pop();

            bool_term->pre_ = bool_exp;
            bool_exp->next_ = bool_term;

            node->children_.emplace_front(bool_term);
            node->children_.emplace_front(bool_exp);
        }
            break;
        case 77: /* 77. <�������ʽ> -> <������> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 78: /* 78. <������> -> <������> && <��������> */
        {
            node->content_ = "&&";
            node->type_ = "BOP";

            ASTNode * bool_factor = tree_node_.top();
            tree_node_.pop();

            ASTNode * bool_term = tree_node_.top();
            tree_node_.pop();

            bool_factor->pre_ = bool_term;
            bool_term->next_ = bool_factor;

            node->children_.emplace_front(bool_factor);
            node->children_.emplace_front(bool_term);
        }
            break;
        case 79: /* 79. <������> -> <��������> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 80: /* 80. <��������> -> ! <��������> */
        {
            node->content_ = "!";
            node->type_ = "BOP";

            ASTNode * bool_factor = tree_node_.top();
            tree_node_.pop();
            node->children_.emplace_front(bool_factor);
        }
            break;
        case 81: /* 81. <��������> -> <��ϵ���ʽ> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 82: /* 82. <��ϵ���ʽ> -> <�������ʽ> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 83: /* 83. <��ϵ���ʽ> -> <�������ʽ> <��ϵ�����> <�������ʽ> */
        {
            ASTNode * arith_exp1 = tree_node_.top();
            tree_node_.pop();

            ASTNode * rel_op = tree_node_.top();
            tree_node_.pop();

            ASTNode * arith_exp2 = tree_node_.top();
            tree_node_.pop();

            arith_exp2->pre_ = arith_exp1;
            arith_exp1->next_ = arith_exp2;
            rel_op->children_.emplace_front(arith_exp1);
            rel_op->children_.emplace_front(arith_exp2);

            node = rel_op;
        }
            break;
        case 84: /* 84. <��ϵ�����> -> > */
        {
            node->content_ = ">";
            node->type_ = "COP";
        }
            break;
        case 85: /* 85. <��ϵ�����> -> < */
        {
            node->content_ = "<";
            node->type_ = "COP";
        }
            break;
        case 86: /* 86. <��ϵ�����> -> >= */
        {
            node->content_ = ">=";
            node->type_ = "COP";
        }
            break;
        case 87: /* 87. <��ϵ�����> -> <= */
        {
            node->content_ = "<=";
            node->type_ = "COP";
        }
            break;
        case 88: /* 88. <��ϵ�����> -> == */
        {
            node->content_ = "==";
            node->type_ = "COP";
        }
            break;
        case 89: /* 89. <��ϵ�����> -> != */
        {
            node->content_ = "!=";
            node->type_ = "COP";
        }
            break;
        case 90: /* 90. <�������ʽ> -> <��> + <�������ʽ> */
        {
            node->content_ = "+";
            node->type_ = "OOP";

            ASTNode * term = tree_node_.top();
            tree_node_.pop();
            node->children_.emplace_front(term);

            ASTNode * factor = tree_node_.top();
            tree_node_.pop();
            node->children_.emplace_front(factor);

            term->pre_ = factor;
            factor->next_ = term;
        }
            break;
        case 91: /* 91. <�������ʽ> -> <��> - <�������ʽ> */
        {
            node->content_ = "-";
            node->type_ = "OOP";

            ASTNode * term = tree_node_.top();
            tree_node_.pop();
            node->children_.emplace_front(term);

            ASTNode * factor = tree_node_.top();
            tree_node_.pop();
            node->children_.emplace_front(factor);

            term->pre_ = factor;
            factor->next_ = term;
        }
            break;
        case 92: /* 92. <�������ʽ> -> <��> */
        {
            tmp = tree_node_.top();
            tree_node_.pop();
            node = tmp;
        }
            break;
        case 93: /* 93. <��> -> <����> * <��> */
        {
            node->content_ = "*";
            node->type_ = "OOP";

            ASTNode * term = tree_node_.top();
            tree_node_.pop();
            node->children_.emplace_front(term);

            ASTNode * factor = tree_node_.top();
            tree_node_.pop();
            node->children_.emplace_front(factor);

            term->pre_ = factor;
            factor->next_ = term;
        }
            break;
        case 94: /* 94. <��> -> <����> / <��> */
        {
            node->content_ = "/";
            node->type_ = "OOP";

            ASTNode * term = tree_node_.top();
            tree_node_.pop();
            node->children_.emplace_front(term);

            ASTNode * factor = tree_node_.top();
            tree_node_.pop();
            node->children_.emplace_front(factor);

            term->pre_ = factor;
            factor->next_ = term;
        }
            break;
        case 95: /* 95. <��> -> <����> % <��> */
        {
            node->content_ = "%";
            node->type_ = "OOP";

            ASTNode * term = tree_node_.top();
            tree_node_.pop();
            node->children_.emplace_front(term);

            ASTNode * factor = tree_node_.top();
            tree_node_.pop();
            node->children_.emplace_front(factor);

            term->pre_ = factor;
            factor->next_ = term;
        }
            break;
        case 96: /* 96. <��> -> <����> */
        {
            ASTNode * factor = tree_node_.top();
            tree_node_.pop();
            node = factor;
        }
            break;
        case 97: /* 97. <����> -> ( <�������ʽ> ) */
        {
            ASTNode * arith_exp = tree_node_.top();
            tree_node_.pop();
            node = arith_exp;
        }
            break;
        case 98: /* 98. <����> -> <����> */
        {
            ASTNode * const_num = tree_node_.top();
            tree_node_.pop();
            node = const_num;
        }
            break;
        case 99: /* 99. <����> -> <�ַ�����> */
        {
            ASTNode * cconst = tree_node_.top();
            tree_node_.pop();
            node = cconst;
        }
            break;
        case 100: /* 100. <����> -> <�ַ�������> */
        {
            ASTNode * sconst = tree_node_.top();
            tree_node_.pop();
            node = sconst;
        }
            break;
        case 101: /* 101. <����> -> <����> */
        {
            ASTNode * var = tree_node_.top();
            tree_node_.pop();
            node = var;
        }
            break;
        case 102: /* 102. <����> -> <��������> */
        {
            ASTNode * func_call = tree_node_.top();
            tree_node_.pop();

            node = func_call;
        }
            break;
        case 103: /* 103. <��������>  -> <������> ( <ʵ���б�> ) */
        {
            node->type_ = "��������";

            ASTNode * actual_args = tree_node_.top();
            tree_node_.pop();

            while (actual_args)
            {
                node->children_.emplace_back(actual_args);
                actual_args = actual_args->next_;
            }

            ASTNode * func_name = tree_node_.top();
            tree_node_.pop();

            if (actual_args)
                actual_args->pre_ = func_name;
            func_name->next_ = actual_args;
            node->children_.emplace_front(func_name);
        }
            break;
        case 104: /* 104. <ʵ���б�> -> <���ʽ> */
        {
            ASTNode * exp = tree_node_.top();
            tree_node_.pop();

            node = exp;
        }
            break;
        case 105: /* 105. <ʵ���б�> -> <���ʽ> , <ʵ���б�> */
        {
            ASTNode * actual_args = tree_node_.top();
            tree_node_.pop();

            ASTNode * exp = tree_node_.top();
            tree_node_.pop();

            actual_args->pre_ = exp;
            exp->next_ = actual_args;

            node = exp;
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

ASTTree *LR1Parser::GetAST()
{
    return tree;
}

/*!
 * �������еĸ���Ҷ�ӽڵ�����ű��������
 * ͬʱ�����������ֵ����ڵ���
 * @param node
 */
void LR1Parser::SetAttribute(ASTNode *node)
{
    while (node)
    {
        for (auto& child : node->children_)
        {
            if (child->IsLeaf())
            {
                if (child->type_ == "<������>" || child->type_ == "<����>" || child->type_ == "<����>")
                {
                    // ���ú���
                    if (CLibCall::GetInstance()->IsApiCall(child->content_))
                        continue;

                    child->symbol_ = type_system_.GetSymbolByText(child->content_);
                    if (child->symbol_ != nullptr)
                    {
                        for (auto sym : type_system_.symbol_table_.find(child->content_)->second)
                            if (sym->symbol_scope_ == symbol_scope_)
                            {
                                child->symbol_ = sym;
                                break;
                            }
                    }
                }
                else if (child->type_ == "<����>")
                {
                    if (child->content_.find('.') != std::string::npos) // ������
                    {
                        child->is_int_ = false;
                        child->f_value_ = std::stof(child->content_, nullptr);
                    }
                    else // ����
                    {
                        child->is_int_ = true;

                        if (child->content_ == "0")
                        {
                            child->i_value_ = 0;
                            continue;
                        }
                        else if (child->content_.at(0) == '0')
                        {
                            if (child->content_.at(1) == 'x' || child->content_.at(1) == 'X')
                                child->i_value_ = std::stoi(child->content_, nullptr, 16);
                            else if (child->content_.at(1) == 'b' || child->content_.at(1) == 'B')
                                child->i_value_ = std::stoi(child->content_, nullptr, 2);
                            else
                                child->i_value_ = std::stoi(child->content_, nullptr, 8);
                        }
                        else
                        {
                            child->i_value_ = std::stoi(child->content_, nullptr, 10);
                        }
                    }
                }
                else if (child->type_ == "<�ַ�����>")
                    child->c_value_ = child->content_.at(1);
                else if (child->type_ == "<�ַ�������>")
                    child->s_value_ = child->content_;
            }
            else
                SetAttribute(child);
        }
        node = node->next_;
    }
}

void LR1Parser::AddScope(ASTNode * node, const std::string& func_name)
{
    for (auto child : node->GetChildrenList())
    {
        // ��ʱ��δɾ���պ��ӽڵ�
        if (child == nullptr)
            break;

        if (child->IsLeaf())
        {
            if (child->type_ == "<����>" || child->type_ == "<����>")
                child->GetSymbol()->SetScope(func_name);
        }
        else
        {
            AddScope(child, func_name);
        }
    }
}

void LR1Parser::GenerateAsm(ASTNode * node)
{
    if (node == nullptr)
        return;

    std::string type = node->type_;
    if (type == "if���")
    {
        ProgramGenerator::GetInstance()->IncreaseBranch();
        ProgramGenerator::GetInstance()->if_stmt.emplace(ProgramGenerator::GetInstance()->branch_count);

        auto iter0 = node->GetChildrenList().begin();
        GenerateAsm(*iter0);

        auto iter1 = node->GetChildrenList().begin();
        std::advance(iter1, 1);
        GenerateAsm(*iter1);

        if (node->GetChildrenList().size() > 2) // ����else��֧
        {
            ProgramGenerator::EmitString(Instruction::GOTO.ToString() + " " + ProgramGenerator::GetInstance()->GetBranchOut() + "\n");
            ProgramGenerator::EmitString(ProgramGenerator::GetInstance()->GetBranch() + ":\n");

            auto iter2 = node->GetChildrenList().begin();
            std::advance(iter2, 2);
            // ִ��else��֧
            GenerateAsm(*iter2);
        }

        ProgramGenerator::GetInstance()->EmitBranchOut();
        ProgramGenerator::GetInstance()->if_stmt.pop();
    }
    else if (type == "for���")
    {
        auto iter0 = node->GetChildrenList().begin();
        GenerateAsm(*iter0);    // ִ�и�ֵ���

        ProgramGenerator::GetInstance()->IncreaseLoop();
        ProgramGenerator::GetInstance()->loop_stmt.emplace(ProgramGenerator::GetInstance()->loop_count);
        ProgramGenerator::GetInstance()->IncreaseBranch();
        ProgramGenerator::GetInstance()->if_stmt.emplace(ProgramGenerator::GetInstance()->branch_count);
        ProgramGenerator::EmitString("\n" +ProgramGenerator::GetInstance()->GetLoop() + ":\n");

        auto iter1 = node->GetChildrenList().begin();
        std::advance(iter1, 1);
        GenerateAsm(*iter1);    // ִ�бȽ�

        auto iter3 = node->GetChildrenList().begin();
        std::advance(iter3, 3);
        GenerateAsm(*iter3);

        auto iter2 = node->GetChildrenList().begin();
        std::advance(iter2, 2);
        GenerateAsm(*iter2);

        ProgramGenerator::EmitString(Instruction::GOTO.ToString() + " " + ProgramGenerator::GetInstance()->GetLoop() + "\n\n");
        ProgramGenerator::EmitString(ProgramGenerator::GetInstance()->GetBranch() + ":\n");
        ProgramGenerator::GetInstance()->loop_stmt.pop();
        ProgramGenerator::GetInstance()->if_stmt.pop();
    }
    /* while ���� do-while ��������ȫһ�� */
    else if (type == "while���")
    {
        ProgramGenerator::GetInstance()->IncreaseLoop();
        ProgramGenerator::GetInstance()->loop_stmt.emplace(ProgramGenerator::GetInstance()->loop_count);
        ProgramGenerator::GetInstance()->IncreaseBranch();
        ProgramGenerator::GetInstance()->if_stmt.emplace(ProgramGenerator::GetInstance()->branch_count);
        ProgramGenerator::EmitString("\n" +ProgramGenerator::GetInstance()->GetLoop() + ":\n");

        auto iter0 = node->GetChildrenList().begin();
        GenerateAsm(*iter0);    // ִ�бȽ�

        auto iter1 = node->GetChildrenList().begin();
        std::advance(iter1, 1);
        GenerateAsm(*iter1);    // ִ���ڲ����

        ProgramGenerator::EmitString(Instruction::GOTO.ToString() + " " + ProgramGenerator::GetInstance()->GetLoop() + "\n\n");
        ProgramGenerator::EmitString(ProgramGenerator::GetInstance()->GetBranch() + ":\n");
        ProgramGenerator::GetInstance()->loop_stmt.pop();
        ProgramGenerator::GetInstance()->if_stmt.pop();
    }
    else if (type == "do-while���")
    {
        ProgramGenerator::GetInstance()->IncreaseLoop();
        ProgramGenerator::GetInstance()->loop_stmt.emplace(ProgramGenerator::GetInstance()->loop_count);
        ProgramGenerator::GetInstance()->IncreaseBranch();
        ProgramGenerator::GetInstance()->if_stmt.emplace(ProgramGenerator::GetInstance()->branch_count);
        ProgramGenerator::EmitString("\n" +ProgramGenerator::GetInstance()->GetLoop() + ":\n");

        auto iter0 = node->GetChildrenList().begin();
        GenerateAsm(*iter0);    // ִ���ڲ����

        auto iter1 = node->GetChildrenList().begin();
        std::advance(iter1, 1);
        GenerateAsm(*iter1);    // ִ�бȽ�

        ProgramGenerator::EmitString(Instruction::GOTO.ToString() + " " + ProgramGenerator::GetInstance()->GetLoop() + "\n\n");
        ProgramGenerator::EmitString(ProgramGenerator::GetInstance()->GetBranch() + ":\n");
        ProgramGenerator::GetInstance()->loop_stmt.pop();
        ProgramGenerator::GetInstance()->if_stmt.pop();
    }
    else if (type == "INIT")
    {
        ASTNode * lhs = node->GetChildrenList().front();
        ASTNode * rhs = node->GetChildrenList().back();

        GenerateAsm(rhs);

        int idx;
        idx = GetLocalVariableIndex(lhs->GetSymbol());
        if (rhs->type_ == "<����>")
        {
            idx = GetLocalVariableIndex(rhs->GetSymbol());
            if (idx != -1)
                ProgramGenerator::Emit(Instruction::ILOAD, std::to_string(idx));
            else
                ProgramGenerator::Emit(Instruction::GETSTATIC, ProgramGenerator::program_name  + "/" + rhs->content_ + " I");
        }
        else if (rhs->type_ == "<����>")
        {
            ProgramGenerator::Emit(Instruction::SIPUSH, std::to_string(rhs->i_value_));
        }
        // �������÷���ֵ��OOP��������Ѿ���ջ������������

        if (idx != -1)
            ProgramGenerator::Emit(Instruction::ISTORE, std::to_string(idx));
        else
            ProgramGenerator::Emit(Instruction::PUTSTATIC, ProgramGenerator::program_name  + "/" + lhs->content_ + " I");
    }
    else if (type == "��������")
    {
        // ���ú����ĵ���ʵ��
        if (CLibCall::GetInstance()->IsApiCall(node->GetChildrenList().front()->content_))
        {
            std::list<ASTNode *> args;
            auto iter = node->GetChildrenList().begin();
            std::advance(iter, 1);
            size_t idx = 1;
            while (idx++ < node->GetChildrenSize())
            {
                args.emplace_back((*iter)->GetChildrenList().front());
                std::advance(iter, 1);
            }

            std::string arg_str = args.front()->s_value_;
            arg_str.erase(std::find(arg_str.begin(), arg_str.end(), '"'));
            arg_str.erase(std::find(arg_str.begin(), arg_str.end(), '"'));
            args.pop_front();
            std::string format_str;

            int i = 0;
            while (i < arg_str.length())
            {
                if (arg_str.at(i) == '%' && (i+1) < arg_str.length()
                    && arg_str.at(i+1) == 'd')
                {
                    ProgramGenerator::Emit(Instruction::GETSTATIC, "java/lang/System/out Ljava/io/PrintStream;");
                    ProgramGenerator::Emit(Instruction::LDC, "\"" + format_str + "\"");
                    ProgramGenerator::Emit(Instruction::INVOKEVIRTUAL, "java/io/PrintStream/print(Ljava/lang/String;)V");

                    format_str.clear();
                    ASTNode * tmp = args.front();
                    args.pop_front();

                    if (tmp->type_ == "<����>")
                    {
                        ProgramGenerator::Emit(Instruction::GETSTATIC, "java/lang/System/out Ljava/io/PrintStream;");
                        idx = GetLocalVariableIndex(tmp->GetSymbol());
                        if (idx != -1)
                            ProgramGenerator::Emit(Instruction::ILOAD, std::to_string(idx));
                        else
                            ProgramGenerator::Emit(Instruction::GETSTATIC, ProgramGenerator::program_name  + "/" + tmp->content_ + " I");
                        std::string print_method = "java/io/PrintStream/print(I)V";
                        ProgramGenerator::Emit(Instruction::INVOKEVIRTUAL, print_method);
                    }
                    else if (tmp->type_ == "<����>")
                    {
                        ProgramGenerator::Emit(Instruction::GETSTATIC, "java/lang/System/out Ljava/io/PrintStream;");
                        ProgramGenerator::Emit(Instruction::SIPUSH, std::to_string(tmp->i_value_));
                        ProgramGenerator::Emit(Instruction::INVOKEVIRTUAL, "java/io/PrintStream/print(I)V");
                    }
                    else
                    {
                        ProgramGenerator::Emit(Instruction::GETSTATIC, "java/lang/System/out Ljava/io/PrintStream;");
                        GenerateAsm(tmp);
                        ProgramGenerator::Emit(Instruction::INVOKEVIRTUAL, "java/io/PrintStream/print(I)V");
                    }
                    i+=2;
                }
                else
                {
                    format_str.append(1, arg_str.at(i));
                    ++i;
                }
            }

            ProgramGenerator::Emit(Instruction::GETSTATIC, "java/lang/System/out Ljava/io/PrintStream;");
            ProgramGenerator::Emit(Instruction::LDC, "\"\n\"");
            ProgramGenerator::Emit(Instruction::INVOKEVIRTUAL, "java/io/PrintStream/print(Ljava/lang/String;)V");

            //CLibCall::GetInstance()->InvokeAPI(node->GetChildrenList().front()->content_, args);
        }
        else    // ��ͨ��������
        {
            Symbol * func_symbol = node->GetChildrenList().front()->GetSymbol();
            Symbol * func_args = func_symbol->args_;

            if (func_args == nullptr)   // �޲�����������
            {
                std::string declaration = func_symbol->name_ + EmitArgs(func_symbol);
                ProgramGenerator::Emit(Instruction::INVOKESTATIC, CodeGenerator::program_name + "/" + declaration);
                ProgramGenerator::EmitDirective(Directive::METHOD_PUBLIC_STATIC, declaration);
            }
            else    // �в�����������
            {
                auto tmp = func_args;
                auto iter = node->GetChildrenList().begin();
                while (tmp)
                {
                    std::advance(iter, 1);

                    GenerateAsm(*iter);

                    if ((*iter)->GetChildrenList().front()->type_ == "<����>")
                    {
                        int idx = GetLocalVariableIndex((*iter)->GetChildrenList().front()->GetSymbol());
                        if (idx != -1)
                            ProgramGenerator::Emit(Instruction::ILOAD, std::to_string(idx));
                        else    // ȫ�ֱ���
                            ProgramGenerator::Emit(Instruction::GETSTATIC, ProgramGenerator::program_name  + "/" + (*iter)->GetChildrenList().front()->content_ + " I");
                    }
                    else if ((*iter)->GetChildrenList().front()->type_ == "<����>")
                    {
                        ProgramGenerator::Emit(Instruction::SIPUSH, std::to_string((*iter)->GetChildrenList().front()->i_value_));
                    }
                    // �������ã�OOP�ķ���ֵ�Ѿ�ѹ���˶�ջ�У�AOP������Ϊʵ�δ���

                    tmp = tmp->GetNextSymbol();
                }

                std::string declaration = func_symbol->name_ + EmitArgs(func_symbol);
                ProgramGenerator::Emit(Instruction::INVOKESTATIC, CodeGenerator::program_name + "/" + declaration);
            }
        }
    }
    else if (type == "return���")
    {
        ASTNode * tmp = node->GetChildrenList().front();

        if (tmp != nullptr)
            GenerateAsm(tmp);

        tmp = tmp->GetChildrenList().front();

        if (tmp->type_ == "<����>")
        {
            int idx = GetLocalVariableIndex(tmp->GetSymbol());
            if (idx != -1)
                ProgramGenerator::Emit(Instruction::ILOAD, std::to_string(idx));
            else    // ȫ�ֱ���
                ProgramGenerator::Emit(Instruction::GETSTATIC, ProgramGenerator::program_name  + "/" + tmp->content_ + " I");
            EmitReturnInstruction(tmp->GetSymbol());
        }
        else if (tmp->type_ == "<����>")
        {
            ProgramGenerator::Emit(Instruction::SIPUSH, std::to_string(tmp->i_value_));
            EmitReturnInstruction(tmp->GetSymbol());
        }
        else if (tmp->type_ == "��������")
        {
            EmitReturnInstruction(tmp->GetChildrenList().front()->GetSymbol());
        }
        else if (tmp->type_ == "AOP")   // AOP������ջ��Ԫ�ظ����˱���������Ҫ���½�����ѹ��ջ��
        {
            int idx = GetLocalVariableIndex(tmp->GetChildrenList().front()->GetSymbol());
            if (idx != -1)
                ProgramGenerator::Emit(Instruction::ILOAD, std::to_string(idx));
            else
                ProgramGenerator::Emit(Instruction::GETSTATIC, ProgramGenerator::program_name  + "/" + tmp->GetChildrenList().front()->content_ + " I");
            EmitReturnInstruction(tmp->GetChildrenList().front()->GetSymbol());
        }
        else if (tmp->type_ == "OOP")
        {
            // OOP�����������Ѿ�������ջ����������Ҫ�鿴�����������ͣ��˴�δ���
            ProgramGenerator::Emit(Instruction::IRETURN);
        }
    }
    else
    {
        for (auto child : node->GetChildrenList())
            GenerateAsm(child);
    }

    if (type == "AOP")
    {
        ASTNode * lhs = node->GetChildrenList().front();
        ASTNode * rhs = node->GetChildrenList().back();

        int idx;
        idx = GetLocalVariableIndex(lhs->GetSymbol());
        if (rhs->type_ == "<����>")
        {
            idx = GetLocalVariableIndex(rhs->GetSymbol());
            if (idx != -1)
                ProgramGenerator::Emit(Instruction::ILOAD, std::to_string(idx));
            else    // ȫ�ֱ���
                ProgramGenerator::Emit(Instruction::GETSTATIC, ProgramGenerator::program_name  + "/" + rhs->content_ + " I");
        }
        else if (rhs->type_ == "<����>")
        {
            ProgramGenerator::Emit(Instruction::SIPUSH, std::to_string(rhs->i_value_));
        }
        // ���rhs��type_Ϊ������˵�������Ѿ���ջ������������

        if (idx != -1)
            ProgramGenerator::Emit(Instruction::ISTORE, std::to_string(idx));
        else
            ProgramGenerator::Emit(Instruction::PUTSTATIC, ProgramGenerator::program_name  + "/" + lhs->content_ + " I");
    }

    if (type == "OOP")
    {
        ASTNode * lhs = node->GetChildrenList().front();
        ASTNode * rhs = node->GetChildrenList().back();

        if (node->content_ == "+")
        {
            if (lhs->type_ == "<����>")
            {
                int idx = GetLocalVariableIndex(lhs->GetSymbol());
                if (idx != -1)
                    ProgramGenerator::Emit(Instruction::ILOAD, std::to_string(idx));
                else    // ȫ�ֱ���
                    ProgramGenerator::Emit(Instruction::GETSTATIC, ProgramGenerator::program_name  + "/" + lhs->content_ + " I");
            }
            else if (lhs->type_ == "<����>")
            {
                ProgramGenerator::Emit(Instruction::SIPUSH, std::to_string(lhs->i_value_));
            }

            if (rhs->type_ == "<����>")
            {
                int idx = GetLocalVariableIndex(rhs->GetSymbol());
                if (idx != -1)
                    ProgramGenerator::Emit(Instruction::ILOAD, std::to_string(idx));
                else    // ȫ�ֱ���
                    ProgramGenerator::Emit(Instruction::GETSTATIC, ProgramGenerator::program_name  + "/" + rhs->content_ + " I");
            }
            else if (rhs->type_ == "<����>")
            {
                ProgramGenerator::Emit(Instruction::SIPUSH, std::to_string(rhs->i_value_));
            }

            // �����lhs��rhs��type_��������˵�����������Ѿ������ջ������ȡ��ѹ��
            ProgramGenerator::Emit(Instruction::IADD);
        }
        if (node->content_ == "-")
        {
            if (lhs->type_ == "<����>")
            {
                int idx = GetLocalVariableIndex(lhs->GetSymbol());
                if (idx != -1)
                    ProgramGenerator::Emit(Instruction::ILOAD, std::to_string(idx));
                else    // ȫ�ֱ���
                    ProgramGenerator::Emit(Instruction::GETSTATIC, ProgramGenerator::program_name  + "/" + lhs->content_ + " I");
            }
            else if (lhs->type_ == "<����>")
            {
                ProgramGenerator::Emit(Instruction::SIPUSH, std::to_string(lhs->i_value_));
            }

            if (rhs->type_ == "<����>")
            {
                int idx = GetLocalVariableIndex(rhs->GetSymbol());
                if (idx != -1)
                    ProgramGenerator::Emit(Instruction::ILOAD, std::to_string(idx));
                else    // ȫ�ֱ���
                    ProgramGenerator::Emit(Instruction::GETSTATIC, ProgramGenerator::program_name  + "/" + rhs->content_ + " I");
            }
            else if (rhs->type_ == "<����>")
            {
                ProgramGenerator::Emit(Instruction::SIPUSH, std::to_string(rhs->i_value_));
            }

            // �����lhs��rhs��type_��������˵�����������Ѿ������ջ������ȡ��ѹ��
            ProgramGenerator::Emit(Instruction::ISUB);
        }
        if (node->content_ == "*")
        {
            if (lhs->type_ == "<����>")
            {
                int idx = GetLocalVariableIndex(lhs->GetSymbol());
                if (idx != -1)
                    ProgramGenerator::Emit(Instruction::ILOAD, std::to_string(idx));
                else    // ȫ�ֱ���
                    ProgramGenerator::Emit(Instruction::GETSTATIC, ProgramGenerator::program_name  + "/" + lhs->content_ + " I");
            }
            else if (lhs->type_ == "<����>")
            {
                ProgramGenerator::Emit(Instruction::SIPUSH, std::to_string(lhs->i_value_));
            }

            if (rhs->type_ == "<����>")
            {
                int idx = GetLocalVariableIndex(rhs->GetSymbol());
                if (idx != -1)
                    ProgramGenerator::Emit(Instruction::ILOAD, std::to_string(idx));
                else    // ȫ�ֱ���
                    ProgramGenerator::Emit(Instruction::GETSTATIC, ProgramGenerator::program_name  + "/" + rhs->content_ + " I");
            }
            else if (rhs->type_ == "<����>")
            {
                ProgramGenerator::Emit(Instruction::SIPUSH, std::to_string(rhs->i_value_));
            }

            // �����lhs��rhs��type_��������˵�����������Ѿ������ջ������ȡ��ѹ��
            ProgramGenerator::Emit(Instruction::IMUL);
        }
        if (node->content_ == "/")
        {
            if (lhs->type_ == "<����>")
            {
                int idx = GetLocalVariableIndex(lhs->GetSymbol());
                if (idx != -1)
                    ProgramGenerator::Emit(Instruction::ILOAD, std::to_string(idx));
                else    // ȫ�ֱ���
                    ProgramGenerator::Emit(Instruction::GETSTATIC, ProgramGenerator::program_name  + "/" + lhs->content_ + " I");
            }
            else if (lhs->type_ == "<����>")
            {
                ProgramGenerator::Emit(Instruction::SIPUSH, std::to_string(lhs->i_value_));
            }

            if (rhs->type_ == "<����>")
            {
                int idx = GetLocalVariableIndex(rhs->GetSymbol());
                if (idx != -1)
                    ProgramGenerator::Emit(Instruction::ILOAD, std::to_string(idx));
                else    // ȫ�ֱ���
                    ProgramGenerator::Emit(Instruction::GETSTATIC, ProgramGenerator::program_name  + "/" + rhs->content_ + " I");
            }
            else if (rhs->type_ == "<����>")
            {
                ProgramGenerator::Emit(Instruction::SIPUSH, std::to_string(rhs->i_value_));
            }

            // �����lhs��rhs��type_��������˵�����������Ѿ������ջ������ȡ��ѹ��
            ProgramGenerator::Emit(Instruction::IDIV);
        }
    }

    if (type == "COP")
    {
        ASTNode * lhs = node->GetChildrenList().front();
        ASTNode * rhs = node->GetChildrenList().back();

        if (lhs->type_ == "<����>")
        {
            int idx = GetLocalVariableIndex(lhs->GetSymbol());
            if (idx != -1)
                ProgramGenerator::Emit(Instruction::ILOAD, std::to_string(idx));
            else    // ȫ�ֱ���
                ProgramGenerator::Emit(Instruction::GETSTATIC, ProgramGenerator::program_name  + "/" + lhs->content_+ " I");
        }
        else if (lhs->type_ == "<����>")
        {
            ProgramGenerator::Emit(Instruction::SIPUSH, std::to_string(lhs->i_value_));
        }

        if (rhs->type_ == "<����>")
        {
            int idx = GetLocalVariableIndex(rhs->GetSymbol());
            if (idx != -1)
                ProgramGenerator::Emit(Instruction::ILOAD, std::to_string(idx));
            else    // ȫ�ֱ���
                ProgramGenerator::Emit(Instruction::GETSTATIC, ProgramGenerator::program_name  + "/" + rhs->content_ + " I");
        }
        else if (rhs->type_ == "<����>")
        {
            ProgramGenerator::Emit(Instruction::SIPUSH, std::to_string(rhs->i_value_));
        }

        std::string branch = ProgramGenerator::GetInstance()->GetBranch() + "\n";
        // �����lhs��rhs��type_��������˵�����������Ѿ������ջ������ȡ��ѹ��
        std::string oper = node->content_;
        if (oper == "==")
        {

        }
        if (oper == "<")
        {
            ProgramGenerator::EmitString(Instruction::IF_ICMPGE.ToString() + " " + branch);
        }
        if (oper == "<=")
        {

        }
        if (oper == ">")
        {
            ProgramGenerator::EmitString(Instruction::IF_ICMPLE.ToString() + " " + branch);
        }
        if (oper == ">=")
        {

        }
        if (oper == "!=")
        {

        }
    }

    if (type == "BOP")
    {
        ASTNode * lhs = node->GetChildrenList().front();
        ASTNode * rhs = node->GetChildrenList().back();
    }

/*    if (type == "<����>")
        node->type_ = "<����>";
    if (type == "<����>")
        node->type_ = "<����>";*/
}

int LR1Parser::GetLocalVariableIndex(Symbol *symbol)
{
    Symbol * func = type_system_.GetSymbolByText(symbol->symbol_scope_);
    std::string func_name = func->name_;

    std::list<Symbol *> local_variables;
    Symbol * func_args = func->args_;
    while (func_args)
    {
        local_variables.emplace_front(func_args);
        func_args = func_args->GetNextSymbol();
    }
    local_variables.reverse();

    local_vars_.clear();
    GetVariblesByFuncBody((ASTNode *)func->body_);

    for (auto sym : local_variables)    // �Ƚ�list�еĺ�������ȥ��
    {
        for (auto iter = local_vars_.begin(); iter != local_vars_.end(); ++iter)
        {
            if (*iter == sym)
            {
                local_vars_.erase(iter);
                break;
            }
        }
    }

    local_variables.insert(local_variables.end(), local_vars_.begin(), local_vars_.end());

    int idx = 0;
    for (auto elem : local_variables)
    {
        if (elem == symbol)
            return idx;
        ++idx;
    }

    return -1;
}

/*!
 * �ݹ���������壬�鿴�������õ�����������˳��
 * @param node
 * @return
 */
void LR1Parser::GetVariblesByFuncBody(ASTNode * node)
{
    for (auto child : node->GetChildrenList())
    {
        GetVariblesByFuncBody(child);

        if (child->type_ == "��������")
        {
            ASTNode * tmp = child->GetChildrenList().front();
            for (auto var : tmp->GetChildrenList())
            {
                if (var->type_ == "INIT")
                    local_vars_.emplace_back(var->GetChildrenList().front()->GetSymbol());
                else
                    local_vars_.emplace_back(var->GetSymbol());
            }
        }
    }
}

std::string LR1Parser::EmitArgs(Symbol *func)
{
    Symbol * func_args = func->args_;
    std::list<Symbol *> params;
    while (func_args)
    {
        params.emplace_back(func_args);
        func_args = func_args->GetNextSymbol();
    }
    params.reverse();
    std::string args = "(";
    for (auto param : params)
    {
        std::string arg;
        if (param->GetType() == "int")
            arg += "I";

        args += arg;
    }

    if (func->GetType() == "int")
    {
        args += ")I";
    }
    else
    {
        args += ")V";
    }

    return args;
}

void LR1Parser::EmitReturnInstruction(Symbol *sym)
{
    if (sym->GetType() == "int")
        ProgramGenerator::Emit(Instruction::IRETURN);
    else
        ProgramGenerator::Emit(Instruction::RETURN);
}

void LR1Parser::CpySymbolToParent(ASTNode *child, ASTNode *parent)
{
    parent->type_ = child->type_;
    parent->symbol_ = child->symbol_;
    parent->i_value_ = child->i_value_;
    parent->f_value_ = child->f_value_;
    parent->c_value_ = child->c_value_;
    parent->s_value_ = child->s_value_;
}

LR1Parser::~LR1Parser()
= default;
