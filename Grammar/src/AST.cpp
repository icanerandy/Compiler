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

#include "include/AST.h"

ASTNode::ASTNode()
{
    ast_name_ = "";

    children_.clear();
    next_ = nullptr;
    parent_ = nullptr;

    content_ = "";	// tokenԭʼ����
    type_ = "";	    // token����
    line_no_ = -1;
    column_no_ = -1;
}

ASTNode::~ASTNode()
= default;

void ASTNode::SetParents(ASTNode* parent)
{
    parent_ = parent;
}

size_t ASTNode::GetChildrenSize()
{
    return children_.size();
}

bool ASTNode::IsRoot()
{
    return (parent_ == nullptr);
}

bool ASTNode::IsLeaf()
{
    return children_.empty();
}

std::list<struct ASTNode *> ASTNode::GetChildrenList() const
{
    return children_;
}

struct ASTNode *ASTNode::GetParent() const
{
    return parent_;
}

ASTTree::ASTTree(ASTNode * root)
    :
    root_(root)
{

}

ASTTree::~ASTTree()
= default;

void ASTTree::print_node(const ASTNode *node, int level, const std::string &indent, bool is_last_sibling) {
    if (node == nullptr) {
        return;
    }

    // �����ǰ�ڵ�
    tree_out << indent;
    if (level > 0) {
        tree_out << (is_last_sibling ? "������" : "������");
    }
    tree_out << node->ast_name_ << "\n";

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

    tree_out.open("../Grammar/tree_out.txt", std::ios::out);
    if (!tree_out.is_open())
    {
        std::cerr << "�޷����ļ����﷨��������ļ���" << std::endl;
        exit(-1);
    }

    // ʹ�ø��������Ӹ��ڵ㿪ʼ�ݹ����
    print_node(root_, 0, "", true);

    tree_out.flush();
    tree_out.close();
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

