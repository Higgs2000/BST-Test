// Red Black tree implementation
//
// Implementation is based on T.H. Cormen, C.E. Leiserson, R.L. Rivest, C. Stein,
// Introduction to algorithms, 3rd ed. MIT Press, Cambridge, MA, 2009, Chapter 13
//
// Ville Heikkilä

#ifndef REDBLACKTREE_CPP
#define REDBLACKTREE_CPP

#include "redblacktree.hh"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>
#include <windows.h>

namespace
{
/*
inline std::ostream& red(std::ostream &s)
{
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdout,
            FOREGROUND_RED|FOREGROUND_INTENSITY);
    return s;
}

inline std::ostream& white(std::ostream &s)
{
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdout,
            FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
    return s;
}
*/
} // namespace

template<typename Key, typename Value>
RedBlackTree<Key, Value>::RedBlackTree() :
    nil_{ new RedBlackNode<Key, Value>{ -1, {}, Color::Black, nullptr, nullptr, nullptr } },
    root_{ nil_ },
    nodes_{ 0 }
{
}

template<typename Key, typename Value>
RedBlackTree<Key, Value>::~RedBlackTree()
{
    RedBlackNode<Key, Value>* node{ minimum() };
    while (node != nil_)
    {
        erase(node->key_);
        node = minimum();
    }
    delete nil_;
}

template<typename Key, typename Value>
typename RedBlackTree<Key, Value>::size_type RedBlackTree<Key, Value>::size() const
{
    return nodes_;
}

template<typename Key, typename Value>
int RedBlackTree<Key, Value>::height() const
{
    return height(root_);
}

template<typename Key, typename Value>
int RedBlackTree<Key, Value>::height(RedBlackNode<Key, Value>* node) const
{
    if (node == nil_)
    {
        return -1;
    }

    int leftHeight{ (node->left_ == nil_) ? 0 : height(node->left_) + 1};
    int rightHeight{ (node->right_ == nil_) ? 0 : height(node->right_) + 1};

    return std::max(leftHeight, rightHeight);
}

template<typename Key, typename Value>
bool RedBlackTree<Key, Value>::isNull(RedBlackNode<Key, Value>* node) const
{
    return (node == nullptr or node == nil_);
}

template<typename Key, typename Value>
void RedBlackTree<Key, Value>::clear()
{
    RedBlackNode<Key, Value>* node{ minimum() };
    while (node != nil_)
    {
        erase(node->key_);
        node = minimum();
    }

    root_ = nil_;
    nodes_ = 0;
}

template<typename Key, typename Value>
RedBlackNode<Key, Value>* RedBlackTree<Key, Value>::maximum() const
{
    return maximum(root_);
}

template<typename Key, typename Value>
RedBlackNode<Key, Value>* RedBlackTree<Key, Value>::maximum(RedBlackNode<Key, Value>* node) const
{
    if (node == nil_)
    {
        return nil_;
    }

    RedBlackNode<Key, Value>* x{ node };
    while (x->right_ != nil_)
    {
        x = x->right_;
    }
    return x;
}

template<typename Key, typename Value>
RedBlackNode<Key, Value>* RedBlackTree<Key, Value>::minimum() const
{
    return minimum(root_);
}

template<typename Key, typename Value>
RedBlackNode<Key, Value>* RedBlackTree<Key, Value>::minimum(RedBlackNode<Key, Value>* node) const
{
    if (node == nil_)
    {
        return nil_;
    }

    RedBlackNode<Key, Value>* x{ node };
    while (x->left_ != nil_)
    {
        x = x->left_;
    }
    return x;
}

template<typename Key, typename Value>
RedBlackNode<Key, Value>* RedBlackTree<Key, Value>::successor(RedBlackNode<Key, Value>* node) const
{
    if (node == nil_)
    {
        return nil_;
    }

    if (node->right_ != nil_)
    {
        return minimum(node->right_);
    }

    RedBlackNode<Key, Value>* x{ node };
    RedBlackNode<Key, Value>* y{ x->parent_ };
    while (y != nil_ and x == y->right_)
    {
        x = y;
        y = y->parent_;
    }
    return y;
}

template<typename Key, typename Value>
RedBlackNode<Key, Value>* RedBlackTree<Key, Value>::predecessor(RedBlackNode<Key, Value>* node) const
{
    if (node == nil_)
    {
        return nil_;
    }

    if (node->left_ != nil_)
    {
        return maximum(node->left_);
    }

    RedBlackNode<Key, Value>* x{ node };
    RedBlackNode<Key, Value>* y{ x->parent_ };
    while (y != nil_ and x == y->left_)
    {
        x = y;
        y = y->parent_;
    }
    return y;
}

template<typename Key, typename Value>
bool RedBlackTree<Key, Value>::isInTree(RedBlackNode<Key, Value>* node) const
{
    if (node == nil_ or node == nullptr or root_ == nil_)
    {
        return false;
    }

    RedBlackNode<Key, Value>* x{ node };
    while (x != nil_)
    {
        if (x == root_)
        {
            return true;
        }
        x = x->parent_;
    }

    return false;
}

template<typename Key, typename Value>
RedBlackNode<Key, Value>* RedBlackTree<Key, Value>::find(const Key& key) const
{
    auto x{ root_ };
    while (x != nullptr)
    {
        if (key < x->key_)
        {
            x = x->left_;
        }
        else if (x->key_ < key)
        {
            x = x->right_;
        }
        else
        {
            return x;
        }
    }
    return x;
}

template<typename Key, typename Value>
bool RedBlackTree<Key, Value>::insert(const value_type& value)
{
    RedBlackNode<Key, Value>* x{ root_ };
    RedBlackNode<Key, Value>* parent{ nil_ };

    while (x != nil_)
    {
        parent = x;
        if (value.first < x->key_)
        {
            x = x->left_;
        }
        else if (x->key_ < value.first)
        {
            x = x->right_;
        }
        else
        {
            return false;
        }
    }

    RedBlackNode<Key, Value>* node{
        new RedBlackNode<Key, Value>{ value.first, value.second, Color::Red,
                                      parent, nil_, nil_ } };

    if (parent == nil_)
    {
        root_ = node;
    }
    else if (node->key_ < parent->key_)
    {
        parent->left_ = node;
    }
    else
    {
        parent->right_ = node;
    }
    ++nodes_;

    insertFix(node);
    return true;
}

template<typename Key, typename Value>
typename RedBlackTree<Key, Value>::size_type RedBlackTree<Key, Value>::erase(const Key& key)
{
    auto node{ find(key) };
    if (node == nil_)
    {
        return 0;
    }

    RedBlackNode<Key, Value>* x{ nil_ };
    RedBlackNode<Key, Value>* y{ node };
    Color yOriginalColor{ y->color_ };

    if (node->left_ == nil_)
    {
        x = node->right_;
        transplant(node, node->right_);
    }
    else if (node->right_ == nil_)
    {
        x = node->left_;
        transplant(node, node->left_);
    }
    else
    {
        RedBlackNode<Key, Value>* y{ minimum(node->right_) };
        yOriginalColor = y->color_;
        x = y->right_;

        if (y->parent_ == node)
        {
            x->parent_ = y;
        }
        else
        {
            transplant(y, y->right_);
            y->right_ = node->right_;
            y->right_->parent_ = y;
        }
        transplant(node, y);
        y->left_ = node->left_;
        y->left_->parent_ = y;
        y->color_ = node->color_;
    }

    delete node;
    --nodes_;

    if (yOriginalColor == Color::Black)
    {
        deleteFix(x);
    }

    return 1;
}

template<typename Key, typename Value>
void RedBlackTree<Key, Value>::rotateLeft(RedBlackNode<Key, Value>* x)
{
    RedBlackNode<Key, Value>* y{ x->right_ };

    x->right_ = y->left_;
    if (y->left_ != nil_)
    {
        y->left_->parent_ = x;
    }

    y->parent_ = x->parent_;
    if (x->parent_ == nil_)
    {
        root_ = y;
    }
    else if (x == x->parent_->left_)
    {
        x->parent_->left_ = y;
    }
    else
    {
        x->parent_->right_ = y;
    }

    y->left_ = x;
    x->parent_ = y;
}

template<typename Key, typename Value>
void RedBlackTree<Key, Value>::rotateRight(RedBlackNode<Key, Value>* x)
{
    RedBlackNode<Key, Value>* y{ x->left_ };

    x->left_ = y->right_;
    if (y->right_ != nil_)
    {
        y->right_->parent_ = x;
    }

    y->parent_ = x->parent_;
    if (x->parent_ == nil_)
    {
        root_ = y;
    }
    else if (x == x->parent_->left_)
    {
        x->parent_->left_ = y;
    }
    else
    {
        x->parent_->right_ = y;
    }

    y->right_ = x;
    x->parent_ = y;
}

template<typename Key, typename Value>
void RedBlackTree<Key, Value>::insertFix(RedBlackNode<Key, Value>* x)
{
    while (x->parent_->color_ == Color::Red)
    {
        if (x->parent_ == x->parent_->parent_->left_)
        {
            RedBlackNode<Key, Value>* y{ x->parent_->parent_->right_ };
            if (y->color_ == Color::Red)
            {
                x->parent_->color_ = Color::Black;
                y->color_ = Color::Black;
                x->parent_->parent_->color_ = Color::Red;
                x = x->parent_->parent_;
            }
            else
            {
                if (x == x->parent_->right_)
                {
                    x = x->parent_;
                    rotateLeft(x);
                }

                x->parent_->color_ = Color::Black;
                x->parent_->parent_->color_ = Color::Red;
                rotateRight(x->parent_->parent_);
            }
        }
        else
        {
            RedBlackNode<Key, Value>* y{ x->parent_->parent_->left_ };
            if (y->color_ == Color::Red)
            {
                x->parent_->color_ = Color::Black;
                y->color_ = Color::Black;
                x->parent_->parent_->color_ = Color::Red;
                x = x->parent_->parent_;
            }
            else
            {
                if (x == x->parent_->left_)
                {
                    x = x->parent_;
                    rotateRight(x);
                }

                x->parent_->color_ = Color::Black;
                x->parent_->parent_->color_ = Color::Red;
                rotateLeft(x->parent_->parent_);
            }
        }
    }

    root_->color_ = Color::Black;
}

template<typename Key, typename Value>
void RedBlackTree<Key, Value>::deleteFix(RedBlackNode<Key, Value>* x)
{
    while (x != root_ and x->color_ == Color::Black)
    {
        if (x == x->parent_->left_)
        {
            RedBlackNode<Key, Value>* w = x->parent_->right_;
            if (w->color_ == Color::Red)
            {
                w->color_ = Color::Black;
                x->parent_->color_ = Color::Red;
                rotateLeft(x->parent_);
                w = x->parent_->right_;
            }

            if (w->left_->color_ == Color::Black and w->right_->color_ == Color::Black)
            {
                w->color_ = Color::Red;
                x = x->parent_;
            }
            else
            {
                if (w->right_->color_ == Color::Black)
                {
                    w->left_->color_ = Color::Black;
                    w->color_ = Color::Red;
                    rotateRight(w);
                    w = x->parent_->right_;
                }
                w->color_ = x->parent_->color_;
                x->parent_->color_ = Color::Black;
                w->right_->color_ = Color::Black;
                rotateLeft(x->parent_);
                x = root_;
            }
        }
        else
        {
            RedBlackNode<Key, Value>* w = x->parent_->left_;
            if (w->color_ == Color::Red)
            {
                w->color_ = Color::Black;
                x->parent_->color_ = Color::Red;
                rotateRight(x->parent_);
                w = x->parent_->left_;
            }

            if (w->right_->color_ == Color::Black and w->left_->color_ == Color::Black)
            {
                w->color_ = Color::Red;
                x = x->parent_;
            }
            else
            {
                if (w->left_->color_ == Color::Black)
                {
                    w->right_->color_ = Color::Black;
                    w->color_ = Color::Red;
                    rotateLeft(w);
                    w = x->parent_->left_;
                }
                w->color_ = x->parent_->color_;
                x->parent_->color_ = Color::Black;
                w->left_->color_ = Color::Black;
                rotateRight(x->parent_);
                x = root_;
            }
        }
    }

    x->color_ = Color::Black;
}

template<typename Key, typename Value>
void RedBlackTree<Key, Value>::transplant(RedBlackNode<Key, Value>* u, RedBlackNode<Key, Value>* v)
{
    if (u == nil_)
    {
        return;
    }

    if (u->parent_ == nil_)
    {
        root_ = v;
    }
    else if (u == u->parent_->left_)
    {
        u->parent_->left_ = v;
    }
    else
    {
        u->parent_->right_ = v;
    }

    v->parent_ = u->parent_;
}

template<typename Key, typename Value>
void RedBlackTree<Key, Value>::print() const
{
    const int NODE_WIDTH{ 3 };
    const int NODE_SPACE{ 1 };

    std::vector<RedBlackNode<Key, Value>*> nodeList;
    int h{ height() };
    for (int level{ 0 }; level <= h; ++level)
    {
        std::vector<RedBlackNode<Key, Value>*> nodeRow;

        if (level == 0)
        {
            nodeRow.push_back(root_);
        }
        else
        {
            for (unsigned int i{ 0 }; i < nodeList.size(); ++i)
            {
                RedBlackNode<Key, Value>* x{ nodeList[i] };
                if (isNull(x))
                {
                    nodeRow.push_back(nullptr);
                    nodeRow.push_back(nullptr);
                }
                else
                {
                    nodeRow.push_back(x->left_);
                    nodeRow.push_back(x->right_);
                }
            }
        }

        int factor{ (NODE_WIDTH+NODE_SPACE)/2 };
        int indent{ factor * ((1 << (h - level)) - 1) };

        if (level > 0)
        {
            bool left{ true };
            for (unsigned int i{ 0 }; i < nodeRow.size(); ++i)
            {

                if (i > 0)
                {
                    std::cout << std::setw(NODE_SPACE) << " ";
                }
                if (indent > 0)
                {
                    std::cout << std::setw(indent) << " ";
                }
                std::cout << std::setw(NODE_WIDTH);
                if (not isNull(nodeRow[i]))
                {
                    if (left)
                    {
                        std::cout << std::right << "/";
                    }
                    else
                    {
                        std::cout << std::left << "\\";
                    }
                }
                else
                {
                    std::cout << " ";
                }
                if (i < nodeRow.size() - 1 and indent > 0)
                {
                    std::cout << std::setw(indent) << " ";
                }

                left = not left;
            }
            std::cout << std::endl;
        }

        for (unsigned int i{ 0 }; i < nodeRow.size(); ++i)
        {
            if (i > 0)
            {
                std::cout << std::setw(NODE_SPACE) << " ";
            }
            if (indent > 0)
            {
                std::cout << std::setw(indent) << " ";
            }
            std::cout << std::setw(NODE_WIDTH) << std::right;
            if (not isNull(nodeRow[i]))
            {
                if (nodeRow[i]->color_ == Color::Red)
                {
                    std::cout << red << nodeRow[i]->key_ << white;
                }
                else
                {
                    std::cout << nodeRow[i]->key_;
                }
            }
            else
            {
                std::cout << " ";
            }
            if (i + 1 < nodeRow.size() and indent > 0)
            {
                std::cout << std::setw(indent) << " ";
            }
        }
        std::cout << std::endl;

        nodeList = nodeRow;
    }
}

#endif // REDBLACKTREE_CPP
