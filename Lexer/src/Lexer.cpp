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
    // ��ʼ����������ļ�
    in_.open(input_file, std::ios::in);
    if (!in_.is_open())
    {
        std::cerr << "���ļ�����" << std::endl;
        exit(-1);
    }

    out_.open(output_file, std::ios::out);
    if (!out_.is_open())
    {
        std::cerr << "���ļ�����" << std::endl;
        exit(-1);
    }

    // ��ʼ��������
    in_.read((char *)(line_buffer_), BUFLEN - 1);
    line_buffer_[in_.gcount()] = EOF;   // �ڱ�Ԫ��

    lexeme_beginning_ = forward_ = 0;

    // ��ʼ������״̬��

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
    // ���Կ������EOF����EOF���ݻ�����תΪ"#"��Ϊ�������Ź��﷨����ʹ��
    token = tokens_.at(tokens_.size() - 1);
    out_ << "(" << token.line << ":" << token.column << ")\t\t" << "{" << token.content << ", " << "#" << "}\n";

    std::cout << "�ʷ�������ɣ�" << std::endl;
    out_.flush();
    out_.close();
}

/*!
 * ��Ҫ��Ϊ���¼��ࣺ
 * 1. �հ׷�
 * 2. ע�� ���� ���С�����
 * 3. ������ ���� ���ַ����ַ���
 * 4. ��ʶ�������ؼ��֣�
 * 5. ����
 * 6. ����������
 * @return
 */
Token Lexer::Gettoken()
{
    bool deal_success;
    Token token;
    char next_char = LookPreChar();

    while (IsBlankChar(next_char)) // �հ׷�
    {
        GetNextChar();
        next_char = LookPreChar();
    }

    lexeme_beginning_ = forward_;   // �ʼ��ָ��ָ��ͬһ��ʼλ�ã�ÿ����һ���ַ���forward++��forward-beginning��Ϊ���صĳ���

    // ��¼����δ�����ǵ�����ֵ
    size_t column = column_no_, lineno = line_no_;

    if (next_char == EOF)
    {
        token.type = "EOF";
        token.content = "EOF";
        token.column = column;
        token.line  = lineno;

        return token;
    }

    if (next_char == '/')  // ע��
    {
        GetNextChar();  // ��'/'���룬Ȼ��forwardָ���˵�һ��/����
        next_char = LookPreChar();
        if (next_char == '/')  // ����ע��
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
        else if (next_char == '*') // ����ע��
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
        else    // ����͵���һ�������� / �ţ�����
        {
            deal_success = true;
            token.type = "OOP";
            token.content = GetMorpheme();
            token.column = column;
            token.line  = lineno;
        }
    }

    else if (next_char == '\'') // ���ַ�
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
    else if (next_char == '"')  // �ַ���
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

    else if (IsLetter(next_char))   // letter��ͷ��Ϊ��ʶ����ؼ���
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
        if (next_char == '0')  // 8���ƻ�16���ƻ򸡵���
        {
            GetNextChar();
            // �ٳ�ǰ��ȡһλ�������ع�����Ϊ״̬���ӳ�̬��ʼ����
            next_char = LookPreChar();
            RollBack();
            if (next_char == 'x' || next_char == 'X') // ʮ������
                deal_success = DFADriver(hex_dfa_);
            else if (next_char == 'b' || next_char == 'B')
            {
                forward_ = lexeme_beginning_;
                deal_success = DFADriver(bin_dfa_);
            }
            else    // 8���ƻ򸡵���
            {
                // �Ȱ��ո������������ƥ�両����ʧ�ܣ���Ϊ�˽�����
                deal_success = DFADriver(float_dfa_);
                if (!deal_success)
                {
                    forward_ = lexeme_beginning_;
                    deal_success = DFADriver(oct_dfa_);
                }
            }
        }
        else    // 10���ƻ򸡵���
        {
            // �Ȱ��ո������������ƥ�両����ʧ�ܣ���Ϊʮ������
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

    else    // ����Ķ����ߵ�����
    {
        std::string op;
        deal_success = DFADriver(operator_dfa_);
        if (deal_success)
        {
            op = GetMorpheme();
            // BOP ����������: &&  ||
            // COP �Ƚϲ�����: <	<=  >	>=	==	!=
            // AOP ��ֵ������: =   +=  -=  *=  /=  %=
            // OOP ���������: +	-	*	/   %     &   |
            // EOP ��ĩ������: ;
            // SOP �ṹ�ָ���: (	)	,	[	]	{	}
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

    // ������
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
 * ˫������
 * @return
 */
char Lexer::GetNextChar()
{
    switch (line_buffer_[forward_])
    {
        case EOF:
            if (forward_ == BUFLEN - 1)
            {
                // ���صڶ���������
                in_.read((char *)(line_buffer_ + BUFLEN), BUFLEN - 1);
                line_buffer_[BUFLEN + in_.gcount()] = EOF;
                forward_ = BUFLEN;
            }
            else if (forward_ == 2 * BUFLEN - 1)
            {
                // ���ص�һ��������
                in_.read((char *)(line_buffer_), BUFLEN - 1);
                line_buffer_[in_.gcount()] = EOF;
                forward_ = 0;
            }
            else
            {
                // �������ڲ��� EOF������������
                // ��ֹ�ʷ�����
                return EOF;
            }
        //case �����ַ����
        default:
        {

        }
    }

    // �����к��к�
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
 * �ж��Ƿ��ǿհ��ַ�
 * @param ch - Ҫ�жϵ��ַ�
 * @return - �ǿհ��ַ�����true������false
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
 * �жϵ�ǰ�ַ��Ƿ�����ĸ��_
 * @param ch - Ҫ�жϵ��ַ�
 * @return - �������ĸ��_����true������false
 */
bool Lexer::IsLetter(char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '_');
}

/*!
 * DFA����������
 * ���̣�
 * 1. �жϵ�ǰ״̬�Ƿ�Ϊ����̬
 *  * ����̬�����ջ��ѹ�뵱ǰ״̬
 *  * �ǽ���̬��ֱ��ѹ�뵱ǰ״̬
 * 2. ��ת��
 * @param dfa - ��Ӧ״̬ת��ͼ��dfa
 * @return - ���ٵ���һ����̬��ƥ��ɹ�������ƥ��ʧ��
 */
bool Lexer::DFADriver(DFA& dfa)
{
    State start_state = dfa.GetStartState();    // ��ʼ״̬
    std::set<State> end_state_set = dfa.GetEndStateSet();    // ����״̬��
    std::map<State, std::map<char, State>>& dfa_table = dfa.GetDFATable();  // ״̬ת��ͼ��Ӧ��״̬ת����

    State state = start_state;
    std::stack<State> state_stack;  // ʵ����Զƥ��

    char ch;
    while (state != 99999999)  // ��ǰ״̬��Ϊ����״̬
    {
        // ��ǰ״̬Ϊ����״̬�����״̬ջ
        if (end_state_set.find(state) != end_state_set.end())
            while (!state_stack.empty()) state_stack.pop();
        state_stack.push(state);

        ch = GetNextChar();
        if (dfa_table[state].find(ch) == dfa_table[state].end())    // ����״̬����ǰ״̬���ܽ��ո÷���
            state = 99999999;
        else
            state = dfa_table[state].at(ch);
    }

    // ��ǰ״̬��Ϊ����״̬
    while (!state_stack.empty() && end_state_set.find(state) == end_state_set.end())
    {
        state = state_stack.top();
        state_stack.pop();
        RollBack();
    }

    // �������EOF���µ�ǰ״̬Ϊ����״̬������ع�һ�Σ���Ҫ��forwardǰ��
    if (ch == EOF)
        GetNextChar();

    // ���ȡ����ջ�ڵ�Ԫ�ض�û�ҵ���̬����ǰdfa����ƥ��
    if (state_stack.empty() && end_state_set.find(state) == end_state_set.end())
        return false;
    else
        return true;
    // ����lexeme_beginningָ����ؿ�ͷ��forwardָ����ؽ�β���������һ��Ԫ�أ�
}

bool Lexer::DealSingleLineComment()
{
    // ������ע�Ͳ����ܳ���

    GetNextChar();  // ��ʱforwardָ��//��һ���ַ�
    char ch = GetNextChar();

    while (ch != '\n' && ch != EOF)
        ch = GetNextChar();

    if (ch == EOF)
    {
        // ʲô������Ҫ��
    }

    return true;
}

bool Lexer::DealMultiLineComment()
{
    GetNextChar();  // ִ�к�forwardָ��/*��һ������

    char cur, pre;

    do {
        cur = GetNextChar();  // һ���ַ�һ���ַ��ı��������ָ��EOF���ļ�����

        pre = LookPreChar();

        if (cur == EOF)
        {
            // ��������ע�ͳ������������û���ҵ���Ӧ���Ҳ�
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

    // char ch = ''; �յ��ַ������ڣ��Ǵ���ģ�'EOFҲ�������жϴ���
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

    // std::string s = ""; �մ����ڣ�����ȷ�ģ�'EOFҲ�������жϴ���
    while (cur != '"')
    {
        cur = GetNextChar();    // һ��һ���ַ��ƶ�

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
    // TODO: ���ƻ��˲���
    /*
     * ��˫�������ʷ��������У����л��˲�������һ�ζ�ȡ�ļ�ʱ���°뻺���������ݿ��ܻᱻ���ǵ���������Ϊ��˫������ʵ���У����ǻ���ÿ����������ĩβ��ȡ�µ��ַ���
     * ����Ҫ����������ʱ������Ҫȷ�����˲������ᵼ�����ݶ�ʧ��Ϊ�ˣ����Բ�ȡ���²��ԣ�
     * 1. ����Ҫ����ʱ���ȼ�鵱ǰ���˲����Ƿ�ᵼ�����ݶ�ʧ��������˲������漰��ǰ�����������ᵼ�����ݶ�ʧ����ô���Է��Ľ��л��ˡ�
     * 2. ������˲����漰���绺��������Ҫ����С�ġ�����������£������ڻ��˲���֮ǰ���浱ǰforwardָ���λ�á�
     *    ���´���Ҫ����������ʱ�����Խ������ݼ��ص���һ�����������ǵ�ǰ�Ļ��������������ݱ����forwardָ��λ�ý��ж�ȡ�����������˲������ᵼ�����ݶ�ʧ��
     * 3. ������˲�����Ҫ��Խ����������߽磬��ô������Ҫ����ʹ�ø���Ļ�����������������ԣ�����ѭ����������
     * ��ע�⣬��Щ���Կ��ܻ����Ӵʷ���������ʵ�ָ����ԡ���ʵ��Ӧ���У����Ը��ݾ�������ͳ���ѡ����ʵĲ��ԡ�
     * ���⣬���ڴ�������ԣ����˲���ͨ�����漰�����ַ�����˲�̫���ܳ��ֻ��˲������´������ݶ�ʧ�����⡣
     */
    forward_--;

    // ������������
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
            // ����һ�п�ͷ�ƻ���һ��ĩβ�����ɣ�
            // ��˵����һ��ĩβ�͵ڶ��п�ͷ���������ָ���ˣ���һ��token�����Ǵ�һ�е�ͷ����ȡ������һ�е�β������
            // ������ջ��к�������������Ҫ��column_no_��Ϊ�ϴε�column_no_
            column_no_ = last_column_no_;
            line_no_--;
            break;
        default:
            column_no_--;
    }
}

/*!
 * һ���򵥵Ĵ�������������Ϊ����������forwardָ������������ֵĵ�һ���հ��ַ���
 */
void Lexer::DealError()
{
    // TODO: forward��lexeme_beginning����ͬһ���ֻ�������ν����

    // ��ָ��lexeme_beginning��ʼ�ƶ�������forward_ָ���ֵ����Ϊlexeme_beginning
    forward_ = lexeme_beginning_;

    // ��ȡ�ַ���ֱ�������հ׷���
    char ch;
    do
    {
        ch = GetNextChar();
    } while (!IsBlankChar(ch) && ch != EOF);
}

