#include "include/LL1Parser.h"

#include <utility>
#include <fstream>
#include <queue>


LL1Parser::LL1Parser(Grammar grammar)
    :
        start_symbol_(grammar.get_start_symbol()),
        grammar_(grammar.get_grammar())
{
    ProcessGrammar();   // �ķ�����

    build_terminal_set(); // �����ս������

    build_nullable_sets();  // ���� NULLABLE ����
    build_first_sets(); // ���� FIRST ����
    build_follow_sets(); // ���� FOLLOW ����
    build_parse_table(); // ���� LL(1) ������

    PrintFirstAndFollowSets();  // ��ӡ FIRST ���� FOLLOW ��
    PrintParseTable();    // ��ӡ������
}

std::vector<std::string> LL1Parser::tokenize(const std::string &input)
{
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;

    while (iss >> token)
    {
        tokens.push_back(token);
    }
    tokens.emplace_back("#");

    return tokens;
}

/*!
 * tokens[];    // all tokens
 * i = 0;
 * stack = [S]  // S�ǿ�ʼ����
 * while (stack != [])
 *  if (stack[top] is a terminal t)
 *      if (t == tokens[i++])
 *          pop();
 *      else error(...);
 *  else if (stack[top] is a nonterminal T)
 *      pop()
 *      push(table[T, tokens[i]])   // �������
 * @param tokens - �ʷ����token��
 * @return  �ķ��Ƿ���ȷ����
 */
bool LL1Parser::Parse(const std::vector<std::string>& tokens)
{
    size_t input_pos = 0;
    std::vector<std::string> parse_tokens = tokens;
    parse_tokens.emplace_back("#");
    std::stack<std::string> parse_stack;
    parse_stack.emplace("#");
    parse_stack.push(start_symbol_);

    std::vector<std::string> out_put;
    out_put.emplace_back("#");
    out_put.emplace_back(start_symbol_);

    while (!parse_stack.empty())
    {
        // ��ӡ����ջ
        std::cout << "The parse stack is: ";
        for (const auto& s : out_put)
        {
            std::cout << s << " ";
        }
        std::cout << std::endl;

        std::string symbol = parse_stack.top();

        if (IsTerminal(symbol))
        {
            if (input_pos == tokens.size())
            {
                if (symbol != "#")
                {
                    std::cerr << "Error occurred at position: " << input_pos
                              << ", Symbol is: " << symbol
                              << std::endl;
                    return false;
                }
                else
                {
                    break;
                }
            }

            if (symbol == parse_tokens[input_pos++])
            {
                parse_stack.pop();

                out_put.erase(out_put.end() - 1);
            }
            else
            {
                std::cerr << "Error occurred at position: " << input_pos
                            << ", Symbol is: " << symbol
                            << std::endl;
                return false;
            }
        }
        else
        {
            parse_stack.pop();

            out_put.erase(out_put.end() - 1);

            // �Ҳ�����Ӧ�ս�����޷�ƥ��
            if (parse_table_.at(symbol).find(parse_tokens[input_pos]) == parse_table_.at(symbol).end())
            {
                std::cerr << "Error occurred at position: " << input_pos
                          << ", Symbol is: " << symbol
                          << std::endl;
                return false;
            }

            Production production = parse_table_.at(symbol).at(parse_tokens[input_pos]).at(0);
            for (auto it = production.rbegin(); it != production.rend(); ++it)
            {
                if (IsEpsilon(*it)) break;

                parse_stack.push(*it);

                out_put.emplace_back(*it);
            }
        }
    }

    std::cout << "Parse successfully!" << std::endl;

    return true;
}

void LL1Parser::PrintFirstAndFollowSets()
{
    std::cout << "First and Follow Sets:" << std::endl;
    std::cout << "---------------------------------" << std::endl;

    for (const auto& nonTerminalEntry : first_sets_)
    {
        const std::string& nonTerminal = nonTerminalEntry.first;
        const std::unordered_set<std::string>& firstSet = nonTerminalEntry.second;

        if (IsTerminal(nonTerminal))
            continue;

        std::cout << "Non-terminal: " << nonTerminal << std::endl;
        std::cout << "  First Set: { ";
        for (const std::string& symbol : firstSet)
        {
            std::cout << symbol << " ";
        }
        std::cout << "}" << std::endl;

        const std::unordered_set<std::string>& followSet = follow_sets_[nonTerminal];
        std::cout << "  Follow Set: { ";
        for (const std::string& symbol : followSet)
        {
            std::cout << symbol << " ";
        }
        std::cout << "}" << std::endl;
    }

    std::cout << "---------------------------------" << std::endl;
}

void LL1Parser::PrintParseTable()
{
    std::cout << "LL(1) Parse Table:" << std::endl;
    std::cout << "--------------------------------------------------------------------" << std::endl;

    for (const auto& nonTerminalEntry : parse_table_)
    {
        const std::string& nonTerminal = nonTerminalEntry.first;
        const auto& terminalEntry = nonTerminalEntry.second;

        for (const auto& terminalProductionPair : terminalEntry)
        {
            const std::string& terminal = terminalProductionPair.first;
            const Productions& productions = terminalProductionPair.second;

            for (const auto& production : productions)
            {
                std::cout << "Non-terminal: " << nonTerminal << ", Terminal: " << terminal << " => Production: ";
                for (const std::string& symbol : production)
                {
                    std::cout << symbol << " ";
                }
                std::cout << std::endl;
            }
        }
    }

    std::cout << "--------------------------------------------------------------------" << std::endl;
}

void LL1Parser::PrintGrammar()
{
    // ���ת������ķ�
    std::cout << "The grammar_:" << std::endl;
    std::cout << "------------------------------------" << std::endl;
    for (const auto &rule: grammar_)
    {
        std::cout << rule.first << " -> ";
        for (size_t i = 0; i < rule.second.size(); ++i)
        {
            Production production = rule.second.at(i);
            for (const auto & j : production)
            {
                std::cout << j << " ";
            }
            if (i != rule.second.size() - 1)
            {
                std::cout << "| ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << "------------------------------------" << std::endl;
}

void LL1Parser::ProcessGrammar()
{
    ExtractLeftFactoring(); // ��ȡ������
    std::cout << "��ȡ�����Ӻ���ķ���\n"
                << "--------------------------------------------" << std::endl;
    PrintGrammar();
    RemoveLeftRecursion();  // ������ݹ�
    ExtractLeftFactoring(); // ��ȡ������
    std::cout << "������ݹ����ķ���\n"
              << "--------------------------------------------" << std::endl;
    PrintGrammar();
}

std::string LL1Parser::GetAddNonTerminalName(NonTerminal new_left_part)
{
    // ��ȡ����������� ���ֶ�Ϊԭ�������� + `, ������ڣ�������� `
    new_left_part += '\'';
    while (true)
    {
        if (grammar_.find(new_left_part) != grammar_.end())
            new_left_part += '\'';
        else
            break;
    }

    return new_left_part;
}

void LL1Parser::ExtractLeftFactoring()
{
    for (const auto& line : grammar_)
    {
        RightPart oldRightPart; // ����
        RightPart newRightPart; // ������

        NonTerminal left_part = line.first;
        // 1. ����
        std::vector<Productions> lists = Classify(left_part);

        // 2. ��ȡ
        bool hasFactor = false;
        for (Productions& item : lists)
        {
            // ��ȡ�����ӣ������޳���item�е���������
            std::vector<std::string> commonFactors = DecompositionExtract(left_part, item);

            // ��������
            if (!commonFactors.empty())
            {
                hasFactor = true;
                // �����󲿵Ĳ���ʽ��
                // ��Ϊ����ʽ����ɾ���˺������������ֱ�����һ����Ϊ�������� + ������
                commonFactors.emplace_back(GetAddNonTerminalName(left_part));
                oldRightPart.push_back(commonFactors);

                for (Production& production : item)
                {
                    // ���������
                    newRightPart.push_back(production);
                }
            }
            else    // û�л��߸�����Ϊ��������
            {
                for (Production& production : item)
                {
                    oldRightPart.push_back(production);
                }
            }
        }

        grammar_[left_part] = oldRightPart;
        if (hasFactor)
            grammar_[GetAddNonTerminalName(left_part)] = newRightPart;
    }
}

std::vector<RightPart> LL1Parser::Classify(const NonTerminal &left_part)
{
    std::vector<RightPart> lists;

    // 1. ��鵱ǰ����ʽ�����Ƿ���������
    for (const Production& production : grammar_[left_part])
    {
        bool haveSame = false;

        // ��ǰitem���Ƿ�����ƥ���������
        for (std::vector<Production>& item : lists)
        {
            // ���ֻ࣬����ÿ�������еĵ�һ���������ƥ�䣬���ʣ�µ�Ҳ��ƥ�䣩�ĵ�һ������
            if (item.at(0).at(0) == production.at(0))
            {
                Production production1(production);
                item.emplace_back(production);
                haveSame = true;
            }
        }

        // ��Ϊ�������ڱ�����ʱ�����޸ģ��������û�м�鵽��ͬ�ģ������е����Ϊһ��������
        if (!haveSame)
        {
            std::vector<Production> tempItem;
            Production production1(production);
            tempItem.push_back(production1);
            lists.push_back(tempItem);
        }
    }

    return lists;
}

/**
	 * �Ե�ǰ��� list ���зֽ���ȡ����
	 *  ����
	 *      E -> E + T | E - T | E + ( T )
	 *      ��ǰ���Ϊ { E + T } { E - T } { E + ( T ) }
	 *
	 *  1. ��ȡ��һ��ĵ�һ������ E, ��ǰ�ķ�Ϊ��
	 *      E -> + T | E - T | E + ( T )
	 *  2. ��ʣ���е�ÿһ�����ƥ����ȡ�����ƥ��ɹ����Ƴ�����
	 *      (1). E -> + T | E - T | E + ( T )
	 *      (2). E -> + T | - T | E + ( T )
	 *      (3). E -> + T | - T | + ( T )
	 *  3. Ȼ������ȡ��һ��ĵ�һ������ +, ��ǰ�ķ�Ϊ��
	 *      E -> T | - T | + ( T )
	 *  4. ��ʣ���е�ÿһ�����ƥ����ȡ�����ƥ��ɹ����Ƴ�����, ���ƥ��ʧ���򷵻�֮ǰ�Ƴ�������
	 *      (1). E -> T | - T | + ( T )
	 *      (2). E -> T | T | + ( T )       {��ʱ�Ƴ�����'-'��ƥ��ʧ�ܣ�����֮ǰ��'+'}
	 *      (3). E -> + T | - T | + ( T )   {���Դ�ʱ����������������� E}
	 *
	 **/
std::vector<std::string> LL1Parser::DecompositionExtract(const NonTerminal& left_part, Productions& list)
{
    std::vector<std::string> commonFactors; // ������
    if (list.size() == 1 || list.empty()) return commonFactors;


    // ���ѭ����û��ƥ��ɹ�(mayExistΪfalse)�������û����������
    bool mayExist = true;

    while (mayExist)
    {
        // �����һ��������
        commonFactors.emplace_back(list.at(0).at(0));

        Production & item = list.at(0);
        item.erase(item.begin());

        for (size_t i = 1; i < list.size(); ++i)
        {
            // �п����Ƴ����ˣ�����ĳһ��Ϊ��
            /*
             * S -> i E t S | i E t S E s ��������� i E t S
             *
             * S -> _ | E s
             */
            if (!list.at(0).empty() && list.at(i).at(0) == *(commonFactors.end() - 1))
            {
                // ƥ��ɹ����Ƴ�
                list.at(i).erase(list.at(i).begin());

                if (list.at(i).empty())
                {
                    // ƥ��ʧ�ܣ�����
                    mayExist = false;

                    std::string subItem = *(commonFactors.end() - 1);
                    commonFactors.pop_back();

                    for (int j = i; j >= 0; --j)
                    {
                        list.at(j).insert(list.at(j).begin(), subItem);
                    }

                    break;
                }
            }
            else
            {
                // ƥ��ʧ�ܣ�����
                mayExist = false;

                std::string subItem = *(commonFactors.end() - 1);
                commonFactors.pop_back();

                for (int j = i - 1; j >= 0; --j)
                {
                    list.at(j).insert(list.at(j).begin(), subItem);
                }

                break;
            }
        }
    }

    return commonFactors;
}

void LL1Parser::RemoveLeftRecursion()
{
    std::vector<std::string> Vn;    // ���ս����
    for (const auto& it : grammar_)
    {
        Vn.insert(Vn.end(), it.first);
    }

    for (int i = 0; i < Vn.size(); ++i)
    {
        std::string pi = Vn[i];
        std::vector<std::vector<std::string>> newRightPart;
        int length = grammar_[pi].size();
        for (int k = 0; k < length; ++k)  // ����pi���Ҳ�����ʽ
        {
            std::vector<std::string> right = grammar_[pi].at(k);   // ��ǰ����ʽ

            for (int j = 0; j < i; ++j)
            {
                std::string pj = Vn.at(j);
                if (pj == right[0])
                {
                    // ����pj���Ҳ�
                    for (const std::vector<std::string>& b_right : grammar_[pj])
                    {
                        std::vector<std::string> production;
                        production = b_right;
                        production.insert(production.end(), right.begin()+1, right.end());
                        newRightPart.push_back(production);
                    }
                    grammar_[pi].erase(grammar_[pi].begin() + k);
                    grammar_[pi].insert(grammar_[pi].begin() + k, newRightPart.begin(), newRightPart.end());

                    // ���µ�ǰ����ʽ����ʽ
                    right = grammar_[pi].at(k);
                    length = grammar_[pi].size();
                    newRightPart.clear();
                }
            }
        }

        // ����ֱ����ݹ�
        RemoveLeftRecursionHelper(pi);
    }
}

void LL1Parser::RemoveLeftRecursionHelper(const NonTerminal& left_part)
{
    // �жϵ�ǰ����ʽ�Ƿ����ֱ����ݹ�
    bool includeLeftPart = false;
    for (auto& production : grammar_[left_part])
    {
        if (production.at(0) == left_part)
            includeLeftPart = true;
    }
    if (!includeLeftPart)
        return;

    std::vector<std::vector<std::string>> a_productions;    // A'����ʽ
    std::vector<std::vector<std::string>> b_productions;    // �µ�A����ʽ

    for (auto& production : grammar_[left_part])
    {
        if (production.at(0) == left_part)
        {
            if (production.size() == 1)
            {
                // A -> A
                continue;
            }
            std::vector<std::string> a_production(production.begin() + 1, production.end());
            a_production.push_back(GetAddNonTerminalName(left_part));
            a_productions.push_back(a_production);
        }
        else
        {
            std::vector<std::string> b_production = production;
            b_production.push_back(GetAddNonTerminalName(left_part));
            b_productions.push_back(b_production);
        }
    }

    a_productions.push_back({"<epsilon>"});

    grammar_[GetAddNonTerminalName(left_part)] = a_productions;
    grammar_[left_part] = b_productions;

    // �����﷨
}

void LL1Parser::build_terminal_set()
{
    for (const auto& rule : grammar_)
    {
        NonTerminal non_terminal = rule.first;
        RightPart right_part = rule.second;
        for (const auto& production : right_part)
        {
            for (const auto& symbol : production)
            {
                if (IsTerminal(symbol))
                    terminal_set_.emplace(symbol);
            }
        }
    }
}

/*!
 * NULLABLE����
 * ���ս��X���ڼ���NULLABLE�����ҽ�����
 * ���������
 *  * X -> ��
 * ���������
 *  * X -> Y1 ... Yn
 *      * Y1, ..., Yn ��n�����ս�����Ҷ�����NULLABLE��
 *
 * �㷨��
 * NULLABLE = { };
 *
 * while (nullable is still changing)
 *  foreach (production p : X -> ��)
 *      if (�� == ��)
 *          NULLABLE ��= { X }
 *      if (�� == Y1 ... Yn)
 *          if (Y1 �� NULLABLE && ... && Yn �� NULLABLE)
 *              NULLABLE ��= { X }
 */
void LL1Parser::build_nullable_sets()
{
    bool set_has_changed = true;
    while (set_has_changed)
    {
        set_has_changed = false;
        size_t size_of_old_nullable_set = nullable_sets_.size();

        for (const auto& rule : grammar_)
        {
            NonTerminal non_terminal = rule.first;
            RightPart right_part = rule.second;

            for (const auto& production : right_part)
            {
                if (IsEpsilon(production.at(0)))  // A -> <epsilon>
                {
                    nullable_sets_.emplace(non_terminal);
                    if (nullable_sets_.size() != size_of_old_nullable_set)
                        set_has_changed = true;
                    break;
                }

                bool all_nullable = true;
                for (const auto& symbol : production)
                {
                    if (IsTerminal(symbol))
                    {
                        all_nullable = false;
                        break;
                    }
                    else
                    {
                        if (nullable_sets_.find(symbol) == nullable_sets_.end())
                        {
                            all_nullable = false;
                            break;
                        }
                    }
                }
                if (all_nullable)
                {
                    nullable_sets_.emplace(non_terminal);
                    if (nullable_sets_.size() != size_of_old_nullable_set)
                        set_has_changed = true;
                }
            }
        }
    }
}

/*!
 * FIRST���Ĳ������㷨��
 * foreach (nonterminal N)
 *  FIRST(n) = { }
 * while (some set is changing)
 *  foreach (production p : N -> ��1 .. ��n)
 *      if (��1 == a)
 *          FIRST(N) ��= { a }
 *      if (��1 == M)
 *          FIRST(N) ��= FIRST(M)
 *
 * FIRST���ϵ��������㹫ʽ
 * ���ڹ��ɵļ������
 *  * ���������
 *      X -> a
 *          * FIRST(X) ��= { a }
 *  * ���������
 *      X -> Y1 Y2 ... Yn
 *          * FIRST(X) ��= FIRST(Y1)
 *          * if Y1 �� NULLABLE, FIRST(X) ��= FIRST(Y2)
 *          * if Y1, Y2 �� NULLABLE, FIRST(X) ��= FIRST(Y3)
 *          * ...
 *
 * FIRST���������������㷨��
 * foreach (nonterminal N)
 *  FIRST(n) = { }
 *
 * while (some set is changing)
 *  foreach (production p : N -> ��1 .. ��n)
 *      foreach (��i from ��1 upto ��n)
 *          if (��i == a)
 *              FIRST(N) ��= { a }
 *              break
 *          if (��i == M ...)
 *              FIRST(N) ��= FIRST(M)
 *              if (M is not in NULLABLE)
 *                  break;
 */
void LL1Parser::build_first_sets()
{
    for (const auto& rule : grammar_)
    {
        NonTerminal non_terminal = rule.first;
        first_sets_[non_terminal].clear();
    }

    bool set_has_changed = true;
    while (set_has_changed)
    {
        set_has_changed = false;

        for (const auto& rule : grammar_)
        {
            NonTerminal non_terminal = rule.first;
            RightPart right_part = rule.second;

            for (const auto& production : right_part)
            {
                for (const auto& symbol : production)
                {
                    size_t size_of_old_first_set = first_sets_.at(non_terminal).size();

                    if (IsEpsilon(symbol))  // ����ʽΪ�ţ��մ�
                        break;

                    if (IsTerminal(symbol))
                    {
                        first_sets_.at(non_terminal).emplace(symbol);

                        if (first_sets_.at(non_terminal).size() != size_of_old_first_set)
                            set_has_changed = true;

                        break;
                    }
                    else
                    {
                        first_sets_.at(non_terminal).insert(
                                first_sets_.at(symbol).begin(),
                                first_sets_.at(symbol).end()
                        );

                        if (first_sets_.at(non_terminal).size() != size_of_old_first_set)
                            set_has_changed = true;

                        if (nullable_sets_.find(symbol) == nullable_sets_.end())
                            break;
                    }
                }
            }
        }
    }
}

/*!
 * FOLLOW���Ĳ������㷨
 * foreach (nonterminal N)
 *  FOLLOW(N) = { }
 *
 * while (some set is changing)
 *  foreach (production p : N -> ��1 ... ��n)
 *      temp = FOLLOW(N)
 *      foreach (��i from ��n downto ��1)  // ����
 *          if (��i == a ...)
 *              temp = { a }
 *          if (��i == M ...)
 *              FOLLOW(M) ��= temp
 *              if (M is not NULLABLE)
 *                  temp = FIRST(M)
 *              else temp ��= FIRST(M)
 */
void LL1Parser::build_follow_sets()
{
    for (const auto& rule : grammar_)
    {
        NonTerminal non_terminal = rule.first;
        follow_sets_[non_terminal].clear();
    }

    follow_sets_[start_symbol_].emplace("#");

    bool set_has_changed = true;
    while (set_has_changed)
    {
        set_has_changed = false;
        for (const auto& rule : grammar_)
        {
            NonTerminal non_terminal = rule.first;
            RightPart right_part = rule.second;
            for (const auto& production : right_part)
            {
                std::unordered_set<Terminal> temp = follow_sets_.at(non_terminal);
                for (auto symbol = production.rbegin(); symbol != production.rend(); ++symbol)
                {
                    if (IsEpsilon(*symbol))
                        break;

                    if (IsTerminal(*symbol))
                    {
                        temp = {*symbol};
                    }
                    else
                    {
                        size_t old_size_of_follow = follow_sets_.at(*symbol).size();
                        follow_sets_.at(*symbol).insert(temp.begin(), temp.end());
                        if (follow_sets_.at(*symbol).size() != old_size_of_follow)
                            set_has_changed = true;

                        if (nullable_sets_.find(*symbol) == nullable_sets_.end())
                            temp = first_sets_.at(*symbol);
                        else
                            temp.insert(first_sets_.at(*symbol).begin(),
                                        first_sets_.at(*symbol).end());
                    }
                }
            }
        }
    }
}

void LL1Parser::build_parse_table()
{
    for (const auto& rule : grammar_)
        parse_table_[rule.first].clear();

    for (const auto& rule : grammar_)
    {
        NonTerminal non_terminal = rule.first;
        RightPart right_part = rule.second;

        for (const auto& production : right_part)
        {
            std::unordered_set<Terminal> first_s_set_of_production = GetFirstSSet(non_terminal, production);
            for (const auto& terminal : first_s_set_of_production)
            {
                //TODO: ��ͻ���
                // ����ĳ�ͻ��⴦��ʽΪ����
                if (!parse_table_.at(non_terminal)[terminal].empty())
                    std::cerr << "Conflict occurred!" << "   " << non_terminal << std::endl;
                Productions productions;
                productions.insert(productions.end(),
                                   parse_table_.at(non_terminal)[terminal].begin(),
                                   parse_table_.at(non_terminal)[terminal].end());
                productions.emplace_back(production);
                parse_table_.at(non_terminal)[terminal] = productions;
            }
        }
    }
}

/*!
 * ��FIRST���ƹ㵽���⴮������ӣ��ϣ����ÿ������ʽ���Ҳ���������
 * FIRST_S(��1 ... ��n) =
 *  FIRST(N),   if ��1 == N;
 *  { a },      if ��1 == a.
 *
 * FIRST_S���������㷨��
 * foreach (production p)
 *  FIRST_S(p) = { }
 *
 * calculate_FIRST_S (production p : N -> ��1 ... ��n)
 *  foreach (��i from ��i to ��n)
 *      if (��i == a ...)
 *          FIRST_S(p) ��= { a }
 *          return;
 *      if (��i == M ...)
 *          FIRST_S(p) ��= FIRST(M)
 *              if (M is not NULLABLE)
 *                  return;
 *              else FIRST_S(p) ��= FOLLOW(N)
 *
 * @param non_terminal  - �󲿷��ս��
 * @param production - ���󲿵�һ������ʽ
 * @return  ���ս����Ӧ����ʽ��FIRST_S����
 */
std::unordered_set<Terminal> LL1Parser::GetFirstSSet(const NonTerminal& non_terminal, const Production& production)
{
    std::unordered_set<Terminal> first_s_set;

    for (const auto& symbol : production)
    {
        if (IsEpsilon(symbol))
            break;

        if (IsTerminal(symbol))
        {

            first_s_set.emplace(symbol);
            return first_s_set;
        }
        else
        {
            first_s_set.insert(first_sets_.at(symbol).begin(),
                               first_sets_.at(symbol).end());

            if (nullable_sets_.find(symbol) == nullable_sets_.end())
                return first_s_set;
        }
    }

    first_s_set.insert(follow_sets_.at(non_terminal).begin(),
                       follow_sets_.at(non_terminal).end());
    return first_s_set;
}

bool LL1Parser::IsTerminal(const std::string &symbol)
{
    return grammar_.find(symbol) == grammar_.end() && (symbol != "<epsilon>");
}

bool LL1Parser::IsEpsilon(const Terminal& symbol)
{
    return symbol == "<epsilon>";
}

LL1Parser::~LL1Parser()
= default;
