// Standard binary search tree implementation
//
// Implementation is based on T.H. Cormen, C.E. Leiserson, R.L. Rivest, C. Stein,
// Introduction to algorithms, 3rd ed. MIT Press, Cambridge, MA, 2009, Chapter 12
//
// Ville Heikkilä

#ifndef BINARYSEARCHTREE_HH
#define BINARYSEARCHTREE_HH

struct TreeNode
{
    int key_;
    TreeNode* parent_;
    TreeNode* left_;
    TreeNode* right_;

    TreeNode(int key) :
        key_{ key },
        parent_{ nullptr },
        left_{ nullptr },
        right_{ nullptr } {}
};

class BinarySearchTree
{
public:
    BinarySearchTree();
    ~BinarySearchTree();

    int nodes() const;
    int height() const;
    int height(TreeNode* node) const;

    void clear();

    TreeNode* maximum() const;
    TreeNode* maximum(TreeNode* node) const;
    TreeNode* minimum() const;
    TreeNode* minimum(TreeNode* node) const;

    TreeNode* successor(TreeNode* node) const;
    TreeNode* predecessor(TreeNode* node) const;

    bool isInTree(TreeNode* node) const;

    TreeNode* search(int key) const;
    void insertNode(TreeNode* node);
    void deleteNode(TreeNode* node);

    void print() const;

private:
    TreeNode* root_;
    int nodes_;

    void transplant(TreeNode* u, TreeNode* v);
};

#endif // BINARYSEARCHTREE_HH
