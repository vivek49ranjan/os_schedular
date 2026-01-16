struct Node {
    int data;
    Node* left;
    Node* right;
    Node* parent;
    int colour;
    Node(int data) : data(data), colour(1), left(nullptr), right(nullptr), parent(nullptr) {}
};
int max(int a, intb){
    return a>b ? a: b;
}


int Node_depth(Node* Root){
    if(!Node) return 0;
    else return max(Node_depth(Root->left) , Node_depth(Root->right));
}

void left_rotate(Node* &root, Node* x) {
    Node* y = x->right;
    x->right = y->left;

    if (y->left)
        y->left->parent = x;

    y->parent = x->parent;

    if (!x->parent)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

void rightRotate(Node* &root, Node* y) {
    Node* x = y->left;
    y->left = x->right;

    if (x->right)
        x->right->parent = y;

    x->parent = y->parent;

    if (!y->parent)
        root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;

    x->right = y;
    y->parent = x;
}

void fixInsert(Node* &root, Node* z) {
    while (z != root && z->parent->color == RED) {

        Node* parent = z->parent;
        Node* grandparent = parent->parent;

        if (parent == grandparent->left) {
            Node* uncle = grandparent->right;

            if (uncle && uncle->color == RED) {
                parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                z = grandparent;
            }
            else {
                if (z == parent->right) {
                    z = parent;
                    leftRotate(root, z);
                }
                parent->color = BLACK;
                grandparent->color = RED;
                rightRotate(root, grandparent);
            }
        }

        else {
            Node* uncle = grandparent->left;

            if (uncle && uncle->color == RED) {
                parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                z = grandparent;
            }
            else {
                if (z == parent->left) {
                    z = parent;
                    rightRotate(root, z);
                }
                parent->color = BLACK;
                grandparent->color = RED;
                leftRotate(root, grandparent);
            }
        }
    }

    root->color = BLACK; 
}
void insert(Node* &root, int key) {
    Node* z = new Node(key);
    Node* y = nullptr;
    Node* x = root;

    while (x) {
        y = x;
        if (z->key < x->key)
            x = x->left;
        else
            x = x->right;
    }

    z->parent = y;

    if (!y)
        root = z;
    else if (z->key < y->key)
        y->left = z;
    else
        y->right = z;

    fixInsert(root, z);
}
