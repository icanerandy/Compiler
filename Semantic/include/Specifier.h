/**
  ******************************************************************************
  * @file           : Specifier.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/6
  ******************************************************************************
  */


#ifndef COMPILER_SPECIFIER_H
#define COMPILER_SPECIFIER_H


class Specifier
{
public:
    // type
    static int INT;
    static int CHAR;
    static int FLOAT;
    static int VOID;
    static int LABEL;

    // storage
    static int FIXED;
    static int REGISTER;
    static int AUTO;
    static int TYPEDEF;
    static int CONSTANT;

    static int NO_OCLASS;   // 如果内存类型是auto，那么存储类型就是NO_OCLASS
    static int PUBLIC;
    static int PRIVATE;
    static int EXTERN;
    static int COMMON;

private:
    int basic_type_{};
    int storage_class_{};
    int output_class_{};

private:
    bool is_long_;  // 表示当前变量占据多大字节，long int
    bool is_signed_;
    bool is_static_{};
    bool is_external_{};

private:
    int constant_value_;


public:
    Specifier();
    void SetType(int type);
    int GetType() const;
    void SetStorageClass(int s);
    int GetStorageClass() const;
    void SetOutputClass(int c);
    int GetOutputClass() const;
    void SetLong(bool l);
    bool IsLong() const;
    void SetSign(bool sign);
    bool IsSigned() const;
    void SetStatic(bool s);
    bool IsStatic() const;
    void SetExternal(bool e);
    bool IsExternal() const;
    void SetConstantVal(int v);
    int GetConstantVal() const;
};


#endif //COMPILER_SPECIFIER_H
