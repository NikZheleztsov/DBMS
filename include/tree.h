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

    void height (node<Y, T>* nd);

    node<Y, T>* small_l_rot (node<Y, T>* nd);
    node<Y, T>* small_r_rot (node<Y, T>* nd);
    node<Y, T>* big_l_rot (node<Y, T>* nd);
    node<Y,T>* big_r_rot(node<Y, T>* nd);
    node<Y,T>* balance (node<Y, T>* nd, tree<Y, T>& tr, node<Y,T>* top);
    node<Y,T>* push_in(tree<Y, T>& tr, node<Y, T>* nd, T data, Y key, node<Y, T>* top);
    node<Y,T>* l_max (node<Y, T>* nd);
    bool is_leaf (node<Y,T>* nd);
    void del_in(tree<Y,T>& tr, node<Y, T>* nd, Y key, node<Y, T>* top);
    bool is_number (char num);
    node<Y,T>* destruct_in (node<Y, T>* nd, node<Y, T>* top);

public:

    tree();
    void push (tree<Y, T>& tr, T data, Y key);
    void del(tree<Y,T>& tr, Y key);
    T search (Y key);
    ~tree();
};

