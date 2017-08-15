// Standard binary search tree implementation
//
// Implementation is based on T.H. Cormen, C.E. Leiserson, R.L. Rivest, C. Stein,
// Introduction to algorithms, 3rd ed. MIT Press, Cambridge, MA, 2009, Chapter 12
//
// Ville Heikkilä

#ifndef BINARYSEARCHTREE_CPP
#define BINARYSEARCHTREE_CPP

#include "binarysearchtree.hh"
#include <iomanip>
#include <iostream>
#include <vector>

template<typename Key, typename Value>
BinarySearchTree<Key, Value>::BinarySearchTree() :
    root_{ nullptr },
    nodes_{ 0 }
{
}

template<typename Key, typename Value>
BinarySearchTree<Key, Value>::~BinarySearchTree()
{
    auto node{ minimum() };
    while (node != nullptr)
    {
        erase(node->key_);
        node = minimum();
    }
}

template<typename Key, typename Value>
typename BinarySearchTree<Key, Value>::size_type BinarySearchTree<Key, Value>::size() const
{
    return nodes_;
}

template<typename Key, typename Value>
int BinarySearchTree<Key, Value>::height() const
{
    return height(root_);
}

template<typename Key, typename Value>
int BinarySearchTree<Key, Value>::height(TreeNode<Key, Value>* node) const
{
    if (node == nullptr)
    {
        return -1;
    }

    int h{ 0 };
    int hMin{ 0 };
    int hMax{ 0 };

    auto x{ minimum() };
    while (x != nullptr)
    {
        if (x->right_ != nullptr)
        {
            x = x->right_;
            --h;
            while (x->left_ != nullptr)
            {
                x = x->left_;
                --h;
            }
        }
        else
        {
            auto y{ x->parent_ };
            while (y != nullptr and x == y->right_)
            {
                x = y;
                y = y->parent_;
                ++h;
            }
            x = y;
            ++h;
        }

        if (x != nullptr and h > hMax)
        {
            hMax = h;
        }
        if (x != nullptr and h < hMin)
        {
            hMin = h;
        }
    }

    return hMax - hMin;
}

template<typename Key, typename Value>
void BinarySearchTree<Key, Value>::clear()
{
    auto node{ minimum() };
    while (node != nullptr)
    {
        erase(node->key_);
        node = minimum();
    }

    root_ = nullptr;
    nodes_ = 0;
}

template<typename Key, typename Value>
TreeNode<Key, Value>* BinarySearchTree<Key, Value>::maximum() const
{
    return maximum(root_);
}

template<typename Key, typename Value>
TreeNode<Key, Value>* BinarySearchTree<Key, Value>::maximum(TreeNode<Key, Value>* node) const
{
    if (node == nullptr)
    {
        return nullptr;
    }

    auto x{ node };
    while (x->right_ != nullptr)
    {
        x = x->right_;
    }
    return x;
}

template<typename Key, typename Value>
TreeNode<Key, Value>* BinarySearchTree<Key, Value>::minimum() const
{
    return minimum(root_);
}

template<typename Key, typename Value>
TreeNode<Key, Value>* BinarySearchTree<Key, Value>::minimum(TreeNode<Key, Value>* node) const
{
    if (node == nullptr)
    {
        return nullptr;
    }

    auto x{ node };
    while (x->left_ != nullptr)
    {
        x = x->left_;
    }
    return x;
}

template<typename Key, typename Value>
TreeNode<Key, Value>* BinarySearchTree<Key, Value>::successor(TreeNode<Key, Value>* node) const
{
    if (node == nullptr)
    {
        return nullptr;
    }

    if (node->right_ != nullptr)
    {
        return minimum(node->right_);
    }

    auto x{ node };
    auto y{ x->parent_ };
    while (y != nullptr and x == y->right_)
    {
        x = y;
        y = y->parent_;
    }
    return y;
}

template<typename Key, typename Value>
TreeNode<Key, Value>* BinarySearchTree<Key, Value>::predecessor(TreeNode<Key, Value>* node) const
{
    if (node == nullptr)
    {
        return nullptr;
    }

    if (node->left_ != nullptr)
    {
        return maximum(node->left_);
    }

    auto x{ node };
    auto y{ x->parent_ };
    while (y != nullptr and x == y->left_)
    {
        x = y;
        y = y->parent_;
    }
    return y;
}

template<typename Key, typename Value>
bool BinarySearchTree<Key, Value>::isInTree(TreeNode<Key, Value>* node) const
{
    if (node == nullptr or root_ == nullptr)
    {
        return false;
    }

    auto x{ node };
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
TreeNode<Key, Value>* BinarySearchTree<Key, Value>::find(const Key& key) const
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
bool BinarySearchTree<Key, Value>::insert(const value_type& value)
{
    TreeNode<Key, Value>* x{ root_ };
    TreeNode<Key, Value>* parent{ nullptr };

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
            // Key alreydy exists in the tree
            return false;
        }
    }

    TreeNode<Key, Value>* node{
        new TreeNode<Key, Value>{ value.first, value.second,
                                  parent, nullptr, nullptr }};

    if (parent == nullptr)
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

    return true;
}

template<typename Key, typename Value>
typename BinarySearchTree<Key, Value>::size_type BinarySearchTree<Key, Value>::erase(const Key& key)
{
    auto node{ find(key) };
    if (node == nullptr)
    {
        return 0;
    }

    if (node->left_ == nullptr)
    {
        transplant(node, node->right_);
    }
    else if (node->right_ == nullptr)
    {
        transplant(node, node->left_);
    }
    else
    {
        TreeNode<Key, Value>* y{ minimum(node->right_) };
        if (y->parent_ != node)
        {
            transplant(y, y->right_);
            y->right_ = node->right_;
            y->right_->parent_ = y;
        }
        transplant(node, y);
        y->left_ = node->left_;
        y->left_->parent_ = y;
    }

    delete node;
    --nodes_;
    return 1;
}

template<typename Key, typename Value>
void BinarySearchTree<Key, Value>::transplant(TreeNode<Key, Value>* u,
                                  TreeNode<Key, Value>* v)
{
    if (u == nullptr)
    {
        return;
    }

    if (u->parent_ == nullptr)
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

    if (v != nullptr)
    {
        v->parent_ = u->parent_;
    }
}

template<typename Key, typename Value>
void BinarySearchTree<Key, Value>::print() const
{
    const int NODE_WIDTH{ 3 };
    const int NODE_SPACE{ 1 };

    std::vector<TreeNode<Key, Value>*> nodeList;
    int h{ height() };
    for (int level{ 0 }; level <= h; ++level)
    {
        std::vector<TreeNode<Key, Value>*> nodeRow;

        if (level == 0)
        {
            nodeRow.push_back(root_);
        }
        else
        {
            for (unsigned int i{ 0 }; i < nodeList.size(); ++i)
            {
                TreeNode<Key, Value>* x{ nodeList[i] };
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
                std::cout << std::right << nodeRow[i]->key_;
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

#endif // BINARYSEARCHTREE_CPP
