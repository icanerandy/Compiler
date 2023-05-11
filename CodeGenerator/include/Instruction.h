/**
  ******************************************************************************
  * @file           : Instruction.h
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/9
  ******************************************************************************
  */


#ifndef COMPILER_INSTRUCTION_H
#define COMPILER_INSTRUCTION_H

#include <string>

class Instruction
{
public:
    static Instruction LDC;
    static Instruction GETSTATIC;
    static Instruction PUTSTATIC;
    static Instruction SIPUSH;
    static Instruction IADD;
    static Instruction ISUB;
    static Instruction IMUL;
    static Instruction IDIV;
    static Instruction IREM;
    static Instruction IAND;
    static Instruction IOR;
    static Instruction IF_EQ;
    static Instruction IF_ICMPEQ;
    static Instruction IF_ICMPNE;
    static Instruction IF_ICMPLT;
    static Instruction IF_ICMPGE;
    static Instruction IF_ICMPGT;
    static Instruction IF_ICMPLE;
    static Instruction GOTO;
    static Instruction INVOKEVIRTUAL;
    static Instruction INVOKESTATIC;
    static Instruction RETURN;
    static Instruction IRETURN;
    static Instruction ILOAD;
    static Instruction ISTORE;

public:
    explicit Instruction(std::string text);
    std::string ToString();

private:
    std::string text_;
};


#endif //COMPILER_INSTRUCTION_H
