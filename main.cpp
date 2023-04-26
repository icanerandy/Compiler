#include <iostream>
#include "include/Grammar.h"
#include "include/LL1Parser.h"
#include "Lexer/FA.h"

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
    FA fa("(a|b)((c|d)*)");
}

int main() {
    RegexTest();

    getchar();
    return 0;
}


