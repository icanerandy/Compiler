/**
  ******************************************************************************
  * @file           : AST.cpp
  * @author         : icaner
  * @brief          : None
  * @attention      : None
  * @date           : 2023/5/4
  ******************************************************************************
  */
//
// Created by icaner on 2023/5/4.
//

#include <utility>
#include "include/AST.h"


ASTNode::ASTNode()
{
    content_ = "";	// token原始内容
    type_ = "";	    // token类型
    res_type_ = "";
    line_no_ = 0;
    column_no_ = 0;

    is_int_ = true;
    i_value_ = 0;
    f_value_ = 0;
    s_value_ = "";
    c_value_ = 0;

    children_.clear();
    symbol_ = nullptr;
    pre_ = nullptr;
    next_ = nullptr;
    parent_ = nullptr;
}

ASTTree::ASTTree(ASTNode * root)
        :   root_(root)
{

}

ASTTree::~ASTTree()
= default;

ASTNode::~ASTNode()
= default;

void ASTNode::SetParents(ASTNode* parent)
{
    parent_ = parent;
}

size_t ASTNode::GetChildrenSize() const
{
    return children_.size();
}

bool ASTNode::IsRoot() const
{
    return (parent_ == nullptr);
}

bool ASTNode::IsLeaf() const
{
    return children_.empty();
}

std::list<struct ASTNode *>& ASTNode::GetChildrenList()
{
    return children_;
}

struct ASTNode *ASTNode::GetParent() const
{
    return parent_;
}

void ASTNode::SetAttributeForLeaf(Symbol *symbol, std::string content)
{
    symbol_ = symbol;
    content_ = std::move(content);
}

Symbol *ASTNode::GetSymbol()
{
    return symbol_;
}

void ASTTree::AddTypeSystem(TypeSystem * typeSystem)
{
    typeSystem_ = typeSystem;
}

void ASTTree::print_node(const ASTNode *node, int level, const std::string &indent, bool is_last_sibling) {
    if (node == nullptr) {
        return;
    }

    // 输出当前节点
    tree_out_ << indent;
    if (level > 0) {
        tree_out_ << (is_last_sibling ? "└──" : "├──");
    }
    if (node->type_ == "<函数名>" || node->type_ == "<常量>" || node->type_ == "<变量>"
        || node->type_ == "<常数>" || node->type_ == "<字符常量>" || node->type_ == "<字符串常量>")
        tree_out_ << node->content_ << "\n";
    else
        tree_out_ << node->type_ << "\n";

    // 计算子节点的缩进
    std::string child_indent = indent + (level > 0 ? (is_last_sibling ? "    " : "│   ") : "");

    // 递归输出子节点
    size_t num_children = node->children_.size();
    size_t count = 0;
    for (const auto &child : node->children_) {
        print_node(child, level + 1, child_indent, ++count == num_children);
    }
}

void ASTTree::PrettyPrint() {
    if (root_ == nullptr) {
        return;
    }

    tree_out_.open("../Grammar/tree_out.txt", std::ios::out);
    if (!tree_out_.is_open())
    {
        std::cerr << "无法打开文件将语法树输出到文件！" << std::endl;
        exit(-1);
    }

    // 使用辅助函数从根节点开始递归输出
    print_node(root_, 0, "", true);

    tree_out_.flush();
    tree_out_.close();
}

void ASTTree::RemoveEmptyNodes(ASTNode* node) {
    if (node == nullptr) {
        return;
    }

    // 递归处理子节点
    for (auto it = node->children_.begin(); it != node->children_.end(); ) {
        if (*it == nullptr) {
            delete *it;
            it = node->children_.erase(it);
        } else {
            RemoveEmptyNodes(*it);
            ++it;
        }
    }

    // 继续处理子节点的子节点
    for (auto child : node->children_) {
        RemoveEmptyNodes(child);
    }
}

void ASTTree::RemoveEmptyNodes()
{
    RemoveEmptyNodes(root_);
}

ASTNode * ASTTree::ExecuteNode(ASTNode * root)
{
    if (root == nullptr)
        return nullptr;

    std::string type = root->type_;
    if (type == "if语句")
    {
        interpreter_ << "执行if语句：\n";
        auto iter0 = root->GetChildrenList().begin();
        ExecuteNode(*iter0);
        auto iter1 = root->GetChildrenList().begin();
        std::advance(iter1, 1);

        std::list<ASTNode *>::iterator iter2;
        if (root->GetChildrenList().size() > 2)
        {
            iter2 = root->GetChildrenList().begin();
            std::advance(iter2, 2);
        }
        if ((*iter0)->i_value_)
            ExecuteNode(*iter1);
        else
            if (root->GetChildrenList().size() > 2)
                ExecuteNode(*iter2);
        interpreter_ << "if语句结束\n";
        interpreter_.flush();
    }
    else if (type == "for语句")
    {
        interpreter_ << "执行for语句：\n";
        ExecuteNode(root->GetChildrenList().front());   // 执行赋初值语句
        auto iter1 = root->GetChildrenList().begin();
        std::advance(iter1, 1);
        auto iter2 = root->GetChildrenList().begin();
        std::advance(iter2, 2);
        auto iter3 = root->GetChildrenList().begin();
        std::advance(iter3, 3);

        int time = 1;
        ExecuteNode(*iter1);
        while ((*iter1)->i_value_)
        {
            interpreter_ << "第" << time++ << "次：\n";
            ExecuteNode(*iter3);
            ExecuteNode(*iter2);
            ExecuteNode(*iter1);
        }
        interpreter_ << "for语句结束\n";
        interpreter_.flush();
    }
    else if (type == "while语句")
    {
        interpreter_ << "执行while语句：\n";
        auto iter1 = root->GetChildrenList().begin();
        auto iter2 = root->GetChildrenList().begin();
        std::advance(iter2, 1);

        int time = 1;
        ExecuteNode(*iter1);
        while ((*iter1)->i_value_)
        {
            interpreter_ << "第" << time++ << "次：\n";
            ExecuteNode(*iter2);
            ExecuteNode(*iter1);
        }
        interpreter_ << "while语句结束\n";
        interpreter_.flush();
    }
    else if (type == "do-while语句")
    {
        interpreter_ << "执行do-while语句：\n";
        auto iter1 = root->GetChildrenList().begin();
        auto iter2 = root->GetChildrenList().begin();
        std::advance(iter2, 1);

        int time = 1;
        do
        {
            interpreter_ << "第" << time++ << "次：\n";
            ExecuteNode(*iter1);
            ExecuteNode(*iter2);
        } while ((*iter2)->i_value_);
        interpreter_ << "do-while语句结束\n";
        interpreter_.flush();
    }
    else if (type == "函数调用")
    {
        // 内置函数的简陋实现
        if (root->GetChildrenList().front()->content_ == "printf")
        {
            // TODO: 解释器的内置函数调用
            std::list<ASTNode *> args;
            auto iter = root->GetChildrenList().begin();
            std::advance(iter, 1);
            size_t idx = 1;
            while (idx++ < root->GetChildrenSize())
            {
                ExecuteNode(*iter);
                args.emplace_back(*iter);
                std::advance(iter, 1);
            }

            std::string arg_str = args.front()->s_value_;
            arg_str.erase(std::find(arg_str.begin(), arg_str.end(), '"'));
            arg_str.erase(std::find(arg_str.begin(), arg_str.end(), '"'));
            args.pop_front();
            std::string format_str;

            int i = 0;
            while (i < arg_str.length())
            {
                if (arg_str.at(i) == '%' && (i+1) < arg_str.length()
                    && arg_str.at(i+1) == 'd')
                {
                    ASTNode * tmp = args.front();
                    args.pop_front();
                    format_str.append(std::to_string(tmp->i_value_));
                    i+=2;
                }
                else
                {
                    format_str.append(1, arg_str.at(i));
                    ++i;
                }
            }

            std::cout << format_str << std::endl;
        }
        // 普通函数调用
        else
        {
            interpreter_ << "执行函数调用：" << root->GetChildrenList().front()->content_ << "\n";
            Symbol * func_symbol = root->GetChildrenList().front()->GetSymbol();
            auto * func_entry = (ASTNode *)func_symbol->body_;
            Symbol * func_args = func_symbol->args_;

            func_call_stack.emplace(root, true);
            if (func_args == nullptr)   // 无参数函数调用
            {
                ExecuteNode(func_entry);
            }
            else    // 有参数函数调用
            {
                auto tmp = func_args;
                auto iter = root->GetChildrenList().begin();
                while (tmp)
                {
                    std::advance(iter, 1);
                    ExecuteNode(*iter);
                    tmp = tmp->GetNextSymbol();
                }

                std::list<Symbol *> * local_vars = GetArgsByScope(runtime_scope_.top());
                runtime_scope_.emplace(func_symbol->name_);
                SaveArgs(local_vars);   // 递归调用函数保护参数环境

                iter = root->GetChildrenList().begin();
                tmp = func_args;
                while (tmp)
                {
                    std::advance(iter, 1);
                    PassValueToFuncArg(tmp, *iter);
                    tmp = tmp->GetNextSymbol();
                }

                ExecuteNode(func_entry);
                RestoreArgs(local_vars); // 恢复保护参数堆栈
                runtime_scope_.pop();
            }

            func_call_stack.pop();
            interpreter_ << "函数调用结束\n";
            interpreter_.flush();
        }
    }
    else if (type == "return语句")
    {
        if (!root->GetChildrenList().empty())
        {
            ExecuteNode(root->GetChildrenList().front());
            CpyChildToParent(func_call_stack.top().first, root->GetChildrenList().front());
            func_ret_val.emplace_back(func_call_stack.top().first->i_value_);
        }

        interpreter_ << "执行return语句\t返回值：" << func_call_stack.top().first->i_value_ << "\n";
        func_call_stack.top().second = false;
        interpreter_.flush();
    }
    else
    {
        for (auto child : root->GetChildrenList())
        {
            ExecuteNode(child);
            if (!func_call_stack.top().second)
                break;
        }
    }

    if (type == "表达式")
    {
        auto child = root->GetChildrenList().front();
        CpyChildToParent(root, child);

        if (child->type_ == "函数调用" /* 同时需要函数调用有返回值 */ && child->GetChildrenList().front()->content_ != "printf" && GetType(child) != "void" )
        {
            root->i_value_ = func_ret_val.front();
            func_ret_val.pop_front();
        }
        interpreter_.flush();
    }

    if (type == "INIT")
    {
        ASTNode * lhs = root->GetChildrenList().front();
        ASTNode * rhs = root->GetChildrenList().back();

        Symbol * symbol = lhs->GetSymbol();
        std::string var_type = GetType(lhs);

        if (rhs->type_ == "函数调用")
        {
            rhs->i_value_ = func_ret_val.front();
            func_ret_val.pop_front();
        }

        if (var_type == "String")
        {
            lhs->s_value_ = rhs->s_value_;
            symbol->SetValue(rhs->s_value_);
            interpreter_ << "执行初始化语句：Variable " << lhs->content_ << " is initialized to value of " << symbol->s_value_ << "\n";
            interpreter_.flush();
        }

        if (var_type == "char")
        {
            lhs->c_value_ = rhs->c_value_;
            symbol->SetValue(rhs->c_value_);
            interpreter_ << "执行初始化语句：Variable " << lhs->content_ << " is initialized to value of " << symbol->c_value_ << "\n";
            interpreter_.flush();
        }

        if (var_type == "int")
        {
            lhs->i_value_ = rhs->i_value_;
            symbol->SetValue(rhs->i_value_);
            interpreter_ << "执行初始化语句：Variable " << lhs->content_ << " is initialized to value of " << symbol->i_value_ << "\n";
            interpreter_.flush();
        }

        if (var_type == "float")
        {
            lhs->f_value_ = rhs->f_value_;
            symbol->SetValue(rhs->f_value_);
            interpreter_ << "执行初始化语句：Variable " << lhs->content_ << " is initialized to value of " << symbol->f_value_ << "\n";
            interpreter_.flush();
        }
    }

    if (type == "AOP")
    {
        ASTNode * lhs = root->GetChildrenList().front();
        ASTNode * rhs = root->GetChildrenList().back();

        Symbol * symbol = lhs->GetSymbol();
        std::string var_type = GetType(lhs);

        if (rhs->type_ == "函数调用")
        {
            rhs->i_value_ = func_ret_val.front();
            func_ret_val.pop_front();
        }

        if (var_type == "String")
        {
            lhs->s_value_ = rhs->s_value_;
            symbol->SetValue(rhs->s_value_);
            CpyChildToParent(root, lhs);
            interpreter_ << "执行赋值语句：Variable " << lhs->content_ << " is assigned to value of " << symbol->s_value_ << "\n";
            interpreter_.flush();
        }

        if (var_type == "char")
        {
            lhs->c_value_ = rhs->c_value_;
            symbol->SetValue(rhs->c_value_);
            CpyChildToParent(root, lhs);
            interpreter_ << "执行赋值语句：Variable " << lhs->content_ << " is assigned to value of " << symbol->c_value_ << "\n";
            interpreter_.flush();
        }

        if (var_type == "int")
        {
            int res = AOP(root->content_, lhs->i_value_, rhs->i_value_);
            symbol->SetValue(res);
            CpyChildToParent(root, lhs);
            interpreter_ << "执行赋值语句：Variable " << lhs->content_ << " is assigned to value of " << res << "\n";
            interpreter_.flush();
        }

        if (var_type == "float")
        {
            float res = AOP(root->content_, lhs->f_value_, rhs->f_value_);
            symbol->SetValue(res);
            CpyChildToParent(root, lhs);
            interpreter_ << "执行赋值语句：" << lhs->content_ << " is assigned to value of " << res << "\n";
            interpreter_.flush();
        }
    }

    if (type == "OOP")
    {
        ASTNode * lhs = root->GetChildrenList().front();
        ASTNode * rhs = root->GetChildrenList().back();

        if (lhs->type_ == "函数调用")
        {
            lhs->i_value_ = func_ret_val.front();
            func_ret_val.pop_front();
        }

        if (rhs->type_ == "函数调用")
        {
            rhs->i_value_ = func_ret_val.front();
            func_ret_val.pop_front();
        }

        if (lhs->is_int_)
        {
            root->i_value_ = OOP(root->content_, lhs->i_value_, rhs->i_value_);
            root->is_int_ = true;
            interpreter_ << "执行算数运算：" << lhs->i_value_ << root->content_ << rhs->i_value_;
            interpreter_ << "\tresult is " << root->i_value_ << "\n";
            interpreter_.flush();
        }
        else    // float
        {
            root->f_value_ = OOP(root->content_, lhs->f_value_, rhs->f_value_);
            root->is_int_ = false;
            interpreter_ << "执行算数运算：" << lhs->f_value_ << root->content_ << rhs->f_value_ << "\n";
            interpreter_ << "\tresult is " << root->f_value_ << "\n";
            interpreter_.flush();
        }
    }

    if (type == "COP")
    {
        ASTNode * lhs = root->GetChildrenList().front();
        ASTNode * rhs = root->GetChildrenList().back();

        if (lhs->type_ == "函数调用")
        {
            lhs->i_value_ = func_ret_val.front();
            func_ret_val.pop_front();
        }

        if (rhs->type_ == "函数调用")
        {
            rhs->i_value_ = func_ret_val.front();
            func_ret_val.pop_front();
        }

        if (lhs->is_int_)
        {
            root->i_value_ = COP(root->content_, lhs->i_value_, rhs->i_value_);
            root->is_int_ = true;
            interpreter_ << "执行关系比较操作：" << lhs->i_value_ << " " << root->content_ << " " << rhs->i_value_ <<
                            "\tresult is: " << root->i_value_ << "\n";
            interpreter_.flush();
        }
        else    // float
        {
            root->i_value_ = COP(root->content_, lhs->f_value_, rhs->f_value_);
            root->is_int_ = false;
            interpreter_ << "执行关系比较操作：" << lhs->f_value_ << " " << root->content_ << " " << rhs->f_value_ <<
                            "\tresult is: " << root->i_value_ << "\n";
            interpreter_.flush();
        }
    }

    if (type == "BOP")
    {
        ASTNode * lhs = root->GetChildrenList().front();
        ASTNode * rhs = root->GetChildrenList().back();

        if (lhs->type_ == "函数调用")
        {
            lhs->i_value_ = func_ret_val.front();
            func_ret_val.pop_front();
        }

        if (rhs->type_ == "函数调用")
        {
            rhs->i_value_ = func_ret_val.front();
            func_ret_val.pop_front();
        }

        if (lhs->is_int_)
        {
            root->i_value_ = BOP(root->content_, lhs->i_value_, rhs->i_value_);
            root->is_int_ = true;
            interpreter_ << "执行布尔关系操作：" << lhs->i_value_ << " " << root->content_ << " " << rhs->i_value_ <<
                           "\tresult is: " << root->i_value_ << "\n";
            interpreter_.flush();
        }
        else    // float
        {
            root->i_value_ = BOP(root->content_, lhs->f_value_, rhs->f_value_);
            root->is_int_ = false;
            interpreter_ << "执行布尔关系操作：" << lhs->f_value_ << " " << root->content_ << " " << rhs->f_value_ <<
                            "\tresult is: " << root->i_value_ << "\n";
            interpreter_.flush();
        }
    }

    // 将符号表中的值拷贝到符号中，从符号表中更新节点中的值
    CpyValueToTreeNode(root);

    return root;
}

void ASTTree::ExecuteTree(Symbol * main_entry)
{
    interpreter_.open("../Grammar/interpreter_out.txt", std::ios::out);
    if (!interpreter_.is_open())
    {
        std::cerr << "无法打开文件将语法树输出到文件！" << std::endl;
        exit(-1);
    }

    func_call_stack.emplace(static_cast<ASTNode *>(main_entry->body_), true);
    interpreter_ << "执行MAIN函数调用：\n";
    runtime_scope_.emplace("main");
    ExecuteNode(static_cast<ASTNode *>(main_entry->body_));
    interpreter_ << "MAIN函数结束\n";

    interpreter_.flush();
    interpreter_.close();
}

void ASTTree::CpyChildToParent(ASTNode * parent, ASTNode * child)
{
    parent->is_int_ = child->is_int_;
    parent->s_value_ = child->s_value_;
    parent->c_value_ = child->c_value_;
    parent->i_value_ = child->i_value_;
    parent->f_value_ = child->f_value_;
}

void ASTTree::CpyValueToTreeNode(ASTNode *root)
{
    if (root->type_ == "<变量>")
    {
        Symbol * symbol = root->GetSymbol();

        // TODO: 实现错误提示
        if (symbol == nullptr)
            std::cerr << "未声明的变量" << std::endl;

        if (symbol->GetType() == "int")
            root->is_int_ = true;
        else
            root->is_int_ = false;

        root->s_value_ = symbol->s_value_;
        root->c_value_ = symbol->c_value_;
        root->i_value_ = symbol->i_value_;
        root->f_value_ = symbol->f_value_;
    }
}

std::string ASTTree::GetType(ASTNode *root)
{
    if (root->type_ == "<字符常量>")
        return "<字符常量>";

    if (root->type_ == "<字符串常量>")
        return "<字符串常量>";

    if (root->type_ == "<变量>")
        return root->GetSymbol()->GetType();

    if (root->type_ == "函数调用")
        return root->GetChildrenList().front()->GetSymbol()->GetType();

    if (root->type_ == "<常数>")
        return "<常数>";

    return "Error type";
}

int ASTTree::OOP(std::string op, int v1, int v2)
{
    int res = 0;

    switch (op.at(0))
    {
        case '+':
            res = v1 + v2;
            break;
        case '-':
            res = v1 - v2;
            break;
        case '*':
            res = v1 * v2;
            break;
        case '/':
            res = v1 / v2;
            break;
        case '%':
            res = v1 % v2;
            break;
        case '&':
            res = v1 & v2;
            break;
        case '|':
            res = v1 | v2;
            break;
    }

    return res;
}

// OOP 运算操作符: +	-	*	/   %     &   |
float ASTTree::OOP(std::string op, float v1, float v2)
{
    float res = 0;

    switch (op.at(0))
    {
        case '+':
            res = v1 + v2;
            break;
        case '-':
            res = v1 - v2;
            break;
        case '*':
            res = v1 * v2;
            break;
        case '/':
            res = v1 / v2;
            break;
    }

    return res;
}

// AOP 赋值操作符: =   +=  -=  *=  /=  %=
int ASTTree::AOP(const std::string& op, int& v1, int& v2)
{
    if (op == "=")
        v1 = v2;
    if (op == "+=")
        v1 += v2;
    if (op == "-=")
        v1 -= v2;
    if (op == "*=")
        v1 *= v2;
    if (op == "/=")
        v1 /= v2;
    if (op == "%=")
        v1 %= v2;

    return v1;
}

float ASTTree::AOP(const std::string& op, float& v1, float& v2)
{
    if (op == "=")
        v1 = v2;
    if (op == "+=")
        v1 += v2;
    if (op == "-=")
        v1 -= v2;
    if (op == "*=")
        v1 *= v2;
    if (op == "/=")
        v1 /= v2;

    return v1;
}

// COP 比较操作符: <	<=  >	>=	==	!=
int ASTTree::COP(const std::string& op, int v1, int v2)
{
    bool res;
    if (op == "<")
        res = (v1 < v2);
    if (op == "<=")
        res = (v1 <= v2);
    if (op == ">")
        res = (v1 > v2);
    if (op == ">=")
        res = (v1 >= v2);
    if (op == "==")
        res = (v1 == v2);
    if (op == "!=")
        res = (v1 != v2);

    return res ? 1 : 0;
}

int ASTTree::COP(const std::string& op, float v1, float v2)
{
    bool res;
    if (op == "<")
        res = (v1 < v2);
    if (op == "<=")
        res = (v1 <= v2);
    if (op == ">")
        res = (v1 > v2);
    if (op == ">=")
        res = (v1 >= v2);
    if (op == "==")
        res = (v1 == v2);
    if (op == "!=")
        res = (v1 != v2);

    return res ? 1 : 0;
}

int ASTTree::BOP(const std::string &op, int v1, int v2)
{
    bool res;
    if (op == "&&")
        res = (v1 && v2);
    if (op == "||")
        res = (v1 || v2);

    return res ? 1 : 0;
}

int ASTTree::BOP(const std::string &op, float v1, float v2)
{
    bool res;
    if (op == "&&")
        res = (v1 && v2);
    if (op == "||")
        res = (v1 || v2);

    return res ? 1 : 0;
}

void ASTTree::PassValueToFuncArg(Symbol *sym, ASTNode *node)
{
    std::string type = sym->GetType();

    if (type == "int")
    {
        sym->i_value_ = node->i_value_;
        interpreter_ << "传递参数: " << node->i_value_ << "\t";
    }
    if (type == "char")
    {
        sym->c_value_ = node->c_value_;
        interpreter_ << "传递参数: " << node->c_value_ << "\t";
    }
    if (type == "String")
    {
        sym->s_value_.assign(node->s_value_);
        interpreter_ << "传递参数: " << node->s_value_ << "\t";
    }
    if (type == "float")
    {
        sym->f_value_ = node->f_value_;
        interpreter_ << "传递参数: " << node->f_value_ << "\t";
    }
}

void ASTTree::SaveArgs(std::list<Symbol *> * local_vars)
{
    interpreter_ << "Save arguments...\n";
    auto iter = local_vars->begin();
    auto * cur = new Symbol();
    CpySymbolValue(*iter, cur);
    func_args_stack.emplace(cur);

    std::advance(iter, 1);
    while (iter != local_vars->end())
    {
        auto * cpy = new Symbol();
        CpySymbolValue(*iter, cpy);
        cur->next_ = cpy;
        cur = cpy;
        std::advance(iter, 1);
    }
    interpreter_.flush();
}

void ASTTree::RestoreArgs(std::list<Symbol *> * local_vars)
{
    interpreter_ << "Restore arguments...\n";
    auto iter = local_vars->begin();
    auto * cur = func_args_stack.top();
    while (cur)
    {
        CpySymbolValue(cur, *iter);
        cur = cur->GetNextSymbol();
        std::advance(iter, 1);
    }
    func_args_stack.pop();
}

void ASTTree::CpySymbolValue(Symbol *src, Symbol *des)
{
    des->name_ = src->name_;
    des->level_ = src->level_;
    des->symbol_scope_ = src->symbol_scope_;

    des->i_value_ = src->i_value_;
    des->f_value_ = src->f_value_;
    des->c_value_ = src->c_value_;
    des->s_value_ = src->s_value_;
}

std::list<Symbol *> * ASTTree::GetArgsByScope(const std::string &scope)
{
    auto * list = new std::list<Symbol *>();
    for (const auto& pair : typeSystem_->symbol_table_)
    {
        for (auto local_var : pair.second)
        {
            if (local_var->name_ != scope && local_var->symbol_scope_ == scope)
                list->emplace_back(local_var);
        }
    }
    return list;
}

