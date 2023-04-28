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
// 定义串的最大长度
#define MAXLEN 4096

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
    void Init(const std::string& input_file, const std::string& output_file);
    void Tokenize();
    Token Gettoken();   // 获取一个token
    char GetNextChar(); // 获取下一个字符
    std::string GetMorpheme(); // 通过开始指针和向前指针获取词素
    void RollBack();    // 回退一个字符

private:
    size_t line_no_, column_;  // 行号, 列号
    size_t lexeme_beginning_;   // 开始指针
    size_t forward_;     // 向前指针
    std::ifstream in_;    // 输入流
    std::ofstream out_;   // 输出流

private:
    std::vector<Token> tokens;

    char line_buffer_[2*BUFLEN]{};
};


#endif //COMPILER_LEXER_H
