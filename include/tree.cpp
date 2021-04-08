#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <iomanip>

template <typename Y, typename T>
struct node
{
    T data;
    Y key;
    node* top;
    node* left;
    node* right;
    int dif; // right - left
    int height;
};

template < typename Y, typename T>
class tree
{
    node<Y,T>* root;

    void height (node<Y, T>* nd) // calculating height based on bottom nodes
    {
        if (nd)
        {
            if (nd->right != nullptr && nd->left != nullptr)
            {
                nd->height = std::max(nd->right->height, nd->left->height) + 1;
                nd->dif = nd->right->height - nd->left->height;

            } else if (nd->right == nullptr && nd->left != nullptr)
            {
                nd->height = nd->left->height + 1;
                nd->dif = 0 - nd->left->height;

            } else if (nd->right != nullptr && nd->left == nullptr)
            {
                nd->height = nd->right->height + 1;
                nd->dif = nd->right->height;

            } else if (nd->right == nullptr && nd->left == nullptr)
            {
                nd->height = 1;
                nd->dif = 0;
            }
        }
    }

    node<Y, T>* small_l_rot (node<Y, T>* nd) 
    {
        node<Y, T>* temp = nd->right; //always exists

        nd->right = temp->left;
        if (temp->left)
            temp->left->top = nd;

        temp->left = nd;
        nd->top = temp;

        nd = temp;

        return nd;
    } 

    node<Y, T>* small_r_rot (node<Y, T>* nd)
    {
        node<Y, T>* temp = nd->left;

        nd->left = temp->right;
        if (temp->right)
            temp->right->top = nd;

        temp->right = nd;
        nd->top = temp;

        nd = temp;

        return nd;
    }

    node<Y, T>* big_l_rot (node<Y, T>* nd)
    {
        nd->right = small_r_rot(nd->right);
        nd = small_l_rot(nd);

        return nd;
    }

    node<Y,T>* big_r_rot(node<Y, T>* nd)
    {
        nd->left = small_l_rot(nd->left);
        nd = small_r_rot(nd);

        return nd;
    }


    node<Y,T>* balance (node<Y, T>* nd, tree<Y, T>& tr, node<Y,T>* top)
    {
        if (nd)
        {
            height(nd);

            bool root_flag = 0;
            if (nd == tr.root)
                root_flag = 1;

            if (nd->dif == 2 && ((nd->right == nullptr ? 1 : (nd->right->dif == 0 ? 1 : 0)) //ternary op. to avoid leaks and for reliability
                        || (nd->right == nullptr ? 0 : (nd->right->dif == 1 ? 1 : 0)))) //algorithms from materials
            {
                nd = small_l_rot(nd);
                nd->top = top;
                height(nd->left);
                height(nd);

                if (root_flag)
                    tr.root = nd;

            } 

            if (nd->right != nullptr)
            {
                if (nd->dif == 2 && nd->right->dif < 0 )
                {
                    nd = big_l_rot(nd);
                    nd->top = top;
                    height(nd->left);
                    height(nd->right);
                    height(nd);

                    if (root_flag)
                        tr.root = nd;
                }
            }

            if (nd->dif == -2 && ((nd->left == nullptr ? 1 : (nd->left->dif == 0 ? 1 : 0))
                        || (nd->left == nullptr ? 0 : (nd->left->dif  == -1 ? 1 : 0))))
            {
                nd = small_r_rot(nd);
                nd->top = top;
                height(nd->right);
                height(nd);

                if (root_flag)
                    tr.root = nd;

            } 

            if (nd->left != nullptr) 
            {
                if (nd->dif == -2 && nd->left->dif > 0 )
                {
                    nd = big_r_rot(nd);
                    nd->top = top;
                    height(nd->left);
                    height(nd->right);
                    height(nd);

                    if (root_flag)
                        tr.root = nd;
                }
            }

        }

        return nd;
    }

    node<Y,T>* push_in(tree<Y, T>& tr, node<Y, T>* nd, T data, Y key, node<Y, T>* top)
    {
        if (nd == nullptr)
        {
            nd = new node<Y,T>;
            nd->data = data;
            nd->key = key;
            nd->left = nullptr;
            nd->right = nullptr;
            nd->height = 1;
            nd->top = top;
            nd->dif = 0;

            if (tr.root == nullptr)
                tr.root = nd;

            return nd;

        } else if (nd->key == key)
        {
            std::cout << "Similar keys are forbidden!\n";
            return nd;

        } else if (key > nd->key)
        {
            nd->right = push_in(tr, nd->right, data, key, nd);
            nd = balance(nd, tr, top);
            return nd;
        } else 
        {
            nd->left = push_in(tr, nd->left, data, key, nd);
            nd = balance(nd, tr, top);
            return nd;
        }
    }

    node<Y,T>* l_max (node<Y, T>* nd)
    {
        node<Y,T>* max = nd;

        do {

            if (nd->key > max->key)
                max = nd;
            nd = nd->right;

        } while (nd != nullptr);

        return max;
    }

    bool is_leaf (node<Y,T>* nd)
    {
        if (nd->left == nullptr && nd->right == nullptr)
            return true;
        else 
            return false;
    }

    void del_in(tree<Y,T>& tr, node<Y, T>* nd, Y key, node<Y, T>* top)
    {
        if (nd->key == key)
        {
            if (is_leaf(nd))
            {
                if (top)
                {
                    if (top->right == nd)
                        top->right = nullptr;
                    else 
                        top->left = nullptr;
                } else 
                    tr.root = nullptr;

                delete nd;

            } else {

                node<Y,T>* for_ch;
                if (nd->left)
                    for_ch = l_max(nd->left);
                else 
                    for_ch = nd->right;

                node<Y,T>* bal = for_ch->top;

                nd->data  = for_ch->data;
                nd->key = for_ch->key;

                if (is_leaf(for_ch))
                {
                    if (bal)
                    {
                        if (bal->right == for_ch)
                            bal->right = nullptr;
                        else 
                            bal->left = nullptr;
                    }

                    delete for_ch;

                } else {

                    for_ch->left->top = bal;
                    if (bal->right == for_ch)
                        bal->right = for_ch->left;
                    else 
                        bal->left = for_ch->left;
                    delete for_ch;
                }

                //balancing from for_ch
                if (bal)
                {
                    while (bal != nullptr)
                    {
                        if (bal->top) 
                        {
                            if (bal == nd->top)
                                break;

                            if (bal->top->right == bal)
                                bal->top->right = balance(bal, tr, bal->top);
                            else 
                                bal->top->left = balance(bal, tr, bal->top);

                        } else {
                            node<Y,T>* null = nullptr;
                            balance(bal,tr, null);
                            break;
                        }

                        bal = bal->top;
                    }
                }
            }

        } else if (key > nd->key && nd->right != nullptr)
        {
            //nd->right = del_in(tr, nd->right, key, nd);
            del_in(tr, nd->right, key, nd);
            nd = balance(nd, tr, top);

        } else if (key < nd->key && nd->left != nullptr)
        {
            //nd->left = del_in(tr, nd->left, key, nd);
            del_in(tr, nd->left, key, nd);
            nd = balance(nd, tr, top);
        }
    }

    bool is_number (char num)
    {
        try  {
            std::stoi(static_cast<std::string>(&num));
            return 1;
        } catch (...) {
            return (num == '\\' || num == '/');
        }
    }

    node<Y,T>* destruct_in (node<Y, T>* nd, node<Y, T>* top)
    {
        if (is_leaf(nd))
        {
            delete nd;
            return nullptr;
        }

        if (nd->right != nullptr)
            nd->right = destruct_in(nd->right, nd);
        if (nd->left != nullptr)
            nd->left = destruct_in(nd->left, nd);
    }

    T search_in (Y key, node<Y,T>* nd)
    {
        if (nd->key == key)
            return nd->data;

        else if (key > nd->key && nd->right != nullptr)
        {
            search_in(key, nd->right);

        } else if (key < nd->key && nd->left != nullptr)
        {
            search_in(key, nd->left);
        }
    }

public:

    tree()
    {
        root = nullptr;
    }

    void push (tree<Y, T>& tr, T data, Y key)
    {
        push_in(tr, tr.root, data, key, nullptr);
    }

    void del(tree<Y,T>& tr, Y key)
    {
        if (tr.root != nullptr)
        {
            node<Y,T>* null = nullptr;
            del_in(tr, tr.root, key, null);

        } else 
            std::cout << "Empty tree\n";
    }

    T search (Y key)
    {
        return search_in (key, root);
    }

    ~tree()
    {
        if (root != nullptr)
        {
            node<Y,T>* null = nullptr;
            root = destruct_in(root, null);
        }
    }

};
