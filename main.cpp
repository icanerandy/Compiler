#include <iostream>
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

void RegexTest()
{
    FA fa(R"(if|ifif)");

    if (fa.Judge(R"(ififf)"))
        std::cout << "Accepted." << std::endl;
    else
        std::cout << "Rejected." << std::endl;
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


