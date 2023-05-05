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

    content_ = "";	// token原始内容
    type_ = "";	    // token类型
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

    // 输出当前节点
    tree_out << indent;
    if (level > 0) {
        tree_out << (is_last_sibling ? "└──" : "├──");
    }
    tree_out << node->ast_name_ << "\n";

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

    tree_out.open("../Grammar/tree_out.txt", std::ios::out);
    if (!tree_out.is_open())
    {
        std::cerr << "无法打开文件将语法树输出到文件！" << std::endl;
        exit(-1);
    }

    // 使用辅助函数从根节点开始递归输出
    print_node(root_, 0, "", true);

    tree_out.flush();
    tree_out.close();
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

