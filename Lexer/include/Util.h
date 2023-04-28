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
private:
    static std::unordered_map<std::string, int> keyword_list_;
};


#endif //COMPILER_UTIL_H
