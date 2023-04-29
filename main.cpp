#include <iostream>
#include <sstream>
#include "include/Grammar.h"
#include "include/LL1Parser.h"
#include "Lexer/include/FA.h"
#include "Lexer/include/Lexer.h"

void GrammarTest()
{
    Grammar grammar(R"(D:\Project\CLion\Grammar\Grammar\GrammarRules\grammar.txt)", "<程序>");

    // 构建 LL1Parser 对象
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

    std::ifstream in(R"(D:\Project\CLion\Compiler\Lexer\regex.txt)", std::ios::in);
    if (!in.is_open())
    {
        std::cerr << "读取文件失败！" << std::endl;
        exit(-1);
    }

    std::string line;
    while (std::getline(in, line))
    {
        std::stringstream ss(line);
        std::string category;

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

    in.close();
    return regex_map;
}

void InitFA()
{
    std::map<std::string, std::string> regex_map = ReadRegex();

    FA identifier_fa("identifier", regex_map.at("identifier"));
    FA operator_fa("operator", regex_map.at("operator"));
    FA decimal_fa("decimal", regex_map.at("decimal"));
    FA hex_fa("hex", regex_map.at("hex"));
    FA oct_fa("oct", regex_map.at("oct"));
    FA bin_fa("bin", regex_map.at("bin"));
    FA float_fa("float", regex_map.at("float"));
}

void LexerTest()
{
    Lexer lexer(R"(D:\Project\CLion\Compiler\Lexer\test.txt)",
                R"(D:\Project\CLion\Compiler\Lexer\out.txt)");
}

int main() {
    // InitFA();
    LexerTest();

    getchar();
    return 0;
}


