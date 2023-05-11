/**
  ******************************************************************************
  * @file           : Declarator.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/6
  ******************************************************************************
  */


#ifndef COMPILER_DECLARATOR_H
#define COMPILER_DECLARATOR_H


class Declarator
{
public:
    static int POINTER;
    static int ARRAY;
    static int FUNCTION;

private:
    int declare_type_;  // 修饰符类型：指针、数组
    int number_of_elements_; // 数组元素数目

public:
    explicit Declarator(int type);

public:
    void SetElementNum(int num);
    int GetType() const;
};


#endif //COMPILER_DECLARATOR_H
