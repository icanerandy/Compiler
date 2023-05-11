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
    int declare_type_;  // ���η����ͣ�ָ�롢����
    int number_of_elements_; // ����Ԫ����Ŀ

public:
    explicit Declarator(int type);

public:
    void SetElementNum(int num);
    int GetType() const;
};


#endif //COMPILER_DECLARATOR_H
