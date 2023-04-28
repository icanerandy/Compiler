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
    column_(1)
{
    Init(input_file, output_file);

    Tokenize();
}

void Lexer::Init(const std::string &input_file, const std::string& output_file)
{
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
    line_buffer_[in_.gcount()] = EOF;

    lexeme_beginning_ = forward_ = 0;
    // 设置哨兵（sentinel）字符
//    line_buffer_[BUFLEN - 1] = EOF;
//    line_buffer_[2*BUFLEN - 1] = EOF;
}

void Lexer::Tokenize()
{

}

Token Lexer::Gettoken()
{
    return Token();
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
                break;
            }
        //case 其它字符情况
    }

    return line_buffer_[forward_++];
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
    --forward_;
}
