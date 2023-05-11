#include <iostream>
#include <sstream>
#include "include/LR1Parser.h"
#include "include/FA.h"
#include "include/Lexer.h"

std::map<std::string, std::string> ReadRegex()
{
    std::map<std::string, std::string> regex_map;

    std::ifstream in(R"(D:\Project\CLion\Compiler\Lexer\regex.txt)", std::ios::in);
    if (!in.is_open())
    {
        std::cerr << "¶ÁÈ¡ÎÄ¼þÊ§°Ü£¡" << std::endl;
        system("pause");
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

int main() {
    // InitFA();

    Lexer lexer(R"(..\Lexer\test.txt)",
                R"(..\Lexer\lexical_out.txt)");

    std::vector<Token> tokens = lexer.tokens_;
    LR1Parser lr1Parser(tokens,R"(..\Grammar\GrammarRules\testGrammar.txt)", R"(..\Grammar\lr1_parse_result.txt)");

    ASTTree * tree = lr1Parser.GetAST();

    std::cout << std::endl;
    system("pause");
    return 0;
}


