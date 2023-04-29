#include <iostream>
#include <sstream>
#include "include/Grammar.h"
#include "include/LL1Parser.h"
#include "Lexer/include/FA.h"
#include "Lexer/include/Lexer.h"

void GrammarTest()
{
    Grammar grammar(R"(D:\Project\CLion\Grammar\Grammar\GrammarRules\grammar.txt)", "<����>");

    // ���� LL1Parser ����
    LL1Parser parser(grammar);

    std::vector<std::string> tokens = {
            "int",
            "main",
            "(",
            ")",
            "{",
            "}"
    };

    if (parser.Parse(tokens)) {
        std::cout << "Accepted." << std::endl;
    } else {
        std::cout << "Rejected." << std::endl;
    }
}

std::map<std::string, std::string> ReadRegex()
{
    std::map<std::string, std::string> regex_map;

    // 1. ������Ҫ��ͷ�ļ������Ϸ��г�
    // 2. ���ļ�������Ƿ�ɹ���
    std::ifstream in(R"(D:\Project\CLion\Compiler\Lexer\regex.txt)", std::ios::in);
    if (!in.is_open())
    {
        std::cerr << "��ȡ�ļ�ʧ�ܣ�" << std::endl;
        exit(-1);
    }

    // 3. ʹ��std::getline�������ж�ȡ�ļ�
    std::string line;
    while (std::getline(in, line))
    {
        // 4. ʹ��std::stringstream��ÿ�зָ�ɵ���
        std::stringstream ss(line);
        std::string category;

        // 5. ����ÿ������

        ss >> category;
        category.pop_back();
        std::string regex_symbol;

        regex_map[category].append("(");
        while (ss >> regex_symbol)
        {
            if (regex_map.find(regex_symbol) != regex_map.end())
            {
                regex_map[category].append(regex_map.at(regex_symbol));
            }
            else
            {
                regex_map[category].append(regex_symbol);
            }
        }
        regex_map[category].append(")");
    }

    // 6. �ر��ļ�
    in.close();

    return regex_map;
}

void RegexTest()
{
    std::map<std::string, std::string> regex_map = ReadRegex();

    FA identifier_fa(regex_map.at("identifier"));
    FA operator_fa(regex_map.at("operator"));
    FA decimal_fa(regex_map.at("decimal"));
    FA hex_fa(regex_map.at("hex"));
    FA oct_fa(regex_map.at("oct"));
    FA float_fa(regex_map.at("float"));

    std::string user_input;
    while (true)
    {
        std::cout << "������һ���ַ�����" << std::endl;
        std::cin >> user_input;
        if (identifier_fa.Judge(user_input))
            std::cout << "����һ����ʶ����" << std::endl;
        if (operator_fa.Judge(user_input))
            std::cout << "����һ���������" << std::endl;
        if (decimal_fa.Judge(user_input))
            std::cout << "����һ��ʮ��������" << std::endl;
        if (hex_fa.Judge(user_input))
            std::cout << "����һ��ʮ����������" << std::endl;
        if (oct_fa.Judge(user_input))
            std::cout << "����һ���˽�������" << std::endl;
        if (float_fa.Judge(user_input))
            std::cout << "����һ����������" << std::endl;
    }
}

void LexerTest()
{
    Lexer lexer(R"(D:\Project\CLion\Compiler\Lexer\test.txt)",
                R"(D:\Project\CLion\Compiler\Lexer\out.txt)");
}

int main() {
    RegexTest();
    //LexerTest();

    getchar();
    return 0;
}


