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
    line_no_(0),
    line_pos_(0),
    buf_size_(0)
{
    OpenFile(input_file, output_file);

    char ch;
    do {
        ch = GetNextChar();
        out_ << ch;
    } while (ch != EOF);
}

void Lexer::OpenFile(const std::string &input_file, const std::string& output_file)
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
}

Token Lexer::Gettoken()
{
    return Token();
}

char Lexer::GetNextChar()
{
    if (line_pos_ < buf_size_)  // 要读取的位置未超出缓冲区
    {
        return line_buffer_[line_pos_++];
    }
    else    // 超出缓冲区，读取BUFLEN个字符到缓冲区
    {
        if (in_.eof())
            return EOF;

        in_.get(line_buffer_, BUFLEN, EOF);
        buf_size_ = strlen(line_buffer_);
        line_pos_ = 0;
        return line_buffer_[line_pos_++];
    }
}
