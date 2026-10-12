#include <iostream>
using namespace std;

class Node
{
public:
    int data;
    Node* left;
    Node* right;

    Node(int val)
    {
        data = val;
        left = right = nullptr;
    }
};

class BST
{
private:
    Node* root;

    Node* insert(Node* root, int val)
    {
        if (root == nullptr)
        {
            return new Node(val);
        }
        if (val < root->data)
        {
            root->left = insert(root->left, val);
        }
        else if (val > root->data)
        {
            root->right = insert(root->right, val);
        }
        return root;
    }

    void inorder(Node* root)
    {
        if (root != nullptr)
        {
            inorder(root->left);
            cout << root->data << " ";
            inorder(root->right);
        }
    }

    void preorder(Node* root)
    {
        if (root != nullptr)
        {
            cout << root->data << " ";
            preorder(root->left);
            preorder(root->right);
        }
    }

    void postorder(Node* root)
    {
        if (root != nullptr)
        {
            postorder(root->left);
            postorder(root->right);
            cout << root->data << " ";
        }
    }

public:
    BST()
    {
        root = nullptr;
    }

    void insert(int val)
    {
        root = insert(root, val);
    }

    void inorder()
    {
        cout << "Inorder Traversal: ";
        inorder(root);
        cout << endl;
    }

    void preorder()
    {
        cout << "Preorder Traversal: ";
        preorder(root);
        cout << endl;
    }

    void postorder()
    {
        cout << "Postorder Traversal: ";
        postorder(root);
        cout << endl;
    }
};

int main()
{
    BST tree;

    tree.insert(55);
    tree.insert(34);
    tree.insert(77);
    tree.insert(82);
    tree.insert(90);
    tree.insert(130);
    tree.insert(61);

    tree.inorder();
    tree.preorder();
    tree.postorder();

    return 0;
}
/*PS D:\chaitanya thorat> cd "d:\chaitanya thorat\" ; if ($?) { g++ traversal.cpp -o traversal } ; if ($?) { .\traversal }
Inorder Traversal: 34 55 61 77 82 90 130 
Preorder Traversal: 55 34 77 61 82 90 130 
Postorder Traversal: 34 61 130 90 82 77 55 
PS D:\chaitanya thorat> */