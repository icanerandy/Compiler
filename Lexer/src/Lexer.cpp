/**
  ******************************************************************************
  * @file           : Lexer.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/28
  ******************************************************************************
  */

#include "../include/Lexer.h"

Lexer::Lexer(const std::string& input_file, const std::string& output_file)
    :
    line_no_(1),
    column_no_(1)
{
    Init(input_file, output_file);

    Tokenize();
}

void Lexer::Init(const std::string &input_file, const std::string& output_file)
{
    // 初始化输入输出文件
    in_.open(input_file, std::ios::in);
    if (!in_.is_open())
    {
        std::cerr << "打开文件错误！" << std::endl;
        exit(-1);
    }

    out_.open(output_file, std::ios::out);
    if (!out_.is_open())
    {
        std::cerr << "打开文件错误！" << std::endl;
        exit(-1);
    }

    // 初始化缓冲区
    in_.read((char *)(line_buffer_), BUFLEN - 1);
    line_buffer_[in_.gcount()] = EOF;   // 哨兵元素

    lexeme_beginning_ = forward_ = 0;

    // 初始化各类状态机

    FA identifier_fa("../Lexer/DFATables/identifier_DFA_table");
    FA operator_fa("../Lexer/DFATables/operator_DFA_table");
    FA decimal_fa("../Lexer/DFATables/decimal_DFA_table");
    FA hex_fa("../Lexer/DFATables/hex_DFA_table");
    FA oct_fa("../Lexer/DFATables/oct_DFA_table");
    FA bin_fa("../Lexer/DFATables/bin_DFA_table");
    FA float_fa("../Lexer/DFATables/float_DFA_table");

    identifier_dfa_ = identifier_fa.GetDFA();
    operator_dfa_ = operator_fa.GetDFA();
    decimal_dfa_ = decimal_fa.GetDFA();
    hex_dfa_ = hex_fa.GetDFA();
    oct_dfa_ = oct_fa.GetDFA();
    bin_dfa_ = bin_fa.GetDFA();
    float_dfa_ = float_fa.GetDFA();
}

void Lexer::Tokenize()
{
    line_no_ = 1;
    column_no_ = 1;

    Token token;
    do
    {
        token = Gettoken();
        if (token.type != "ERROR" && token.type != "NOTE")
            tokens_.emplace_back(token);
        else if (token.type == "ERROR")
            std::cout <<  "(" << token.line << ":" << token.column << ")\t\t" << "{" << token.content << ", " << token.type << "}\n";
    } while (token.type != "EOF");

    for (auto it = tokens_.begin(); it != tokens_.end() - 1; ++it)
    {
        Token cur = *it;
        Token pre = *(it + 1);
        if (cur.type == "ID" && pre.content == "(")
            it->type = "IDF";
        if (cur.type == "ID" && pre.content == "=")
            it->type = "ID1";

        out_ <<  "(" << it->line << ":" << it->column << ")\t\t" << "{" << it->content << ", " << it->type << "}\n";
    }
    // 可以考虑输出EOF，将EOF内容或类型转为"#"作为结束符号供语法分析使用
    token = tokens_.at(tokens_.size() - 1);
    out_ << "(" << token.line << ":" << token.column << ")\t\t" << "{" << token.content << ", " << "#" << "}\n";

    std::cout << "词法分析完成！" << std::endl;
    out_.flush();
    out_.close();
}

/*!
 * 主要分为以下几类：
 * 1. 空白符
 * 2. 注释 —— 单行、多行
 * 3. 字面量 —— 单字符、字符串
 * 4. 标识符（含关键字）
 * 5. 数字
 * 6. 操作符或界符
 * @return
 */
Token Lexer::Gettoken()
{
    bool deal_success;
    Token token;
    char next_char = LookPreChar();

    while (IsBlankChar(next_char)) // 空白符
    {
        GetNextChar();
        next_char = LookPreChar();
    }

    lexeme_beginning_ = forward_;   // 最开始两指针指向同一起始位置，每读入一个字符，forward++，forward-beginning即为词素的长度

    // 记录发生未处理是的行列值
    size_t column = column_no_, lineno = line_no_;

    if (next_char == EOF)
    {
        token.type = "EOF";
        token.content = "EOF";
        token.column = column;
        token.line  = lineno;

        return token;
    }

    if (next_char == '/')  // 注释
    {
        GetNextChar();  // 将'/'读入，然后forward指向了第一个/后面
        next_char = LookPreChar();
        if (next_char == '/')  // 单行注释
        {
            deal_success = DealSingleLineComment();
            if (deal_success)
            {
                token.type = "NOTE";
                token.column = column;
                token.line  = lineno;
            }
            else
            {
                token.type = "ERROR";
                std::stringstream ss;
                ss << "Single-line error occurred at (line: " << lineno << ", column: " << column << ")";
                token.content = ss.str();
                token.column = column;
                token.line  = lineno;
            }
        }
        else if (next_char == '*') // 多行注释
        {
            deal_success = DealMultiLineComment();
            if (deal_success)
            {
                token.type = "NOTE";
                token.column = column;
                token.line  = lineno;
            }
            else
            {
                token.type = "ERROR";
                std::stringstream ss;
                ss << "Multiple-line error occurred at (line: " << lineno << ", column: " << column << ")";
                token.content = ss.str();
                token.column = column;
                token.line  = lineno;
            }
        }
        else    // 否则就当作一个单独的 / 号，除号
        {
            deal_success = true;
            token.type = "OOP";
            token.content = GetMorpheme();
            token.column = column;
            token.line  = lineno;
        }
    }

    else if (next_char == '\'') // 单字符
    {
        deal_success = DealChar();
        if (deal_success)
        {
            token.type = "CCONST";
            token.content = GetMorpheme();
            token.column = column;
            token.line  = lineno;
        }
        else
        {
            token.type = "ERROR";
            std::stringstream ss;
            ss << "Single quotation marks error occurred at (line: " << lineno << ", column: " << column << ")";
            token.content = ss.str();
            token.column = column;
            token.line  = lineno;
        }
    }
    else if (next_char == '"')  // 字符串
    {
        deal_success = DealString();
        if (deal_success)
        {
            token.type = "SCONST";
            token.content = GetMorpheme();
            token.column = column;
            token.line  = lineno;
        }
        else
        {
            token.type = "ERROR";
            std::stringstream ss;
            ss << "Double quotation marks error occurred at (line: " << lineno << ", column: " << column << ")";
            token.content = ss.str();
            token.column = column;
            token.line  = lineno;
        }
    }

    else if (IsLetter(next_char))   // letter开头即为标识符或关键字
    {
        deal_success = DFADriver(identifier_dfa_);
        if (deal_success)
        {
            std::string content = GetMorpheme();
            if (Util::keyword_list_.find(content) != Util::keyword_list_.end())
                token.type = "RESERVED";
            else
                token.type = "ID";
            token.content = GetMorpheme();
            token.column = column;
            token.line  = lineno;
        }
        else
        {
            token.type = "ERROR";
            std::stringstream ss;
            ss << "Identifier error occurred at (line: " << lineno << ", column: " << column << ")";
            token.content = ss.str();
            token.column = column;
            token.line  = lineno;
        }
    }

    else if (isdigit(next_char))
    {
        if (next_char == '0')  // 8进制或16进制或浮点数
        {
            GetNextChar();
            // 再超前读取一位，读完后回滚，因为状态机从初态开始运行
            next_char = LookPreChar();
            RollBack();
            if (next_char == 'x' || next_char == 'X') // 十六进制
                deal_success = DFADriver(hex_dfa_);
            else if (next_char == 'b' || next_char == 'B')
            {
                forward_ = lexeme_beginning_;
                deal_success = DFADriver(bin_dfa_);
            }
            else    // 8进制或浮点数
            {
                // 先按照浮点数处理，如果匹配浮点数失败，则为八进制数
                deal_success = DFADriver(float_dfa_);
                if (!deal_success)
                {
                    forward_ = lexeme_beginning_;
                    deal_success = DFADriver(oct_dfa_);
                }
            }
        }
        else    // 10进制或浮点数
        {
            // 先按照浮点数处理，如果匹配浮点数失败，则为十进制数
            deal_success = DFADriver(float_dfa_);
            if (!deal_success)
            {
                forward_ = lexeme_beginning_;
                deal_success = DFADriver(decimal_dfa_);
            }
        }

        if (deal_success)
        {
            token.type = "NUM";
            token.content = GetMorpheme();
            token.column = column;
            token.line  = lineno;
        }
        else
        {
            token.type = "ERROR";
            std::stringstream ss;
            ss << "Number error occurred at (line: " << lineno << ", column: " << column << ")";
            token.content = ss.str();
            token.column = column;
            token.line  = lineno;
        }
    }

    else    // 其余的都会走到这里
    {
        std::string op;
        deal_success = DFADriver(operator_dfa_);
        if (deal_success)
        {
            op = GetMorpheme();
            // BOP 布尔操作符: &&  ||
            // COP 比较操作符: <	<=  >	>=	==	!=
            // AOP 赋值操作符: =   +=  -=  *=  /=  %=
            // OOP 运算操作符: +	-	*	/   %     &   |
            // EOP 句末操作符: ;
            // SOP 结构分隔符: (	)	,	[	]	{	}
            if (Util::BOP.find(op) != Util::BOP.end())
                token.type = "BOP";
            if (Util::COP.find(op) != Util::COP.end())
                token.type = "COP";
            if (Util::AOP.find(op) != Util::AOP.end())
                token.type = "AOP";
            if (Util::OOP.find(op) != Util::OOP.end())
                token.type = "OOP";
            if (Util::EOP.find(op) != Util::EOP.end())
                token.type = "EOP";
            if (Util::SOP.find(op) != Util::SOP.end())
                token.type = "SOP";

            token.content = GetMorpheme();
            token.column = column;
            token.line  = lineno;
        }
        else
        {
            token.type = "ERROR";
            std::stringstream ss;
            ss << "Symbol error occurred at (line: " << lineno << ", column: " << column << ")";
            token.content = ss.str();
            token.column = column;
            token.line  = lineno;
        }
    }

    // 错误处理
    if (!deal_success)
        DealError();

    return token;
}

char Lexer::LookPreChar()
{
    if (forward_ == BUFLEN - 1 || forward_ == 2*BUFLEN - 1)
        return (char)in_.peek();
    else
        return line_buffer_[forward_];
}

/*!
 * 双缓冲区
 * @return
 */
char Lexer::GetNextChar()
{
    switch (line_buffer_[forward_])
    {
        case EOF:
            if (forward_ == BUFLEN - 1)
            {
                // 重载第二个缓冲区
                in_.read((char *)(line_buffer_ + BUFLEN), BUFLEN - 1);
                line_buffer_[BUFLEN + in_.gcount()] = EOF;
                forward_ = BUFLEN;
            }
            else if (forward_ == 2 * BUFLEN - 1)
            {
                // 重载第一个缓冲区
                in_.read((char *)(line_buffer_), BUFLEN - 1);
                line_buffer_[in_.gcount()] = EOF;
                forward_ = 0;
            }
            else
            {
                // 缓冲区内部的 EOF，标记输入结束
                // 终止词法分析
                return EOF;
            }
        //case 其它字符情况
        default:
        {

        }
    }

    // 处理行号列号
    char ch = line_buffer_[forward_++];
    if (IsBlankChar(ch))
        DealBlankChar(ch);
    else
        column_no_++;

    return ch;
}

std::string Lexer::GetMorpheme()
{
    std::string morpheme;
    if (lexeme_beginning_ < BUFLEN)
    {
        if (forward_ <= BUFLEN)
        {
            morpheme = std::string(line_buffer_, lexeme_beginning_, forward_ - lexeme_beginning_);
        }
        else
        {
            morpheme = std::string(line_buffer_, lexeme_beginning_, BUFLEN - 1 - lexeme_beginning_);
            std::string tmp = std::string(line_buffer_, BUFLEN, forward_ - BUFLEN);
            morpheme += tmp;
        }
    }
    else
    {
        if (forward_ > BUFLEN)
        {
            morpheme = std::string(line_buffer_, lexeme_beginning_, forward_ - lexeme_beginning_);
        }
        else
        {
            morpheme = std::string(line_buffer_, lexeme_beginning_, 2*BUFLEN - 1 - lexeme_beginning_);
            std::string tmp = std::string(line_buffer_, forward_, BUFLEN - 1 - forward_);
            morpheme += tmp;
        }
    }


    return morpheme;
}

/*!
 * 判断是否是空白字符
 * @param ch - 要判断的字符
 * @return - 是空白字符返回true，否则false
 */
bool Lexer::IsBlankChar(char ch)
{
    switch (ch)
    {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            return true;
        default:
            return false;
    }
}

/*!
 * 判断当前字符是否是字母或_
 * @param ch - 要判断的字符
 * @return - 如果是字母或_返回true，否则false
 */
bool Lexer::IsLetter(char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '_');
}

/*!
 * DFA表驱动程序
 * 流程：
 * 1. 判断当前状态是否为接收态
 *  * 接收态：清空栈再压入当前状态
 *  * 非接收态：直接压入当前状态
 * 2. 做转移
 * @param dfa - 对应状态转换图的dfa
 * @return - 至少到达一个终态则匹配成功，否则匹配失败
 */
bool Lexer::DFADriver(DFA& dfa)
{
    State start_state = dfa.GetStartState();    // 开始状态
    std::set<State> end_state_set = dfa.GetEndStateSet();    // 结束状态集
    std::map<State, std::map<char, State>>& dfa_table = dfa.GetDFATable();  // 状态转换图对应的状态转换表

    State state = start_state;
    std::stack<State> state_stack;  // 实现最远匹配

    char ch;
    while (state != 99999999)  // 当前状态不为出错状态
    {
        // 当前状态为接收状态则清空状态栈
        if (end_state_set.find(state) != end_state_set.end())
            while (!state_stack.empty()) state_stack.pop();
        state_stack.push(state);

        ch = GetNextChar();
        if (dfa_table[state].find(ch) == dfa_table[state].end())    // 出错状态，当前状态不能接收该符号
            state = 99999999;
        else
            state = dfa_table[state].at(ch);
    }

    // 当前状态不为接收状态
    while (!state_stack.empty() && end_state_set.find(state) == end_state_set.end())
    {
        state = state_stack.top();
        state_stack.pop();
        RollBack();
    }

    // 如果接收EOF导致当前状态为出错状态，则会多回滚一次，需要是forward前进
    if (ch == EOF)
        GetNextChar();

    // 如果取完了栈内的元素都没找到终态，则当前dfa不能匹配
    if (state_stack.empty() && end_state_set.find(state) == end_state_set.end())
        return false;
    else
        return true;
    // 现在lexeme_beginning指向词素开头，forward指向词素结尾（词素最后一个元素）
}

bool Lexer::DealSingleLineComment()
{
    // 处理单行注释不可能出错

    GetNextChar();  // 此时forward指向//后一个字符
    char ch = GetNextChar();

    while (ch != '\n' && ch != EOF)
        ch = GetNextChar();

    if (ch == EOF)
    {
        // 什么都不需要做
    }

    return true;
}

bool Lexer::DealMultiLineComment()
{
    GetNextChar();  // 执行后forward指向/*后一个符号

    char cur, pre;

    do {
        cur = GetNextChar();  // 一个字符一个字符的遍历，如果指向EOF则文件结束

        pre = LookPreChar();

        if (cur == EOF)
        {
            // 出错，多行注释出错仅仅可能是没有找到对应的右部
            return false;
        }
    } while (!(cur == '*' && pre == '/'));
    GetNextChar();

    return true;
}

bool Lexer::DealChar()
{
    GetNextChar();
    char cur = GetNextChar();
    char pre = GetNextChar();

    // char ch = ''; 空单字符不存在，是错误的，'EOF也看可以判断处理
    if (cur != '\'' && pre == '\'')
    {
        return true;
    }

    return false;
}

bool Lexer::DealString()
{
    GetNextChar();
    char cur = GetNextChar();

    // std::string s = ""; 空串存在，是正确的，'EOF也看可以判断处理
    while (cur != '"')
    {
        cur = GetNextChar();    // 一个一个字符移动

        if (cur == EOF)
            return false;
    }

    return true;
}

bool Lexer::DealBlankChar(char ch)
{
    switch (ch)
    {
        case ' ':
            column_no_++;
            break;
        case '\t':
            column_no_ += 4;
            break;
        case '\r':
        case '\n':
            last_column_no_ = column_no_;
            column_no_ = 1;
            line_no_++;
            break;
        default:
            return false;
    }
    return true;
}

void Lexer::RollBack()
{
    // TODO: 完善回退操作
    /*
     * 在双缓冲区词法分析器中，进行回退操作后，下一次读取文件时，下半缓冲区的内容可能会被覆盖掉。这是因为在双缓冲区实现中，我们会在每个缓冲区的末尾读取新的字符。
     * 当需要加载新数据时，我们要确保回退操作不会导致数据丢失。为此，可以采取以下策略：
     * 1. 在需要回退时，先检查当前回退操作是否会导致数据丢失。如果回退操作仅涉及当前缓冲区，不会导致数据丢失，那么可以放心进行回退。
     * 2. 如果回退操作涉及到跨缓冲区，需要格外小心。在这种情况下，可以在回退操作之前保存当前forward指针的位置。
     *    当下次需要加载新数据时，可以将新数据加载到另一个缓冲区（非当前的缓冲区），并根据保存的forward指针位置进行读取。这样，回退操作不会导致数据丢失。
     * 3. 如果回退操作需要跨越多个缓冲区边界，那么可能需要考虑使用更大的缓冲区或其他缓冲策略，例如循环缓冲区。
     * 请注意，这些策略可能会增加词法分析器的实现复杂性。在实际应用中，可以根据具体需求和场景选择合适的策略。
     * 另外，对于大多数语言，回退操作通常仅涉及少量字符，因此不太可能出现回退操作导致大量数据丢失的问题。
     */
    forward_--;

    // 处理行列坐标
    char ch = line_buffer_[forward_];
    switch (ch)
    {
        case ' ':
            column_no_--;
            break;
        case '\t':
            column_no_ -= 4;
            break;
        case '\r':
        case '\n':
            // 从下一行开头移回上一行末尾如何完成？
            // 这说明第一行末尾和第二行开头被换行所分割开来了，而一个token最早是从一行的头部读取，最晚到一行的尾部结束
            // 比如接收换行后立即出错，则需要将column_no_改为上次的column_no_
            column_no_ = last_column_no_;
            line_no_--;
            break;
        default:
            column_no_--;
    }
}

/*!
 * 一个简单的错误处理程序：其机制为发生错误则将forward指针移向错误后出现的第一个空白字符处
 */
void Lexer::DealError()
{
    // TODO: forward和lexeme_beginning不在同一部分缓冲区如何解决？

    // 从指针lexeme_beginning开始移动，即将forward_指针的值设置为lexeme_beginning
    forward_ = lexeme_beginning_;

    // 读取字符，直到遇到空白符号
    char ch;
    do
    {
        ch = GetNextChar();
    } while (!IsBlankChar(ch) && ch != EOF);
}

