/**
  ******************************************************************************
  * @file           : Lexer.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/28
  ******************************************************************************
  */


#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <istream>
#include <ostream>

#include "Util.h"

// 定义cminus的缓冲区
#define BUFLEN 4096

// token结构体
typedef struct Token
{
    std::string content;	// token原始内容
    std::string type;	    // token类型
    size_t line;			// token所在行
    size_t column;			// token所在列
} Token;

class Lexer
{
public:
    Lexer(const std::string& input_file, const std::string& output_file);

private:
    void OpenFile(const std::string& input_file, const std::string& output_file);
    Token Gettoken();   // 获取一个token
    char GetNextChar(); // 获取下一个字符

private:
    size_t line_no_;  // 行号
    size_t line_pos_;   // 行内位置，方便打印，是用来读取和回退位于源文件内指针的标记
    size_t buf_size_;   // 缓冲区内有效字符长度
    std::ifstream in_;    // 输入流
    std::ofstream out_;   // 输出流

    char line_buffer_[BUFLEN]{};
};


#endif //COMPILER_LEXER_H
