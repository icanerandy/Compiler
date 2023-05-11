/**
  ******************************************************************************
  * @file           : Instruction.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/9
  ******************************************************************************
  */
//
// Created by icaner on 2023/5/9.
//

#include "../include/Instruction.h"

Instruction Instruction::LDC("ldc");
Instruction Instruction::GETSTATIC("getstatic");
Instruction Instruction::PUTSTATIC("putstatic");
Instruction Instruction::SIPUSH("sipush");
Instruction Instruction::IADD("iadd");
Instruction Instruction::ISUB("isub");
Instruction Instruction::IMUL("imul");
Instruction Instruction::IDIV("idiv");
Instruction Instruction::IREM("irem");
Instruction Instruction::IAND("iand");
Instruction Instruction::IOR("ior");
Instruction Instruction::IF_EQ("ifeq");
Instruction Instruction::IF_ICMPEQ("if_icmpeq");
Instruction Instruction::IF_ICMPNE("if_icmpne");
Instruction Instruction::IF_ICMPLT("if_icmplt");
Instruction Instruction::IF_ICMPGE("if_icmpge");
Instruction Instruction::IF_ICMPGT("if_icmpgt");
Instruction Instruction::IF_ICMPLE("if_icmple");
Instruction Instruction::GOTO("goto");
Instruction Instruction::INVOKEVIRTUAL("invokevirtual");
Instruction Instruction::INVOKESTATIC("invokestatic");
Instruction Instruction::RETURN("return");
Instruction Instruction::IRETURN("ireturn");
Instruction Instruction::ILOAD("iload");
Instruction Instruction::ISTORE("istore");

Instruction::Instruction(std::string text)
    : text_(std::move(text))
{

}

std::string Instruction::ToString()
{
    return text_;
}