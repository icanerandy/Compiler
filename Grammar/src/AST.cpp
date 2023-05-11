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
    content_ = "";	// tokenԭʼ����
    type_ = "";	    // token����
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

    // �����ǰ�ڵ�
    tree_out_ << indent;
    if (level > 0) {
        tree_out_ << (is_last_sibling ? "������" : "������");
    }
    if (node->type_ == "<������>" || node->type_ == "<����>" || node->type_ == "<����>"
        || node->type_ == "<����>" || node->type_ == "<�ַ�����>" || node->type_ == "<�ַ�������>")
        tree_out_ << node->content_ << "\n";
    else
        tree_out_ << node->type_ << "\n";

    // �����ӽڵ������
    std::string child_indent = indent + (level > 0 ? (is_last_sibling ? "    " : "��   ") : "");

    // �ݹ�����ӽڵ�
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
        std::cerr << "�޷����ļ����﷨��������ļ���" << std::endl;
        exit(-1);
    }

    // ʹ�ø��������Ӹ��ڵ㿪ʼ�ݹ����
    print_node(root_, 0, "", true);

    tree_out_.flush();
    tree_out_.close();
}

void ASTTree::RemoveEmptyNodes(ASTNode* node) {
    if (node == nullptr) {
        return;
    }

    // �ݹ鴦���ӽڵ�
    for (auto it = node->children_.begin(); it != node->children_.end(); ) {
        if (*it == nullptr) {
            delete *it;
            it = node->children_.erase(it);
        } else {
            RemoveEmptyNodes(*it);
            ++it;
        }
    }

    // ���������ӽڵ���ӽڵ�
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
    if (type == "if���")
    {
        interpreter_ << "ִ��if��䣺\n";
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
        interpreter_ << "if������\n";
        interpreter_.flush();
    }
    else if (type == "for���")
    {
        interpreter_ << "ִ��for��䣺\n";
        ExecuteNode(root->GetChildrenList().front());   // ִ�и���ֵ���
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
            interpreter_ << "��" << time++ << "�Σ�\n";
            ExecuteNode(*iter3);
            ExecuteNode(*iter2);
            ExecuteNode(*iter1);
        }
        interpreter_ << "for������\n";
        interpreter_.flush();
    }
    else if (type == "while���")
    {
        interpreter_ << "ִ��while��䣺\n";
        auto iter1 = root->GetChildrenList().begin();
        auto iter2 = root->GetChildrenList().begin();
        std::advance(iter2, 1);

        int time = 1;
        ExecuteNode(*iter1);
        while ((*iter1)->i_value_)
        {
            interpreter_ << "��" << time++ << "�Σ�\n";
            ExecuteNode(*iter2);
            ExecuteNode(*iter1);
        }
        interpreter_ << "while������\n";
        interpreter_.flush();
    }
    else if (type == "do-while���")
    {
        interpreter_ << "ִ��do-while��䣺\n";
        auto iter1 = root->GetChildrenList().begin();
        auto iter2 = root->GetChildrenList().begin();
        std::advance(iter2, 1);

        int time = 1;
        do
        {
            interpreter_ << "��" << time++ << "�Σ�\n";
            ExecuteNode(*iter1);
            ExecuteNode(*iter2);
        } while ((*iter2)->i_value_);
        interpreter_ << "do-while������\n";
        interpreter_.flush();
    }
    else if (type == "��������")
    {
        // ���ú����ļ�ªʵ��
        if (root->GetChildrenList().front()->content_ == "printf")
        {
            // TODO: �����������ú�������
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
        // ��ͨ��������
        else
        {
            interpreter_ << "ִ�к������ã�" << root->GetChildrenList().front()->content_ << "\n";
            Symbol * func_symbol = root->GetChildrenList().front()->GetSymbol();
            auto * func_entry = (ASTNode *)func_symbol->body_;
            Symbol * func_args = func_symbol->args_;

            func_call_stack.emplace(root, true);
            if (func_args == nullptr)   // �޲�����������
            {
                ExecuteNode(func_entry);
            }
            else    // �в�����������
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
                SaveArgs(local_vars);   // �ݹ���ú���������������

                iter = root->GetChildrenList().begin();
                tmp = func_args;
                while (tmp)
                {
                    std::advance(iter, 1);
                    PassValueToFuncArg(tmp, *iter);
                    tmp = tmp->GetNextSymbol();
                }

                ExecuteNode(func_entry);
                RestoreArgs(local_vars); // �ָ�����������ջ
                runtime_scope_.pop();
            }

            func_call_stack.pop();
            interpreter_ << "�������ý���\n";
            interpreter_.flush();
        }
    }
    else if (type == "return���")
    {
        if (!root->GetChildrenList().empty())
        {
            ExecuteNode(root->GetChildrenList().front());
            CpyChildToParent(func_call_stack.top().first, root->GetChildrenList().front());
            func_ret_val.emplace_back(func_call_stack.top().first->i_value_);
        }

        interpreter_ << "ִ��return���\t����ֵ��" << func_call_stack.top().first->i_value_ << "\n";
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

    if (type == "���ʽ")
    {
        auto child = root->GetChildrenList().front();
        CpyChildToParent(root, child);

        if (child->type_ == "��������" /* ͬʱ��Ҫ���������з���ֵ */ && child->GetChildrenList().front()->content_ != "printf" && GetType(child) != "void" )
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

        if (rhs->type_ == "��������")
        {
            rhs->i_value_ = func_ret_val.front();
            func_ret_val.pop_front();
        }

        if (var_type == "String")
        {
            lhs->s_value_ = rhs->s_value_;
            symbol->SetValue(rhs->s_value_);
            interpreter_ << "ִ�г�ʼ����䣺Variable " << lhs->content_ << " is initialized to value of " << symbol->s_value_ << "\n";
            interpreter_.flush();
        }

        if (var_type == "char")
        {
            lhs->c_value_ = rhs->c_value_;
            symbol->SetValue(rhs->c_value_);
            interpreter_ << "ִ�г�ʼ����䣺Variable " << lhs->content_ << " is initialized to value of " << symbol->c_value_ << "\n";
            interpreter_.flush();
        }

        if (var_type == "int")
        {
            lhs->i_value_ = rhs->i_value_;
            symbol->SetValue(rhs->i_value_);
            interpreter_ << "ִ�г�ʼ����䣺Variable " << lhs->content_ << " is initialized to value of " << symbol->i_value_ << "\n";
            interpreter_.flush();
        }

        if (var_type == "float")
        {
            lhs->f_value_ = rhs->f_value_;
            symbol->SetValue(rhs->f_value_);
            interpreter_ << "ִ�г�ʼ����䣺Variable " << lhs->content_ << " is initialized to value of " << symbol->f_value_ << "\n";
            interpreter_.flush();
        }
    }

    if (type == "AOP")
    {
        ASTNode * lhs = root->GetChildrenList().front();
        ASTNode * rhs = root->GetChildrenList().back();

        Symbol * symbol = lhs->GetSymbol();
        std::string var_type = GetType(lhs);

        if (rhs->type_ == "��������")
        {
            rhs->i_value_ = func_ret_val.front();
            func_ret_val.pop_front();
        }

        if (var_type == "String")
        {
            lhs->s_value_ = rhs->s_value_;
            symbol->SetValue(rhs->s_value_);
            CpyChildToParent(root, lhs);
            interpreter_ << "ִ�и�ֵ��䣺Variable " << lhs->content_ << " is assigned to value of " << symbol->s_value_ << "\n";
            interpreter_.flush();
        }

        if (var_type == "char")
        {
            lhs->c_value_ = rhs->c_value_;
            symbol->SetValue(rhs->c_value_);
            CpyChildToParent(root, lhs);
            interpreter_ << "ִ�и�ֵ��䣺Variable " << lhs->content_ << " is assigned to value of " << symbol->c_value_ << "\n";
            interpreter_.flush();
        }

        if (var_type == "int")
        {
            int res = AOP(root->content_, lhs->i_value_, rhs->i_value_);
            symbol->SetValue(res);
            CpyChildToParent(root, lhs);
            interpreter_ << "ִ�и�ֵ��䣺Variable " << lhs->content_ << " is assigned to value of " << res << "\n";
            interpreter_.flush();
        }

        if (var_type == "float")
        {
            float res = AOP(root->content_, lhs->f_value_, rhs->f_value_);
            symbol->SetValue(res);
            CpyChildToParent(root, lhs);
            interpreter_ << "ִ�и�ֵ��䣺" << lhs->content_ << " is assigned to value of " << res << "\n";
            interpreter_.flush();
        }
    }

    if (type == "OOP")
    {
        ASTNode * lhs = root->GetChildrenList().front();
        ASTNode * rhs = root->GetChildrenList().back();

        if (lhs->type_ == "��������")
        {
            lhs->i_value_ = func_ret_val.front();
            func_ret_val.pop_front();
        }

        if (rhs->type_ == "��������")
        {
            rhs->i_value_ = func_ret_val.front();
            func_ret_val.pop_front();
        }

        if (lhs->is_int_)
        {
            root->i_value_ = OOP(root->content_, lhs->i_value_, rhs->i_value_);
            root->is_int_ = true;
            interpreter_ << "ִ���������㣺" << lhs->i_value_ << root->content_ << rhs->i_value_;
            interpreter_ << "\tresult is " << root->i_value_ << "\n";
            interpreter_.flush();
        }
        else    // float
        {
            root->f_value_ = OOP(root->content_, lhs->f_value_, rhs->f_value_);
            root->is_int_ = false;
            interpreter_ << "ִ���������㣺" << lhs->f_value_ << root->content_ << rhs->f_value_ << "\n";
            interpreter_ << "\tresult is " << root->f_value_ << "\n";
            interpreter_.flush();
        }
    }

    if (type == "COP")
    {
        ASTNode * lhs = root->GetChildrenList().front();
        ASTNode * rhs = root->GetChildrenList().back();

        if (lhs->type_ == "��������")
        {
            lhs->i_value_ = func_ret_val.front();
            func_ret_val.pop_front();
        }

        if (rhs->type_ == "��������")
        {
            rhs->i_value_ = func_ret_val.front();
            func_ret_val.pop_front();
        }

        if (lhs->is_int_)
        {
            root->i_value_ = COP(root->content_, lhs->i_value_, rhs->i_value_);
            root->is_int_ = true;
            interpreter_ << "ִ�й�ϵ�Ƚϲ�����" << lhs->i_value_ << " " << root->content_ << " " << rhs->i_value_ <<
                            "\tresult is: " << root->i_value_ << "\n";
            interpreter_.flush();
        }
        else    // float
        {
            root->i_value_ = COP(root->content_, lhs->f_value_, rhs->f_value_);
            root->is_int_ = false;
            interpreter_ << "ִ�й�ϵ�Ƚϲ�����" << lhs->f_value_ << " " << root->content_ << " " << rhs->f_value_ <<
                            "\tresult is: " << root->i_value_ << "\n";
            interpreter_.flush();
        }
    }

    if (type == "BOP")
    {
        ASTNode * lhs = root->GetChildrenList().front();
        ASTNode * rhs = root->GetChildrenList().back();

        if (lhs->type_ == "��������")
        {
            lhs->i_value_ = func_ret_val.front();
            func_ret_val.pop_front();
        }

        if (rhs->type_ == "��������")
        {
            rhs->i_value_ = func_ret_val.front();
            func_ret_val.pop_front();
        }

        if (lhs->is_int_)
        {
            root->i_value_ = BOP(root->content_, lhs->i_value_, rhs->i_value_);
            root->is_int_ = true;
            interpreter_ << "ִ�в�����ϵ������" << lhs->i_value_ << " " << root->content_ << " " << rhs->i_value_ <<
                           "\tresult is: " << root->i_value_ << "\n";
            interpreter_.flush();
        }
        else    // float
        {
            root->i_value_ = BOP(root->content_, lhs->f_value_, rhs->f_value_);
            root->is_int_ = false;
            interpreter_ << "ִ�в�����ϵ������" << lhs->f_value_ << " " << root->content_ << " " << rhs->f_value_ <<
                            "\tresult is: " << root->i_value_ << "\n";
            interpreter_.flush();
        }
    }

    // �����ű��е�ֵ�����������У��ӷ��ű��и��½ڵ��е�ֵ
    CpyValueToTreeNode(root);

    return root;
}

void ASTTree::ExecuteTree(Symbol * main_entry)
{
    interpreter_.open("../Grammar/interpreter_out.txt", std::ios::out);
    if (!interpreter_.is_open())
    {
        std::cerr << "�޷����ļ����﷨��������ļ���" << std::endl;
        exit(-1);
    }

    func_call_stack.emplace(static_cast<ASTNode *>(main_entry->body_), true);
    interpreter_ << "ִ��MAIN�������ã�\n";
    runtime_scope_.emplace("main");
    ExecuteNode(static_cast<ASTNode *>(main_entry->body_));
    interpreter_ << "MAIN��������\n";

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
    if (root->type_ == "<����>")
    {
        Symbol * symbol = root->GetSymbol();

        // TODO: ʵ�ִ�����ʾ
        if (symbol == nullptr)
            std::cerr << "δ�����ı���" << std::endl;

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
    if (root->type_ == "<�ַ�����>")
        return "<�ַ�����>";

    if (root->type_ == "<�ַ�������>")
        return "<�ַ�������>";

    if (root->type_ == "<����>")
        return root->GetSymbol()->GetType();

    if (root->type_ == "��������")
        return root->GetChildrenList().front()->GetSymbol()->GetType();

    if (root->type_ == "<����>")
        return "<����>";

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

// OOP ���������: +	-	*	/   %     &   |
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

// AOP ��ֵ������: =   +=  -=  *=  /=  %=
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

// COP �Ƚϲ�����: <	<=  >	>=	==	!=
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
        interpreter_ << "���ݲ���: " << node->i_value_ << "\t";
    }
    if (type == "char")
    {
        sym->c_value_ = node->c_value_;
        interpreter_ << "���ݲ���: " << node->c_value_ << "\t";
    }
    if (type == "String")
    {
        sym->s_value_.assign(node->s_value_);
        interpreter_ << "���ݲ���: " << node->s_value_ << "\t";
    }
    if (type == "float")
    {
        sym->f_value_ = node->f_value_;
        interpreter_ << "���ݲ���: " << node->f_value_ << "\t";
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

