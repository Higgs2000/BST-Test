// AVL tree implementation
//
// Implementation is based on https://bitlush.com/blog/efficient-avl-tree-in-c-sharp (1.4.2017)
//
// Ville Heikkilä

#ifndef AVLTREE_CPP
#define AVLTREE_CPP

#include "avltree.hh"
#include <iomanip>
#include <iostream>
#include <vector>
#include <windows.h>

namespace
{

inline std::ostream& blue(std::ostream &s)
{
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdout,
            FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
    return s;
}

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

} // namespace

template<typename Key, typename Value>
AVLTree<Key, Value>::AVLTree() :
    root_{ nullptr },
    nodes_{ 0 }
{
}

template<typename Key, typename Value>
AVLTree<Key, Value>::~AVLTree()
{
    AVLNode<Key, Value>* node{ minimum() };
    while (node != nullptr)
    {
        erase(node->key_);
        node = minimum();
    }
}

template<typename Key, typename Value>
typename AVLTree<Key, Value>::size_type AVLTree<Key, Value>::size() const
{
    return nodes_;
}

template<typename Key, typename Value>
int AVLTree<Key, Value>::height() const
{
    return height(root_);
}

template<typename Key, typename Value>
int AVLTree<Key, Value>::height(AVLNode<Key, Value>* node) const
{
    if (node == nullptr)
    {
        return -1;
    }

    int leftHeight{ node->left_ == nullptr ? 0 : height(node->left_) + 1 };
    int rightHeight{ node->right_ == nullptr ? 0 : height(node->right_) + 1 };

    return std::max(leftHeight, rightHeight);
}

template<typename Key, typename Value>
void AVLTree<Key, Value>::clear()
{
    AVLNode<Key, Value>* node{ minimum() };
    while (node != nullptr)
    {
        erase(node->key_);
        node = minimum();
    }

    root_ = nullptr;
    nodes_ = 0;
}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::maximum() const
{
    return maximum(root_);
}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::maximum(AVLNode<Key, Value>* node) const
{
    if (node == nullptr)
    {
        return nullptr;
    }

    AVLNode<Key, Value>* x{ node };
    while (x->right_ != nullptr)
    {
        x = x->right_;
    }
    return x;
}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::minimum() const
{
    return minimum(root_);
}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::minimum(AVLNode<Key, Value>* node) const
{
    if (node == nullptr)
    {
        return nullptr;
    }

    AVLNode<Key, Value>* x{ node };
    while (x->left_ != nullptr)
    {
        x = x->left_;
    }
    return x;
}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::successor(AVLNode<Key, Value>* node) const
{
    if (node == nullptr)
    {
        return nullptr;
    }

    if (node->right_ != nullptr)
    {
        return minimum(node->right_);
    }

    AVLNode<Key, Value>* x{ node };
    AVLNode<Key, Value>* y{ x->parent_ };
    while (y != nullptr and x == y->right_)
    {
        x = y;
        y = y->parent_;
    }
    return y;
}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::predecessor(AVLNode<Key, Value>* node) const
{
    if (node == nullptr)
    {
        return nullptr;
    }

    if (node->left_ != nullptr)
    {
        return maximum(node->left_);
    }

    AVLNode<Key, Value>* x{ node };
    AVLNode<Key, Value>* y{ x->parent_ };
    while (y != nullptr and x == y->left_)
    {
        x = y;
        y = y->parent_;
    }
    return y;
}

template<typename Key, typename Value>
bool AVLTree<Key, Value>::isInTree(AVLNode<Key, Value>* node) const
{
    if (node == nullptr or root_ == nullptr)
    {
        return false;
    }

    AVLNode<Key, Value>* x{ node };
    while (x != nullptr)
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
AVLNode<Key, Value>* AVLTree<Key, Value>::find(const Key& key) const
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
bool AVLTree<Key, Value>::insert(const value_type& value)
{
    AVLNode<Key, Value>* x{ root_ };
    AVLNode<Key, Value>* parent{ nullptr };

    while (x != nullptr)
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

    AVLNode<Key, Value>* node{
        new AVLNode<Key, Value>{ value.first, value.second, 0,
                                  parent, nullptr, nullptr } };

    ++nodes_;

    if (parent == nullptr)
    {
        root_ = node;
//        root_->balance_ = 0;
    }
    else if (node->key_ < parent->key_)
    {
        parent->left_ = node;
        insertBalance(parent, 1);
    }
    else
    {
        parent->right_ = node;
        insertBalance(parent, -1);
    }
    return true;
}

template<typename Key, typename Value>
typename AVLTree<Key, Value>::size_type AVLTree<Key, Value>::erase(const Key& key)
{
    auto node{ find(key) };
    if (node == nullptr)
    {
        return 0;
    }

    AVLNode<Key, Value>* left{ node->left_ };
    AVLNode<Key, Value>* right{ node->right_ };

    if (left == nullptr)
    {
        if (right == nullptr)
        {
            if (node == root_)
            {
                delete root_;
                root_ = nullptr;
            }
            else
            {
                if (node->parent_->left_ == node)
                {
                    node->parent_->left_ = nullptr;
                    deleteBalance(node->parent_, -1);
                    delete node;
                }
                else
                {
                    node->parent_->right_ = nullptr;
                    deleteBalance(node->parent_, 1);
                    delete node;
                }
            }
        }
        else
        {
            replace(node, right);
            deleteBalance(node, 0);
        }
    }
    else if (right == nullptr)
    {
        replace(node, left);
        deleteBalance(node, 0);
    }
    else
    {
        AVLNode<Key, Value>* successor{ right };
        if (successor->left_ == nullptr)
        {
            successor->parent_ = node->parent_;
            successor->left_ = left;
            successor->balance_ = node->balance_;

            if (left != nullptr)
            {
                left->parent_ = successor;
            }

            if (node == root_)
            {
                root_ = successor;
            }
            else
            {
                if (node->parent_->left_ == node)
                {
                    node->parent_->left_ = successor;
                }
                else
                {
                    node->parent_->right_ = successor;
                }
            }

            deleteBalance(successor, 1);
            delete node;
        }
        else
        {
            while (successor->left_ != nullptr)
            {
                successor = successor->left_;
            }

            AVLNode<Key, Value>* successorParent{ successor->parent_ };
            AVLNode<Key, Value>* successorRight{ successor->right_ };

            if (successorParent->left_ == successor)
            {
                successorParent->left_ = successorRight;
            }
            else
            {
                successorParent->right_ = successorRight;
            }

            if (successorRight != nullptr)
            {
                successorRight->parent_ = successorParent;
            }

            successor->parent_ = node->parent_;
            successor->left_ = left;
            successor->balance_ = node->balance_;
            successor->right_ = right;
            right->parent_ = successor;

            if (left != nullptr)
            {
                left->parent_ = successor;
            }

            if (node == root_)
            {
                root_ = successor;
            }
            else
            {
                if (node->parent_->left_ == node)
                {
                    node->parent_->left_ = successor;
                }
                else
                {
                    node->parent_->right_ = successor;
                }
            }

            deleteBalance(successorParent, -1);
            delete node;
        }
    }

    --nodes_;
    return 1;
}

template<typename Key, typename Value>
void AVLTree<Key, Value>::insertBalance(AVLNode<Key, Value>* node, int balance)
{
    while (node != nullptr)
    {
        node->balance_ += balance;
        balance = node->balance_;

        if (balance == 0)
        {
            return;
        }
        else if (balance == 2)
        {
            if (node->left_->balance_ == 1)
            {
                rotateRight(node);
            }
            else
            {
                rotateLeftRight(node);
            }

            return;
        }
        else if (balance == -2)
        {
            if (node->right_->balance_ == -1)
            {
                rotateLeft(node);
            }
            else
            {
                rotateRightLeft(node);
            }

            return;
        }

        if (node->parent_ != nullptr)
        {
            balance = (node->parent_->left_ == node) ? 1 : -1;
        }

        node = node->parent_;
    }
}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* right{ node->right_ };
    AVLNode<Key, Value>* rightleft{ right->left_ };
    AVLNode<Key, Value>* parent{ node->parent_ };

    right->parent_ = parent;
    right->left_ = node;
    node->right_ = rightleft;
    node->parent_ = right;

    if (rightleft != nullptr)
    {
        rightleft->parent_ = node;
    }

    if (node == root_)
    {
        root_ = right;
    }
    else if (parent->right_ == node)
    {
        parent->right_ = right;
    }
    else
    {
        parent->left_ = right;
    }

    ++right->balance_;
    node->balance_ = -right->balance_;

    return right;
}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* left{ node->left_ };
    AVLNode<Key, Value>* leftright{ left->right_ };
    AVLNode<Key, Value>* parent{ node->parent_ };

    left->parent_ = parent;
    left->right_ = node;
    node->left_ = leftright;
    node->parent_ = left;

    if (leftright != nullptr)
    {
        leftright->parent_ = node;
    }

    if (node == root_)
    {
        root_ = left;
    }
    else if (parent->left_ == node)
    {
        parent->left_ = left;
    }
    else
    {
        parent->right_ = left;
    }

    --left->balance_;
    node->balance_ = -left->balance_;

    return left;
}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeftRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* left{ node->left_ };
    AVLNode<Key, Value>* leftright{ left->right_ };
    AVLNode<Key, Value>* parent{ node->parent_ };
    AVLNode<Key, Value>* leftrightleft = leftright->left_;
    AVLNode<Key, Value>* leftrightright = leftright->right_;

    leftright->parent_ = parent;
    node->left_ = leftrightright;
    left->right_ = leftrightleft;
    leftright->left_ = left;
    leftright->right_ = node;
    left->parent_ = leftright;
    node->parent_ = leftright;

    if (leftrightright != nullptr)
    {
        leftrightright->parent_ = node;
    }

    if (leftrightleft != nullptr)
    {
        leftrightleft->parent_ = left;
    }

    if (node == root_)
    {
        root_ = leftright;
    }
    else if (parent->left_ == node)
    {
        parent->left_ = leftright;
    }
    else
    {
        parent->right_ = leftright;
    }

    if (leftright->balance_ == -1)
    {
        node->balance_ = 0;
        left->balance_ = 1;
    }
    else if (leftright->balance_ == 0)
    {
        node->balance_ = 0;
        left->balance_ = 0;
    }
    else
    {
        node->balance_ = -1;
        left->balance_ = 0;
    }

    leftright->balance_ = 0;

    return leftright;
}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRightLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* right{ node->right_ };
    AVLNode<Key, Value>* rightleft{ right->left_ };
    AVLNode<Key, Value>* parent{ node->parent_ };
    AVLNode<Key, Value>* rightleftleft = rightleft->left_;
    AVLNode<Key, Value>* rightleftright = rightleft->right_;

    rightleft->parent_ = parent;
    node->right_ = rightleftleft;
    right->left_ = rightleftright;
    rightleft->right_ = right;
    rightleft->left_ = node;
    right->parent_ = rightleft;
    node->parent_ = rightleft;

    if (rightleftleft != nullptr)
    {
        rightleftleft->parent_ = node;
    }

    if (rightleftright != nullptr)
    {
        rightleftright->parent_ = right;
    }

    if (node == root_)
    {
        root_ = rightleft;
    }
    else if (parent->right_ == node)
    {
        parent->right_ = rightleft;
    }
    else
    {
        parent->left_ = rightleft;
    }

    if (rightleft->balance_ == 1)
    {
        node->balance_ = 0;
        right->balance_ = -1;
    }
    else if (rightleft->balance_ == 0)
    {
        node->balance_ = 0;
        right->balance_ = 0;
    }
    else
    {
        node->balance_ = 1;
        right->balance_ = 0;
    }

    rightleft->balance_ = 0;

    return rightleft;
}

template<typename Key, typename Value>
void AVLTree<Key, Value>::deleteBalance(AVLNode<Key, Value>* node, int balance)
{
    while (node != nullptr)
    {
        node->balance_ += balance;
        balance = node->balance_;

        if (balance == 2)
        {
            if (node->left_->balance_ >= 0)
            {
                node = rotateRight(node);

                if (node->balance_ == -1)
                {
                    return;
                }
            }
            else
            {
                node = rotateLeftRight(node);
            }
        }
        else if (balance == -2)
        {
            if (node->right_->balance_ <= 0)
            {
                node = rotateLeft(node);

                if (node->balance_ == 1)
                {
                    return;
                }
            }
            else
            {
                node = rotateRightLeft(node);
            }
        }
        else if (balance != 0)
        {
            return;
        }

        if (node->parent_ != nullptr)
        {
            balance = (node->parent_->left_ == node) ? -1 : 1;
        }

        node = node->parent_;
    }
}

template<typename Key, typename Value>
void AVLTree<Key, Value>::replace(AVLNode<Key, Value>* target, AVLNode<Key, Value>* source)
{
    target->balance_ = source->balance_;
    target->key_ = source->key_;
    target->left_ = source->left_;
    target->right_ = source->right_;

    if (source->left_ != nullptr)
    {
        source->left_->parent_ = target;
    }

    if (source->right_ != nullptr)
    {
        source->right_->parent_ = target;
    }

    delete source;
}

template<typename Key, typename Value>
void AVLTree<Key, Value>::print() const
{
    const int NODE_WIDTH{ 3 };
    const int NODE_SPACE{ 1 };

    std::vector<AVLNode<Key, Value>*> nodeList;
    int h{ height() };
    for (int level{ 0 }; level <= h; ++level)
    {
        std::vector<AVLNode<Key, Value>*> nodeRow;

        if (level == 0)
        {
            nodeRow.push_back(root_);
        }
        else
        {
            for (unsigned int i{ 0 }; i < nodeList.size(); ++i)
            {
                AVLNode<Key, Value>* x{ nodeList[i] };
                if (x == nullptr)
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
                if (nodeRow[i] != nullptr)
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
            std::cout << std::setw(NODE_WIDTH);
            if (nodeRow[i] != nullptr)
            {
                std::cout << std::setw(NODE_WIDTH) <<std::right;
                if (nodeRow[i]->balance_ == -1)
                {
                    std::cout << red << nodeRow[i]->key_ << white;
                }
                else if (nodeRow[i]->balance_ == 1)
                {
                    std::cout << blue << nodeRow[i]->key_ << white;
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

#endif // AVTREE_CPP
