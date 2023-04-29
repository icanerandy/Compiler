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

    // 1. 包含必要的头文件已在上方列出
    // 2. 打开文件并检查是否成功打开
    std::ifstream in(R"(D:\Project\CLion\Compiler\Lexer\regex.txt)", std::ios::in);
    if (!in.is_open())
    {
        std::cerr << "读取文件失败！" << std::endl;
        exit(-1);
    }

    // 3. 使用std::getline函数逐行读取文件
    std::string line;
    while (std::getline(in, line))
    {
        // 4. 使用std::stringstream将每行分割成单词
        std::stringstream ss(line);
        std::string category;

        // 5. 处理每个单词

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

    // 6. 关闭文件
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
        std::cout << "请输入一个字符串：" << std::endl;
        std::cin >> user_input;
        if (identifier_fa.Judge(user_input))
            std::cout << "这是一个标识符！" << std::endl;
        if (operator_fa.Judge(user_input))
            std::cout << "这是一个运算符！" << std::endl;
        if (decimal_fa.Judge(user_input))
            std::cout << "这是一个十进制数！" << std::endl;
        if (hex_fa.Judge(user_input))
            std::cout << "这是一个十六进制数！" << std::endl;
        if (oct_fa.Judge(user_input))
            std::cout << "这是一个八进制数！" << std::endl;
        if (float_fa.Judge(user_input))
            std::cout << "这是一个浮点数！" << std::endl;
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


