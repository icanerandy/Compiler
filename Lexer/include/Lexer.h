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
#include <sstream>
#include <ostream>

#include "Util.h"
#include "FA.h"

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
    char LookPreChar(); // 查看下一个字符，不移动指针
    char GetNextChar(); // 获取下一个字符

private:
    bool DFADriver(DFA& dfa);    // DFA驱动程序
    void RollBack();    // 回退一个字符
    std::string GetMorpheme(); // 通过开始指针和向前指针获取词素
    static bool IsBlankChar(char ch); // 判断是否是空白字符
    static bool IsLetter(char ch); // 判断是否为a-z A-Z _

private:
    bool DealBlankChar(char ch);   // 处理空白字符
    bool DealSingleLineComment(); // 处理单行注释
    bool DealMultiLineComment();    // 处理多行注释
    bool DealChar();    // 处理字符型变量
    bool DealString();  // 处理字符串型变量
    void DealError();   // 简单的一个错误处理程序

private:
    DFA identifier_dfa_;
    DFA decimal_dfa_;
    DFA hex_dfa_;
    DFA oct_dfa_;
    DFA bin_dfa_;
    DFA float_dfa_;
    // 此程序不实现下面的DFA，因为符号集太大，其正则构成NFA边数太太太太太太太太多！影响效率，且词法分析中的注释本身就很好识别
    // DFA char_dfa_;
    // DFA string_dfa_;
    // DFA comment_dfa_;
    // 不实现操作符的正则，因为太简单了，而且需要确切的类别
    DFA operator_dfa_;

private:
    size_t line_no_, column_no_;  // 行号, 列号
    size_t last_column_no_{}; // 记录换行前的列号（便于回溯）
    char line_buffer_[2*BUFLEN]{};  // 双缓冲区
    size_t lexeme_beginning_{};   // 开始指针
    size_t forward_{};     // 向前指针

private:
    std::ifstream in_;    // 输入流
    std::ofstream out_;   // 输出流

public:
    std::vector<Token> tokens_;

};


#endif //COMPILER_LEXER_H
