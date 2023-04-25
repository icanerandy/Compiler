#include "include/LL1Parser.h"

#include <utility>
#include <fstream>
#include <queue>


LL1Parser::LL1Parser(Grammar grammar)
    :
        start_symbol_(grammar.get_start_symbol()),
        grammar_(grammar.get_grammar())
{
    ProcessGrammar();   // 文法处理

    build_terminal_set(); // 建立终结符集合

    build_nullable_sets();  // 计算 NULLABLE 集合
    build_first_sets(); // 计算 FIRST 集合
    build_follow_sets(); // 计算 FOLLOW 集合
    build_parse_table(); // 构建 LL(1) 分析表

    PrintFirstAndFollowSets();  // 打印 FIRST 集和 FOLLOW 集
    PrintParseTable();    // 打印分析表
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
 * stack = [S]  // S是开始符号
 * while (stack != [])
 *  if (stack[top] is a terminal t)
 *      if (t == tokens[i++])
 *          pop();
 *      else error(...);
 *  else if (stack[top] is a nonterminal T)
 *      pop()
 *      push(table[T, tokens[i]])   // 逆序放入
 * @param tokens - 词法输出token流
 * @return  文法是否正确分析
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
        // 打印分析栈
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

            // 找不到对应终结符，无法匹配
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
    // 输出转换后的文法
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
    ExtractLeftFactoring(); // 提取左公因子
    std::cout << "提取左公因子后的文法：\n"
                << "--------------------------------------------" << std::endl;
    PrintGrammar();
    RemoveLeftRecursion();  // 消除左递归
    ExtractLeftFactoring(); // 提取左公因子
    std::cout << "消除左递归后的文法：\n"
              << "--------------------------------------------" << std::endl;
    PrintGrammar();
}

std::string LL1Parser::GetAddNonTerminalName(NonTerminal new_left_part)
{
    // 获取新增项的名称 名字都为原来的名称 + `, 如果存在，则继续加 `
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
        RightPart oldRightPart; // 旧项
        RightPart newRightPart; // 新增项

        NonTerminal left_part = line.first;
        // 1. 归类
        std::vector<Productions> lists = Classify(left_part);

        // 2. 提取
        bool hasFactor = false;
        for (Productions& item : lists)
        {
            // 获取左公因子，并且剔除了item中的左公因子项
            std::vector<std::string> commonFactors = DecompositionExtract(left_part, item);

            // 有左公因子
            if (!commonFactors.empty())
            {
                hasFactor = true;
                // 更新左部的产生式集
                // 因为产生式集中删除了含左公因子项，所以直接添加一个，为公共子项 + 新增项
                commonFactors.emplace_back(GetAddNonTerminalName(left_part));
                oldRightPart.push_back(commonFactors);

                for (Production& production : item)
                {
                    // 添加新增项
                    newRightPart.push_back(production);
                }
            }
            else    // 没有或者该类型为单独类型
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

    // 1. 检查当前产生式集合是否有左公因子
    for (const Production& production : grammar_[left_part])
    {
        bool haveSame = false;

        // 当前item种是否满足匹配的左公因子
        for (std::vector<Production>& item : lists)
        {
            // 归类，只需检查每个集合中的第一项（如果和这项不匹配，则和剩下的也不匹配）的第一个符号
            if (item.at(0).at(0) == production.at(0))
            {
                Production production1(production);
                item.emplace_back(production);
                haveSame = true;
            }
        }

        // 因为不可以在遍历的时候做修改，所以如果没有检查到相同的，则将现有的添加为一个新种类
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
	 * 对当前类别 list 进行分解提取操作
	 *  即：
	 *      E -> E + T | E - T | E + ( T )
	 *      当前类别为 { E + T } { E - T } { E + ( T ) }
	 *
	 *  1. 提取第一项的第一个子项 E, 当前文法为：
	 *      E -> + T | E - T | E + ( T )
	 *  2. 对剩余中的每一项都进行匹配提取，如果匹配成功则移除子项
	 *      (1). E -> + T | E - T | E + ( T )
	 *      (2). E -> + T | - T | E + ( T )
	 *      (3). E -> + T | - T | + ( T )
	 *  3. 然后再提取第一项的第一个子项 +, 当前文法为：
	 *      E -> T | - T | + ( T )
	 *  4. 对剩余中的每一项都进行匹配提取，如果匹配成功则移除子项, 如果匹配失败则返还之前移除的子项
	 *      (1). E -> T | - T | + ( T )
	 *      (2). E -> T | T | + ( T )       {此时移除的是'-'，匹配失败，返还之前的'+'}
	 *      (3). E -> + T | - T | + ( T )   {所以此时，三项最长的左公因子是 E}
	 *
	 **/
std::vector<std::string> LL1Parser::DecompositionExtract(const NonTerminal& left_part, Productions& list)
{
    std::vector<std::string> commonFactors; // 左公因子
    if (list.size() == 1 || list.empty()) return commonFactors;


    // 如果循环后，没有匹配成功(mayExist为false)，则代表没有左公因子了
    bool mayExist = true;

    while (mayExist)
    {
        // 先添加一个左公因子
        commonFactors.emplace_back(list.at(0).at(0));

        Production & item = list.at(0);
        item.erase(item.begin());

        for (size_t i = 1; i < list.size(); ++i)
        {
            // 有可能移除完了，导致某一个为空
            /*
             * S -> i E t S | i E t S E s 的最长公因子 i E t S
             *
             * S -> _ | E s
             */
            if (!list.at(0).empty() && list.at(i).at(0) == *(commonFactors.end() - 1))
            {
                // 匹配成功，移除
                list.at(i).erase(list.at(i).begin());

                if (list.at(i).empty())
                {
                    // 匹配失败，返还
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
                // 匹配失败，返还
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
    std::vector<std::string> Vn;    // 非终结符集
    for (const auto& it : grammar_)
    {
        Vn.insert(Vn.end(), it.first);
    }

    for (int i = 0; i < Vn.size(); ++i)
    {
        std::string pi = Vn[i];
        std::vector<std::vector<std::string>> newRightPart;
        int length = grammar_[pi].size();
        for (int k = 0; k < length; ++k)  // 遍历pi的右部产生式
        {
            std::vector<std::string> right = grammar_[pi].at(k);   // 当前产生式

            for (int j = 0; j < i; ++j)
            {
                std::string pj = Vn.at(j);
                if (pj == right[0])
                {
                    // 遍历pj的右部
                    for (const std::vector<std::string>& b_right : grammar_[pj])
                    {
                        std::vector<std::string> production;
                        production = b_right;
                        production.insert(production.end(), right.begin()+1, right.end());
                        newRightPart.push_back(production);
                    }
                    grammar_[pi].erase(grammar_[pi].begin() + k);
                    grammar_[pi].insert(grammar_[pi].begin() + k, newRightPart.begin(), newRightPart.end());

                    // 更新当前产生式产生式
                    right = grammar_[pi].at(k);
                    length = grammar_[pi].size();
                    newRightPart.clear();
                }
            }
        }

        // 消除直接左递归
        RemoveLeftRecursionHelper(pi);
    }
}

void LL1Parser::RemoveLeftRecursionHelper(const NonTerminal& left_part)
{
    // 判断当前产生式是否包含直接左递归
    bool includeLeftPart = false;
    for (auto& production : grammar_[left_part])
    {
        if (production.at(0) == left_part)
            includeLeftPart = true;
    }
    if (!includeLeftPart)
        return;

    std::vector<std::vector<std::string>> a_productions;    // A'产生式
    std::vector<std::vector<std::string>> b_productions;    // 新的A产生式

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

    // 化简语法
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
 * NULLABLE集合
 * 非终结符X属于集合NULLABLE，当且仅当：
 * 基本情况：
 *  * X -> ε
 * 归纳情况：
 *  * X -> Y1 ... Yn
 *      * Y1, ..., Yn 是n个非终结符，且都属于NULLABLE集
 *
 * 算法：
 * NULLABLE = { };
 *
 * while (nullable is still changing)
 *  foreach (production p : X -> β)
 *      if (β == ε)
 *          NULLABLE ∪= { X }
 *      if (β == Y1 ... Yn)
 *          if (Y1 ∈ NULLABLE && ... && Yn ∈ NULLABLE)
 *              NULLABLE ∪= { X }
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
 * FIRST集的不动点算法：
 * foreach (nonterminal N)
 *  FIRST(n) = { }
 * while (some set is changing)
 *  foreach (production p : N -> β1 .. βn)
 *      if (β1 == a)
 *          FIRST(N) ∪= { a }
 *      if (β1 == M)
 *          FIRST(N) ∪= FIRST(M)
 *
 * FIRST集合的完整计算公式
 * 基于归纳的计算规则：
 *  * 基本情况：
 *      X -> a
 *          * FIRST(X) ∪= { a }
 *  * 归纳情况：
 *      X -> Y1 Y2 ... Yn
 *          * FIRST(X) ∪= FIRST(Y1)
 *          * if Y1 ∈ NULLABLE, FIRST(X) ∪= FIRST(Y2)
 *          * if Y1, Y2 ∈ NULLABLE, FIRST(X) ∪= FIRST(Y3)
 *          * ...
 *
 * FIRST集的完整不动点算法：
 * foreach (nonterminal N)
 *  FIRST(n) = { }
 *
 * while (some set is changing)
 *  foreach (production p : N -> β1 .. βn)
 *      foreach (βi from β1 upto βn)
 *          if (βi == a)
 *              FIRST(N) ∪= { a }
 *              break
 *          if (βi == M ...)
 *              FIRST(N) ∪= FIRST(M)
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

                    if (IsEpsilon(symbol))  // 产生式为ε，空串
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
 * FOLLOW集的不动点算法
 * foreach (nonterminal N)
 *  FOLLOW(N) = { }
 *
 * while (some set is changing)
 *  foreach (production p : N -> β1 ... βn)
 *      temp = FOLLOW(N)
 *      foreach (βi from βn downto β1)  // 逆序！
 *          if (βi == a ...)
 *              temp = { a }
 *          if (βi == M ...)
 *              FOLLOW(M) ∪= temp
 *              if (M is not NULLABLE)
 *                  temp = FIRST(M)
 *              else temp ∪= FIRST(M)
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
                //TODO: 冲突检测
                // 下面的冲突检测处理方式为放行
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
 * 把FIRST集推广到任意串（或句子）上，针对每条产生式的右部来做计算
 * FIRST_S(β1 ... βn) =
 *  FIRST(N),   if β1 == N;
 *  { a },      if β1 == a.
 *
 * FIRST_S集合完整算法：
 * foreach (production p)
 *  FIRST_S(p) = { }
 *
 * calculate_FIRST_S (production p : N -> β1 ... βn)
 *  foreach (βi from βi to βn)
 *      if (βi == a ...)
 *          FIRST_S(p) ∪= { a }
 *          return;
 *      if (βi == M ...)
 *          FIRST_S(p) ∪= FIRST(M)
 *              if (M is not NULLABLE)
 *                  return;
 *              else FIRST_S(p) ∪= FOLLOW(N)
 *
 * @param non_terminal  - 左部非终结符
 * @param production - 该左部的一条产生式
 * @return  非终结符对应产生式的FIRST_S集合
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
