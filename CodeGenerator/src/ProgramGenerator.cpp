
#include "../include/ProgramGenerator.h"

ProgramGenerator * ProgramGenerator::instance = nullptr;

ProgramGenerator * ProgramGenerator::GetInstance()
{
    if (instance == nullptr)
        instance = new ProgramGenerator();

    return instance;
}

ProgramGenerator::ProgramGenerator()
    : branch_count(0),
    loop_count(0)
{
    if_stmt.emplace(0);
    loop_stmt.emplace(0);
}

void ProgramGenerator::Generate()
{
    EmitDirective(Directive::CLASS_PUBLIC, program_name);
    EmitDirective(Directive::SUPER, "java/lang/Object");
}

void ProgramGenerator::GenerateMainMethod()
{
    EmitBlankLine();
    EmitDirective(Directive::METHOD_PUBLIC_STATIC, "main([Ljava/lang/String;)V");
}

void ProgramGenerator::finish()
{
    EmitBlankLine();
    EmitDirective(Directive::END_CLASS);

    assembly_file.close();
}

void ProgramGenerator::EmitBranchOut()
{
    std::string s = "branch_out" + std::to_string(if_stmt.top()) + ":" + "\n";
    EmitString(s);
}

std::string ProgramGenerator::GetBranchOut()
{
    std::string s = "branch_out" + std::to_string(if_stmt.top());
    return s;
}

std::string ProgramGenerator::GetBranch()
{
    std::string s = "branch" + std::to_string(if_stmt.top());
    return s;
}

void ProgramGenerator::IncreaseBranch()
{
    branch_count++;
}

void ProgramGenerator::IncreaseLoop()
{
    loop_count++;
}

std::string ProgramGenerator::GetLoop()
{
    std::string s = "loop" + std::to_string(loop_stmt.top());
    return s;
}
