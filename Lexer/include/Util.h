/**
  ******************************************************************************
  * @file           : Util.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/28
  ******************************************************************************
  */


#ifndef COMPILER_UTIL_H
#define COMPILER_UTIL_H

#include <unordered_map>
#include <string>

class Util
{
public:
    static std::unordered_map<std::string, int> keyword_list_;
    static std::unordered_map<std::string, int> BOP;
    static std::unordered_map<std::string, int> COP;
    static std::unordered_map<std::string, int> AOP;
    static std::unordered_map<std::string, int> OOP;
    static std::unordered_map<std::string, int> EOP;
    static std::unordered_map<std::string, int> SOP;
};


#endif //COMPILER_UTIL_H
